using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using GMath;
using static GMath.Gfx;
using static GMath.GRandom;
using static GMath.GTools;

namespace SphereScattering
{
    class Program
    {
        #region CVAE samples data set

        static MediumSettings GenerateNewSettings(GRandom rnd)
        {
            float densityRnd = rnd.random();
            float scatterAlbedoRnd = rnd.random();
            float gRnd = rnd.random();

            float density = pow(densityRnd, 3) * 300; // densities varies from 0 to 300 concentrated at 0.
            float scatterAlbedo = min(1, 1.000001f - pow(scatterAlbedoRnd, 6)); // transform the albedo testing set to vary really slow close to 1.
            float g = clamp(gRnd * 2 - 1, -0.999f, 0.999f); // avoid singular cases abs(g)=1

            return new MediumSettings
            {
                Sigma = density,
                Phi = scatterAlbedo,
                G = g
            };
        }

        static void GenerateDatasetForTrainingCVAE()
        {
            const int N = 1 << 22;

            StreamWriter writer = new StreamWriter("ScattersDataSet.ds");
            Console.WriteLine("Generating file...");

            GRandom rnd = new GRandom();

            for (int i = 0; i < N; i++)
            {
                if (i % 1000 == 0)
                {
                    Console.Write("\r                                                ");
                    Console.Write("\rCompleted... " + (i * 100.0f / N).ToString("F2"));
                }

                var settings = GenerateNewSettings(rnd);
                var r = Scattering.GetVPTSampleInSphere(settings, rnd);

                // code path variables in a compact way
                float3 zAxis = float3(0, 0, 1);
                float3 xAxis = abs(r.x.z) > 0.999 ? float3(1, 0, 0) : normalize(cross(r.x, float3(0, 0, 1)));
                float3 yAxis = cross(zAxis, xAxis);

                float3x3 normR = transpose(float3x3(xAxis, yAxis, zAxis));
                float3 normx = mul(r.x, normR);
                float3 normw = mul(r.w, normR);
                float3 normX = mul(r.X, normR);
                float3 normW = mul(r.W, normR);
                float3 B = float3(1, 0, 0);
                float3 T = cross(normx, B);
                float costheta = normx.z;
                float beta = dot(normw, T);
                float alpha = dot(normw, B);

                writer.WriteLine("{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}, {10}, {11}, {12}",
                    settings.Sigma,
                    settings.G,
                    settings.Phi,
                    r.N,
                    costheta,
                    beta,
                    alpha,
                    normX.x,
                    normX.y,
                    normX.z,
                    normW.x,
                    normW.y,
                    normW.z
                    );
            }
            Console.WriteLine();

            writer.Close();
            Console.WriteLine("Done.");
        }

        #endregion

        #region Extended Tabular method samples

        static MediumSettings GenerateSettingsForX(GRandom rnd)
        {
            float densityRnd = rnd.random();
            float scatterAlbedoRnd = rnd.random();
            float gRnd = rnd.random();

            float density = pow(2, densityRnd*7)-1; // densities varies from 0 to 127 concentrated at 0.
            //float scatterAlbedo = min(1, 1.000001f - pow(scatterAlbedoRnd, 6)); // transform the albedo testing set to vary really slow close to 1.
            float g = clamp(gRnd * 2 - 1, -0.999f, 0.999f); // avoid singular cases abs(g)=1

            return new MediumSettings
            {
                Sigma = density,
                Phi = 1,
                G = g
            };
        }

        static void GenerateDatasetForTabulationXMethod()
        {
            int total = 0;

            Parallel.For(0, 8, file =>
            {
                BinaryWriter writer = new BinaryWriter(new FileStream("DataSet" + file + ".bin", FileMode.Create));

                GRandom rnd = new GRandom(file);

                int N = 1 << 24; // 16 million samples per file

                while (N-- > 0)
                {
                    var settings = GenerateSettingsForX(rnd);

                    var r = Scattering.GetVPTSampleInSphere(settings, rnd);

                    // code path variables in a compact way
                    float3 zAxis = float3(0, 0, 1);
                    float3 xAxis = abs(r.x.z) > 0.999 ? float3(1, 0, 0) : normalize(cross(r.x, float3(0, 0, 1)));
                    float3 yAxis = cross(zAxis, xAxis);

                    float3x3 normR = transpose(float3x3(xAxis, yAxis, zAxis));
                    float3 normx = mul(r.x, normR);
                    float3 normw = mul(r.w, normR);
                    float3 normX = mul(r.X, normR);
                    float3 normW = mul(r.W, normR);
                    float3 B = float3(1, 0, 0);
                    float3 T = cross(normx, B);
                    float costheta = normx.z;
                    float beta = dot(normw, T);
                    float alpha = dot(normw, B);

                    writer.Write(settings.Sigma);
                    writer.Write(settings.G);
                    writer.Write(r.N);
                    writer.Write(costheta);
                    writer.Write(beta);
                    writer.Write(alpha);

                    Interlocked.Add(ref total, 1);

                    if (total % 100000 == 0)
                        Console.WriteLine("Overall progress: {0}%", total * 100.0f / (8 * (1 << 24)));
                }

                writer.Close();
            }); 
        }

        #endregion

        #region Tabular samples data set

        // HG factor [-1,1] linear
        const int BINS_G = 200;

        // Scattering albedo [0, 0.999] linear in log(1 / (1 - alpha))
        const int BINS_SA = 1000;

        // Radius [1, 256] linear in log(r)
        const int BINS_R = 9;

        // Theta [0,pi] linear in cos(theta)
        const int BINS_THETA = 45;

        const int STRIDE_G = (BINS_SA * BINS_R * BINS_THETA);
        const int STRIDE_SA = (BINS_R * BINS_THETA);
        const int STRIDE_R = (BINS_THETA);

        static void GenerateDatasetForTabulationMethod()
        {
            /*
             * This method generates data samples and builds a table using 
             * the idea in Mueller et al 2016.
             */
            float[] table = new float[BINS_G * BINS_SA * BINS_R * BINS_THETA];
            float[,,] oneTimeScatteringAlbedo = new float[BINS_G, BINS_SA, BINS_R];
            float[,,] multiTimeScatteringAlbedo = new float[BINS_G, BINS_SA, BINS_R];

            int MAX_N = 200;
            int P = 1 << 18; // one quarter million photons per setting.

            Console.WriteLine("Process started.");
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();

            int TOTAL_NUMBER_OF_SETTINGS = BINS_G * BINS_R;
            int solvedSettings = 0;
            Parallel.For(0, BINS_G, binG =>
            {
                GRandom rnd = new GRandom(binG);
                
                float g = (binG + rnd.random()) * 2.0f * 0.99f / BINS_G - 0.99f;
                for (int binR = 0; binR < BINS_R; binR++)
                {
                    float r = pow(2.0f, binR);

                    Console.WriteLine("Solved {0}%.", solvedSettings * 100.0f / TOTAL_NUMBER_OF_SETTINGS);

                    if (solvedSettings >= 1)
                    {
                        Console.WriteLine("ETA {0} ", stopwatch.Elapsed * (TOTAL_NUMBER_OF_SETTINGS / (float)solvedSettings - 1));
                    }

                    MediumSettings settings = new MediumSettings();
                    settings.Sigma = r;
                    settings.Phi = 1;
                    settings.G = g;

                    long[,] Hl = new long[BINS_THETA, MAX_N + 1];

                    for (int sample = 0; sample < P; sample++)
                    {
                        var summary = Scattering.GetVPTSampleInSphereOffcenter(settings, rnd);

                        int thetaBin = (int)min(BINS_THETA - 1, BINS_THETA * (summary.x.z * 0.5f + 0.5f));
                        int nBin = (int)min(MAX_N, summary.N);
                        Hl[thetaBin, nBin]++;
                    }

                    for (int binSA = 0; binSA < BINS_SA; binSA++)
                    {
                        float phi = binSA == BINS_SA - 1 ? 1.0f : 1 - exp(binSA * log(0.001f) / (BINS_SA - 1));

                        for (int binTheta = 0; binTheta < BINS_THETA; binTheta++)
                        {
                            int pos = STRIDE_G * binG + STRIDE_SA * binSA + STRIDE_R * binR + binTheta;
                            for (int n = 2; n <= MAX_N; n++)
                            {
                                table[pos] += pow(phi, n) * Hl[binTheta, n];
                                multiTimeScatteringAlbedo[binG, binSA, binR] += pow(phi, n) * Hl[binTheta, n];
                            }

                            oneTimeScatteringAlbedo[binG, binSA, binR] += phi * Hl[binTheta, 1];
                        }

                        // Sum and normalize table slice to create an empirical cdf
                        for (int binTheta = 0; binTheta < BINS_THETA; binTheta++)
                        {
                            int pos = STRIDE_G * binG + STRIDE_SA * binSA + STRIDE_R * binR + binTheta;

                            if (binTheta > 0)
                                table[pos] += table[pos - 1];
                        }
                    }

                    solvedSettings++;
                }
            });

            Console.WriteLine("Finished generation in {0}", stopwatch.Elapsed);

            Console.WriteLine("Saving table");

            BinaryWriter bw = new BinaryWriter(new FileStream("stf.bin", FileMode.Create));

            for (int binG = 0; binG < BINS_G; binG++)
                for (int binSA = 0; binSA < BINS_SA; binSA++)
                    for (int binR = 0; binR < BINS_R; binR++)
                        bw.Write(oneTimeScatteringAlbedo[binG, binSA, binR] / P);

            for (int binG = 0; binG < BINS_G; binG++)
                for (int binSA = 0; binSA < BINS_SA; binSA++)
                    for (int binR = 0; binR < BINS_R; binR++)
                        bw.Write(multiTimeScatteringAlbedo[binG, binSA, binR] / P);

            int index = 0;
            for (int binG = 0; binG < BINS_G; binG++)
                for (int binSA = 0; binSA < BINS_SA; binSA++)
                    for (int binR = 0; binR < BINS_R; binR++)
                        for (int binTheta = 0; binTheta < BINS_THETA; binTheta++)
                            bw.Write(table[index++] / P);

            bw.Close();

            Console.WriteLine("Done.");
        }

        #endregion

        static void Main(string[] args)
        {
            //// Used to generate 4 million samples for CVAE training
            //GenerateDatasetForTrainingCVAE();

            //// Used to Generate Mueller data tables
            //GenerateDatasetForTabulationMethod();

            // Used to generate samples for extended table
            // Generates 8 files of 16 million samples each...
            GenerateDatasetForTabulationXMethod();
        }
    }
}

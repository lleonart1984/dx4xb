using System;
using System.Collections.Generic;
using System.Text;
using GMath;
using static GMath.Gfx;
using static GMath.GRandom;
using static GMath.GTools;

namespace SphereScattering
{
    /// <summary>
    /// Gets the variables for a final state of a path in a spherical medium
    /// </summary>
    public class PathSummary
    {
        /// <summary>
        /// Number of scatters. Allways >= 1 because the center produces the first scatter
        /// </summary>
        public int N;
        /// <summary>
        /// Final position in the surface of the unitary sphere
        /// </summary>
        public float3 x;
        /// <summary>
        /// Final direction leaving the medium
        /// </summary>
        public float3 w;
        /// <summary>
        /// A representative position where the path scatters. If X is the k-th position, then the pdf is Phi^k / (1 + Phi + ... +  Phi^N).
        /// </summary>
        public float3 X;
        /// <summary>
        /// The direction arriving to the represenative position
        /// </summary>
        public float3 W;
    }

    /// <summary>
    /// Settings of the medium ruling the experiment
    /// </summary>
    public class MediumSettings
    {
        /// <summary>
        /// Gets the density (extinction coefficient) of the medium
        /// </summary>
        public float Sigma;

        /// <summary>
        /// Gets the scattering albedo of the medium
        /// </summary>
        public float Phi;

        /// <summary>
        /// Gets the HG factor of the phase function
        /// </summary>
        public float G;
    }

    /// <summary>
    /// Allows to compute a path generation in a homogeneus sphere medium starting scatteing in the center.
    /// </summary>
    public class Scattering
    {
        static float invertcdf(float g, float xi)
        {
            float t = (1.0f - g * g) / (1.0f - g + 2.0f * g * xi);
            return 0.5f * (1 + g * g - t * t) / g;
        }

        /// <summary>
        /// Implementation of the HG phase function.
        /// </summary>
        /// <param name="w">Incomming direction</param>
        /// <param name="g">Anisotropy factor</param>
        static float3 SamplePhase(float3 w, float g, GRandom rnd)
        {
            if (abs(g) < 0.001f)
                return rnd.randomDirection(-w);

            float phi = rnd.random() * 2 * pi;
            float cosTheta = invertcdf(g, rnd.random());
            float sinTheta = sqrt(max(0, 1.0f - cosTheta * cosTheta));

            createOrthoBasis(w, out float3 t0, out float3 t1);

            return sinTheta * sin(phi) * t0 + sinTheta * cos(phi) * t1 +
                cosTheta * w;
        }

        /// <summary>
        /// Distance needed to leave the sphere from x in direction d.
        /// </summary>
        static float DistanceToBoundary(float3 x, float3 d)
        {
            //float a = dot(d,d); <- 1 because d is normalized
            float b = 2 * dot(x, d);
            float c = dot(x, x) - 1;

            float Disc = b * b - 4 * c;

            if (Disc <= 0)
                return 0;

            // Assuming x is inside the sphere, only the positive root is needed (intersection forward w).
            return max(0, (-b + sqrt(Disc)) / 2);
        }

        /// <summary>
        /// Performs a VPT in a sphere medium with specific settings starting at center and returns the summary of the path traced.
        /// </summary>
        public static PathSummary GetVPTSampleInSphere(MediumSettings settings, GRandom rnd)
        {
            float3 x = float3(0, 0, 0);
            float3 w = float3(0, 0, 1);
            float3 X = x;
            float3 W = w;
            int N = 0;
            float accum = 0;
            float importance = 1;

            while (true)
            {
                importance *= settings.Phi;
                accum += importance;

                if (rnd.random() < importance / accum) // replace the representative by this one
                {
                    X = x;
                    W = w;
                }

                w = SamplePhase(w, settings.G, rnd);

                N++;

                float d = DistanceToBoundary(x, w);

                float t = settings.Sigma < 0.00001 ? 10000000 : -log(max(0.000000001f, 1 - rnd.random())) / settings.Sigma;

                if (t >= d || float.IsNaN(t) || float.IsInfinity(t))
                {
                    x += w * d;
                    return new PathSummary
                    {
                        N = N,
                        x = x,
                        w = w,
                        X = X,
                        W = W
                    };
                }
                x += w * t;
            }
        }

        /// <summary>
        /// Performs a VPT in a sphere medium with specific settings starting at center and returns the summary of the path traced.
        /// </summary>
        public static PathSummary GetVPTSampleInSphereOffcenter(MediumSettings settings, GRandom rnd)
        {
            float3 x = float3(0, 0, 0);
            float3 w = float3(0, 0, 1);
            float3 X = x;
            float3 W = w;
            int N = 0;
            float accum = 0;
            float importance = 1;

            // First flight from center
            float t = settings.Sigma < 0.00001 ? 10000000 : -log(max(0.000000001f, 1 - rnd.random())) / settings.Sigma;

            if (t >= 1.0f) // leaves the sphere before scattering
            {
                return new PathSummary
                {
                    N = N,
                    x = w,
                    w = w,
                    X = x, // can not used but in any case...
                    W = W
                };
            }
            x += w * t; // move to first scatter offcenter

            while (true)
            {
                importance *= settings.Phi;
                accum += importance;

                if (rnd.random() < importance / accum) // replace the representative by this one
                {
                    X = x;
                    W = w;
                }

                w = SamplePhase(w, settings.G, rnd);

                N++;

                float d = DistanceToBoundary(x, w);

                t = settings.Sigma < 0.00001 ? 10000000 : -log(max(0.000000001f, 1 - rnd.random())) / settings.Sigma;

                if (t >= d || float.IsNaN(t) || float.IsInfinity(t))
                {
                    x += w * d;
                    return new PathSummary
                    {
                        N = N,
                        x = x,
                        w = w,
                        X = X,
                        W = W
                    };
                }
                x += w * t;
            }
        }
    }
}

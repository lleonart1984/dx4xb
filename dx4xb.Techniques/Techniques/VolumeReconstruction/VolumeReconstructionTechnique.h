#include <vector>

class VolumeReconstructionTechnique : public Technique, public IManageScene, public IGatherImageStatistics {

	struct VR_Pipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeReconstruction\\View_Reconstruction_CS.cso"));
		}

		gObj<Buffer> Parameters;
		
		gObj<Buffer> Camera;
		struct AccumulativeInfoCB {
			int Pass;
			int ShowComplexity;
			float PathtracingRatio;
			int Seed;
		} AccumulativeInfo;
		gObj<Buffer> Lighting;

		gObj<Texture2D> Output;
		gObj<Texture2D> Accumulation;
		gObj<Texture2D> SqrAccumulation;
		gObj<Texture2D> Complexity;

		gObj<Texture2D> RngStates;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->Space(0);
			binder->SRV(0, Parameters);

			binder->CBV(0, Camera);
			binder->CBV(1, AccumulativeInfo);
			binder->CBV(2, Lighting);

			binder->UAV(0, Output);
			binder->UAV(1, Accumulation);
			binder->UAV(2, SqrAccumulation);
			binder->UAV(3, Complexity);

			binder->Space(2);
			binder->UAV(0, RngStates);
		}
	};

	struct PTResult {
		float3 Origin;
		float3 Direction;
		float3 Radiance;
	};

	struct FB_Pipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeReconstruction\\ForwardBackward_CS.cso"));
		}

		gObj<Buffer> Parameters;
		gObj<Buffer> Samples;
		gObj<Buffer> Gradients;
		
		gObj<Buffer> FixCamera;
		struct AccumulativeInfoCB {
			int Pass;
			int ShowComplexity;
			float PathtracingRatio;
			int Seed;
		} AccumulativeInfo;
		gObj<Buffer> FixLighting;

		gObj<Texture2D> RngStates;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->Space(0);
			binder->SRV(0, Parameters);
			binder->SRV(1, Samples);

			binder->CBV(0, FixCamera);
			binder->CBV(1, AccumulativeInfo);
			binder->CBV(2, FixLighting);

			binder->UAV(0, Gradients);

			binder->Space(2);
			binder->UAV(0, RngStates);
		}
	};


	struct ZeroGrad_Pipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeReconstruction\\ZeroGrad_CS.cso"));
		}

		gObj<Buffer> Gradients;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->Space(0);
			binder->UAV(0, Gradients);
		}
	};

	struct OptimizerStep_Pipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeReconstruction\\OptimizerStep_CS.cso"));
		}

		gObj<Buffer> Gradients;
		gObj<Buffer> Parameters;

		float LR;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->Space(0);
			binder->UAV(0, Parameters);
			binder->SRV(0, Gradients);
			binder->CBV(0, LR);
		}
	};


	gObj<VR_Pipeline> view_pipeline;
	gObj<FB_Pipeline> fb_pipeline;
	gObj<ZeroGrad_Pipeline> zerograd_pipeline;
	gObj<OptimizerStep_Pipeline> optimizer_pipeline;

public:
	~VolumeReconstructionTechnique() {}

	struct LightingCB {
		float3 LightPosition;
		float rem0;
		float3 LightIntensity;
		float rem1;
		float3 LightDirection;
		float rem2;
	};

	void getAccumulators(gObj<Texture2D>& sum, gObj<Texture2D>& sqrSum, int& frames)
	{
		sum = view_pipeline->Accumulation;
		sqrSum = view_pipeline->SqrAccumulation;
		frames = view_pipeline->AccumulativeInfo.Pass;
	}

	gObj<Buffer> parameters;
	gObj<Buffer> gradients;

	gObj<Buffer> LoadSamples() {
		std::vector<std::string> files = {
			"C:/Users/mendez/Documents/Datasets/Transmittance/camera_0.bin",
			//"C:/Users/mendez/Documents/Datasets/Transmittance/camera_1.bin",
			"C:/Users/mendez/Documents/Datasets/Transmittance/camera_2.bin",
			//"C:/Users/mendez/Documents/Datasets/Transmittance/camera_3.bin",
			"C:/Users/mendez/Documents/Datasets/Transmittance/camera_4.bin",
			//"C:/Users/mendez/Documents/Datasets/Transmittance/camera_5.bin",
			"C:/Users/mendez/Documents/Datasets/Transmittance/camera_top.bin",
			"C:/Users/mendez/Documents/Datasets/Transmittance/camera_bottom.bin",
		};

		std::vector<PTResult> samples;

		for (auto filename : files) {
			FILE* f;
			if (fopen_s(&f, filename.c_str(), "rb"))
				throw Exception::FromError(Errors::Invalid_Operation, "Dataset file not found");

			int width, height;
			fread_s(&width, 4, 4, 1, f);
			fread_s(&height, 4, 4, 1, f);

			int N = width * height;
			for (int i = 0; i < N; i++)
			{
				float row[10];
				fread_s(row, 40, 4, 10, f);
				PTResult entry;
				entry.Origin = float3(row[0], row[1], row[2]);
				entry.Direction = float3(row[3], row[4], row[5]);
				entry.Radiance = float4(row[8], row[7], row[6], row[9]);
				samples.push_back(entry);
			}
			fclose(f);
		}
		gObj<Buffer> result = CreateBufferSRV<PTResult>(samples.size());
		result->Write((byte*)samples.data());

		return result;
	}

	virtual void OnLoad() override {

		Load(view_pipeline); // loads the pipeline.
		Load(fb_pipeline);
		Load(zerograd_pipeline);
		Load(optimizer_pipeline);

		parameters = CreateBufferUAV<float>(RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE);
		gradients = CreateBufferUAV <uint> (RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE);
		float* initial_parameters = new float[RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE];
		//initial_parameters[0] = 100.0f; // initial majorant
		//initial_parameters[1] = 1.0f; // initial albedo
		//initial_parameters[2] = 0.875f*0.6f; // initial gfactor
		for (int i = 0; i < RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE; i++)
			initial_parameters[i] = 0.0 * rand() / (float)RAND_MAX;// 0.05 * rand() / (float)RAND_MAX; // initial densities
		parameters->Write(initial_parameters);
		delete[] initial_parameters;

		view_pipeline->Parameters = parameters;
		
		view_pipeline->Output = CreateTexture2DUAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		view_pipeline->Accumulation = CreateTexture2DUAV<float4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		view_pipeline->SqrAccumulation = CreateTexture2DUAV<float4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), 1, 1);
		view_pipeline->Complexity = CreateTexture2DUAV<uint>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		view_pipeline->RngStates = CreateTexture2DUAV<uint4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		view_pipeline->Lighting = CreateBufferCB<LightingCB>();
		view_pipeline->Camera = CreateBufferCB<float4x4>();
		view_pipeline->AccumulativeInfo = {};

#ifdef SHOW_COMPLEXITY
		view_pipeline->AccumulativeInfo.ShowComplexity = 1;
		view_pipeline->AccumulativeInfo.PathtracingRatio = 0.5;
#else
		view_pipeline->AccumulativeInfo.ShowComplexity = 0;
		view_pipeline->AccumulativeInfo.PathtracingRatio = 0.0;
#endif

		fb_pipeline->FixCamera = CreateBufferCB<float4x4>();
		fb_pipeline->FixLighting = CreateBufferCB<LightingCB>();
		fb_pipeline->AccumulativeInfo = {};
		fb_pipeline->RngStates = view_pipeline->RngStates;
		fb_pipeline->Parameters = parameters;
		fb_pipeline->Gradients = gradients;
		fb_pipeline->Samples = LoadSamples();

		zerograd_pipeline->Gradients = gradients;

		optimizer_pipeline->Gradients = gradients;
		optimizer_pipeline->Parameters = parameters;
		optimizer_pipeline->LR = 0.0001f;

		Execute_OnGPU(LoadAssets);
	}

	void LoadAssets(gObj<GraphicsManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		
		manager->ToGPU(parameters);

		manager->ToGPU(fb_pipeline->Samples);

		float4x4 proj, view;
		scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
		fb_pipeline->FixCamera->Write(mul(inverse(proj), inverse(view)));
		manager->ToGPU(fb_pipeline->FixCamera);
		
		fb_pipeline->FixLighting->Write(LightingCB{
					scene->getMainLight().Position, 0,
					scene->getMainLight().Intensity, 0,
					scene->getMainLight().Direction, 0
			});
		manager->ToGPU(fb_pipeline->FixLighting);

		UpdateBuffers(manager, elements);

		/*for (int epoch = 0; epoch < 40; epoch++) {
			fb_pipeline->AccumulativeInfo.Pass++;
			manager->SetPipeline(zerograd_pipeline);
			manager->Dispatch((int)ceil((RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE) / 1024.0));
			manager->SetPipeline(fb_pipeline);
			manager->Dispatch((int)ceil(fb_pipeline->Samples->ElementCount() / 1024.0));
			manager->SetPipeline(optimizer_pipeline);
			manager->Dispatch((int)ceil((RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE) / 1024.0));
			optimizer_pipeline->LR *= 0.999;
		}*/
	}

	virtual void UpdateBuffers(gObj<GraphicsManager> manager, SceneElement elements) {
		auto desc = scene->getScene();

		if (+(elements & SceneElement::Camera))
		{
			float4x4 proj, view;
			scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
			view_pipeline->Camera->Write(mul(inverse(proj), inverse(view)));
			manager->ToGPU(view_pipeline->Camera);
		}

		if (+(elements & SceneElement::Lights))
		{
			view_pipeline->Lighting->Write(LightingCB{
					scene->getMainLight().Position, 0,
					scene->getMainLight().Intensity, 0,
					scene->getMainLight().Direction, 0
				});
			manager->ToGPU(view_pipeline->Lighting);
		}
	}

	virtual void OnDispatch() override {
		// Update dirty elements
		Execute_OnGPU(UpdateAssets);
		// Draw current Frame
		Execute_OnGPU(DrawScene);
	}

	void UpdateAssets(gObj<GraphicsManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		UpdateBuffers(manager, elements);
		if (elements != SceneElement::None)
			view_pipeline->AccumulativeInfo.Pass = 0; // restart frame for Pathtracing...
	}

	void DrawScene(gObj<GraphicsManager> manager) {

		if (view_pipeline->AccumulativeInfo.Pass == 0) // first frame after scene dirty
		{
			//pipeline->AccumulativeInfo.PathtracingRatio = 1;
			view_pipeline->AccumulativeInfo.Pass = 0;
			view_pipeline->AccumulativeInfo.Seed = rand();

			manager->ClearUAV(view_pipeline->Accumulation, uint4(0));
			manager->ClearUAV(view_pipeline->Complexity, uint4(0));
		}

		//if (view_pipeline->AccumulativeInfo.Pass == 0) 
		{
			manager->SetPipeline(zerograd_pipeline);
			manager->Dispatch((int)ceil((RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE) / 1024.0));
			manager->SetPipeline(fb_pipeline);
			manager->Dispatch((int)ceil(fb_pipeline->Samples->ElementCount() / 1024.0));
			manager->SetPipeline(optimizer_pipeline);
			manager->Dispatch((int)ceil((RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE * RECONSTRUCTION_MAX_SIZE) / 1024.0));
			optimizer_pipeline->LR *= 0.999;
			fb_pipeline->AccumulativeInfo.Pass++;
		}

		manager->SetPipeline(view_pipeline);
		manager->Dispatch((int)ceil(CurrentRenderTarget()->Width() / 32.0), (int)ceil(CurrentRenderTarget()->Height() / 32.0));

		manager->Copy(CurrentRenderTarget(), view_pipeline->Output);

		view_pipeline->AccumulativeInfo.Pass++;
	}
};
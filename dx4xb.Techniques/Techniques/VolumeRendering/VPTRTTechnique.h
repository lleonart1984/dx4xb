class VPTRTTechnique : public Technique, public IManageScene, public IGatherImageStatistics {

	struct VPTPipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\VPTRT_CS.cso"));
		}

		gObj<Texture3D> HGrid;

		gObj<Buffer> Camera;
		struct AccumulativeInfoCB {
			int Pass;
			int ShowComplexity;
			float PathtracingRatio;
			int Seed;
		} AccumulativeInfo;
		gObj<Buffer> VolumeMaterial;
		gObj<Buffer> Lighting;

		gObj<Texture2D> Output;
		gObj<Texture2D> Accumulation;
		gObj<Texture2D> SqrAccumulation;
		gObj<Texture2D> Complexity;

		gObj<Texture2D> RngStates;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->Space(0);
			binder->SRV(0, HGrid);
			binder->CBV(0, Camera);
			binder->CBV(1, AccumulativeInfo);
			binder->CBV(2, VolumeMaterial);
			binder->CBV(3, Lighting);

			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));

			binder->UAV(0, Output);
			binder->UAV(1, Accumulation);
			binder->UAV(2, SqrAccumulation);
			binder->UAV(3, Complexity);

			binder->Space(1);
			binder->SMP_Static(0, Sampler::Point(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));


			binder->Space(2);
			binder->UAV(0, RngStates);
		}
	};

	gObj<VPTPipeline> pipeline;

public:
	~VPTRTTechnique() {}

	struct VolumeMaterialCB {
		float3 Extinction; float pad0;
		float3 ScatteringAlbedo; float pad1;
		float3 G; float pad2;
	};

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
		sum = pipeline->Accumulation;
		sqrSum = pipeline->SqrAccumulation;
		frames = pipeline->AccumulativeInfo.Pass;
	}

	virtual void OnLoad() override {

		auto desc = scene->getScene();

		Load(pipeline); // loads the pipeline.

		if (desc->getGrids().Count > 0) {
			pipeline->HGrid = LoadTexture3DVarMipMap(desc->getGrids().Data[0]);
		}

		// Allocate Memory for scene elements
		pipeline->VolumeMaterial = CreateBufferCB<VolumeMaterialCB>();

		pipeline->Output = CreateTexture2DUAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		pipeline->Accumulation = CreateTexture2DUAV<float4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		pipeline->SqrAccumulation = CreateTexture2DUAV<float4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), 1, 1);
		pipeline->Complexity = CreateTexture2DUAV<uint>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		pipeline->RngStates = CreateTexture2DUAV<uint4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		pipeline->Lighting = CreateBufferCB<LightingCB>();
		pipeline->Camera = CreateBufferCB<float4x4>();
		pipeline->AccumulativeInfo = {};

#ifdef SHOW_COMPLEXITY
		pipeline->AccumulativeInfo.ShowComplexity = 1;
		pipeline->AccumulativeInfo.PathtracingRatio = 0.5;
#else
		pipeline->AccumulativeInfo.ShowComplexity = 0;
		pipeline->AccumulativeInfo.PathtracingRatio = 0.0;
#endif

		Execute_OnGPU(LoadAssets);
	}

	void LoadAssets(gObj<GraphicsManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		UpdateBuffers(manager, elements);
	}

	virtual void UpdateBuffers(gObj<GraphicsManager> manager, SceneElement elements) {
		auto desc = scene->getScene();

		if (+(elements & SceneElement::Materials))
		{
			int materialIndex = desc->Volumes().Data[0].MaterialIndex;
			VolumeMaterialCB volMat = { };
			volMat.Extinction = desc->VolumeMaterials().Data[materialIndex].Extinction;
			volMat.ScatteringAlbedo = desc->VolumeMaterials().Data[materialIndex].ScatteringAlbedo;
			volMat.G = desc->VolumeMaterials().Data[materialIndex].G;

			pipeline->VolumeMaterial->Write(volMat);
			manager->ToGPU(pipeline->VolumeMaterial);
		}

		if (+(elements & SceneElement::Textures)) {
			manager->ToGPU(pipeline->HGrid);
		}

		if (+(elements & SceneElement::Camera))
		{
			float4x4 proj, view;
			scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
			pipeline->Camera->Write(mul(inverse(proj), inverse(view)));
			manager->ToGPU(pipeline->Camera);
		}

		if (+(elements & SceneElement::Lights))
		{
			pipeline->Lighting->Write(LightingCB{
					scene->getMainLight().Position, 0,
					scene->getMainLight().Intensity, 0,
					scene->getMainLight().Direction, 0
				});
			manager->ToGPU(pipeline->Lighting);
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
			pipeline->AccumulativeInfo.Pass = 0; // restart frame for Pathtracing...
	}

	void DrawScene(gObj<GraphicsManager> manager) {

		if (pipeline->AccumulativeInfo.Pass == 0) // first frame after scene dirty
		{
			//pipeline->AccumulativeInfo.PathtracingRatio = 1;
			pipeline->AccumulativeInfo.Pass = 0;
			pipeline->AccumulativeInfo.Seed = rand();

			manager->ClearUAV(pipeline->Accumulation, uint4(0));
			manager->ClearUAV(pipeline->Complexity, uint4(0));
		}

		manager->SetPipeline(pipeline);

		manager->Dispatch((int)ceil(CurrentRenderTarget()->Width() / 32.0), (int)ceil(CurrentRenderTarget()->Height() / 32.0));

		manager->Copy(CurrentRenderTarget(), pipeline->Output);

		pipeline->AccumulativeInfo.Pass++;
	}
};
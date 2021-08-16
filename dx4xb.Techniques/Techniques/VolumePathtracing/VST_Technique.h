class VST_Technique : public Technique, public IManageScene, public IGatherImageStatistics {

#include "VolMipMap_Pipeline.h"
	gObj<VolMipMap> mipmapping;

#include "ComputeRadii_Pipeline.h"
	gObj<ComputeRadii> computingRadii;

#include "CacheRadiiAndAverage_Pipeline.h"
	gObj<CacheRadiiAndAverage> cachingRadiiAndAverage;

	struct VST_Pipeline : public ComputePipeline {

		void Setup() {
			//set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\VST_Hom_MC_CS.cso"));
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumePathtracing\\VST_Hom_CVAE_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> SphereGrid;

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
			binder->SRV(0, Grid);
			binder->SRV(1, SphereGrid);

			binder->CBV(0, Camera);
			binder->CBV(1, AccumulativeInfo);
			binder->CBV(2, VolumeMaterial);
			binder->CBV(3, Lighting);

			binder->SMP_Static(0, Sampler::PointWithoutMipMaps(D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP));

			binder->UAV(0, Output);
			binder->UAV(1, Accumulation);
			binder->UAV(2, SqrAccumulation);
			binder->UAV(3, Complexity);

			binder->Space(2);
			binder->UAV(0, RngStates);
		}
	};

	gObj<VST_Pipeline> pipeline;

	gObj<Texture3D> MinorantMipMap;
	gObj<Texture3D> MajorantMipMap;
	gObj<Texture3D> AverageMipMap;
	
	gObj<Texture3D> Tmp_Minorant;
	gObj<Texture3D> Tmp_Majorant;
	gObj<Texture3D> Tmp_Average;

public:
	~VST_Technique() {}

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

	int halfDim(int d) {
		return d / 2;
	}

	virtual void OnLoad() override {

		auto desc = scene->getScene();

		Load(pipeline); // loads the pipeline.
		Load(mipmapping);
		Load(computingRadii);
		Load(cachingRadiiAndAverage);

		if (desc->getGrids().Count > 0) {
			pipeline->Grid = LoadGrid(desc->getGrids().Data[0]);
			pipeline->SphereGrid = CreateTexture3DUAV<float2>(pipeline->Grid->Width(), pipeline->Grid->Height(), pipeline->Grid->Depth());

			this->MajorantMipMap = CreateTexture3DUAV<float>(halfDim(pipeline->Grid->Width()), halfDim(pipeline->Grid->Height()), halfDim(pipeline->Grid->Depth()), 0);
			this->MinorantMipMap = CreateTexture3DUAV<float>(halfDim(pipeline->Grid->Width()), halfDim(pipeline->Grid->Height()), halfDim(pipeline->Grid->Depth()), 0);
			this->AverageMipMap = CreateTexture3DUAV<float>(halfDim(pipeline->Grid->Width()), halfDim(pipeline->Grid->Height()), halfDim(pipeline->Grid->Depth()), 0);

			this->Tmp_Majorant = CreateTexture3DUAV<float>(halfDim(pipeline->Grid->Width()), halfDim(pipeline->Grid->Height()), halfDim(pipeline->Grid->Depth()));
			this->Tmp_Minorant = CreateTexture3DUAV<float>(halfDim(pipeline->Grid->Width()), halfDim(pipeline->Grid->Height()), halfDim(pipeline->Grid->Depth()));
			this->Tmp_Average = CreateTexture3DUAV<float>(halfDim(pipeline->Grid->Width()), halfDim(pipeline->Grid->Height()), halfDim(pipeline->Grid->Depth()));
			
			computingRadii->Radii = CreateTexture3DUAV<int>(pipeline->Grid->Width(), pipeline->Grid->Height(), pipeline->Grid->Depth());
			computingRadii->Majorant = this->MajorantMipMap;
			computingRadii->Minorant = this->MinorantMipMap;
			computingRadii->Average = this->AverageMipMap;
			computingRadii->Grid = pipeline->Grid;

			cachingRadiiAndAverage->Grid = pipeline->Grid;
			cachingRadiiAndAverage->Radii = computingRadii->Radii;
			cachingRadiiAndAverage->Average = AverageMipMap;
			cachingRadiiAndAverage->SphereGrid = pipeline->SphereGrid;
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
			manager->ToGPU(pipeline->Grid);

			// compute mip maps if grid is updated
			manager->SetPipeline(mipmapping);

			int w = MajorantMipMap->Width();
			int h = MajorantMipMap->Height();
			int d = MajorantMipMap->Depth();
			mipmapping->Src_Majorant = pipeline->Grid;
			mipmapping->Src_Minorant = pipeline->Grid;
			mipmapping->Src_Average = pipeline->Grid;
			mipmapping->Dst_Majorant = Tmp_Majorant;
			mipmapping->Dst_Minorant = Tmp_Minorant;
			mipmapping->Dst_Average = Tmp_Average;
			int currentMipLevel = 0;
			while (w > 1 && h > 1 && d > 1) {
				manager->Dispatch((int)ceil(w / 8.0), (int)ceil(h / 8.0), (int)ceil(d / 8.0));
				// Copy from Temporal Resources to Specific Mip.
				// This is because can not be bound at same time two mips from same resource as SRV and UAV.

				mipmapping->Src_Majorant = MajorantMipMap->SliceMips(currentMipLevel);
				mipmapping->Src_Minorant = MinorantMipMap->SliceMips(currentMipLevel);
				mipmapping->Src_Average = AverageMipMap->SliceMips(currentMipLevel);

				manager->Copy(mipmapping->Src_Majorant, 0, 0, 0, Tmp_Majorant, 0, 0, 0, w, h, d);
				manager->Copy(mipmapping->Src_Minorant, 0, 0, 0, Tmp_Minorant, 0, 0, 0, w, h, d);
				manager->Copy(mipmapping->Src_Average, 0, 0, 0, Tmp_Average, 0, 0, 0, w, h, d);
				
				w = halfDim(w);
				h = halfDim(h);
				d = halfDim(d);
				currentMipLevel++;
			}

			// Compute Radii using heuristics over mip maps
			manager->SetPipeline(computingRadii);
			manager->Dispatch((int)ceil(computingRadii->Radii->Width() / 8.0), (int)ceil(computingRadii->Radii->Height() / 8.0), (int)ceil(computingRadii->Radii->Depth() / 8.0));

			// Caching Radii as the real size and the average for the specific level
			manager->SetPipeline(cachingRadiiAndAverage);
			manager->Dispatch((int)ceil(cachingRadiiAndAverage->SphereGrid->Width() / 8.0), (int)ceil(cachingRadiiAndAverage->SphereGrid->Height() / 8.0), (int)ceil(cachingRadiiAndAverage->SphereGrid->Depth() / 8.0));
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
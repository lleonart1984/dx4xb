#pragma once

#include "dx4xb_scene.h"
#include "../../gui_traits.h"
#include "../Tools/Parameters.h"

using namespace dx4xb;

class VPTTechnique : public Technique, public IManageScene, public IGatherImageStatistics {

	struct ComputeRadii : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\ComputeRadii_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> Radii;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->SRV(0, Grid);
			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));
			binder->UAV(0, Radii);
		}
	};

	struct ConservativeAndPresample : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\ConservativeRadiusAndPresample_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> Radii;
		gObj<Texture3D> HGrid;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->SRV(0, Grid);
			binder->SRV(1, Radii);
			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP));
			binder->UAV(0, HGrid);
		}
	};


	struct VPTPipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\VPT_CS.cso"));
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

			binder->SMP_Static(0, Sampler::LinearWithoutMipMaps(D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP));
			
			binder->UAV(0, Output);
			binder->UAV(1, Accumulation);
			binder->UAV(2, SqrAccumulation);
			binder->UAV(3, Complexity);
			
			binder->Space(2);
			binder->UAV(0, RngStates);
		}
	};

	gObj<VPTPipeline> pipeline;
	gObj<ComputeRadii> radii;
	gObj<ConservativeAndPresample> conservative;

public:
	~VPTTechnique() {}

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
		Load(radii);
		Load(conservative);

		if (desc->getGrids().Count > 0) {
			radii->Grid = LoadTexture3DVarMipMap(desc->getGrids().Data[0]);
		}
		radii->Radii = CreateTexture3DUAV<float>(radii->Grid->Width(), radii->Grid->Height(), radii->Grid->Depth());

		conservative->Radii = radii->Radii;
		conservative->Grid = radii->Grid;
		conservative->HGrid = CreateTexture3DUAV<float4>(radii->Grid->Width(), radii->Grid->Height(), radii->Grid->Depth());

		// Allocate Memory for scene elements
		pipeline->VolumeMaterial = CreateBufferCB<VolumeMaterialCB>();
		pipeline->HGrid = conservative->HGrid;

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
			manager->ToGPU(radii->Grid);
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

			manager->SetPipeline(radii);
			manager->Dispatch(radii->Grid->Width() * radii->Grid->Height() * radii->Grid->Depth() / 1024);
			
			manager->SetPipeline(conservative);
			manager->Dispatch(radii->Grid->Width() * radii->Grid->Height() * radii->Grid->Depth() / 1024);
		}

		manager->SetPipeline(pipeline);

		manager->Dispatch((int)ceil(CurrentRenderTarget()->Width() / 32.0), (int)ceil(CurrentRenderTarget()->Height() / 32.0));

		manager->Copy(CurrentRenderTarget(), pipeline->Output);

		pipeline->AccumulativeInfo.Pass++;
	}
};

class VisTechnique : public Technique, public IManageScene, public VISInfo {

	struct VisPipeline : public ComputePipeline {

		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\VVis_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> Deep;
		gObj<Texture2D> Output;
		gObj<Buffer> Camera;
		struct VISInfoCB {
			float Slice;
			bool ShowComplexity;
		} Info;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->Space(0);
			binder->SRV(0, Grid);
			binder->SRV(1, Deep);
			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP));
			//binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP));
			binder->UAV(0, Output);
			binder->CBV(0, Camera);
			binder->CBV(1, Info);
		}
	};

	struct ComputeRadii : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\ComputeRadii_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> Radii;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->SRV(0, Grid);
			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));
			binder->UAV(0, Radii);
		}
	};

	struct ComputeDeep : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\ComputeDeep_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> Deep;
		gObj<Texture2D> RGN_STATES;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->SRV(0, Grid);
			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));
			binder->UAV(0, Deep);

			binder->Space(2);
			binder->UAV(0, RGN_STATES);
		}
	};

	struct ConservativeAndPresample : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VolumeRendering\\ConservativeRadiusAndPresample_CS.cso"));
		}

		gObj<Texture3D> Grid;
		gObj<Texture3D> Radii;
		gObj<Texture3D> HGrid;

		virtual void Bindings(gObj<ComputeBinder> binder) {
			binder->SRV(0, Grid);
			binder->SRV(1, Radii);
			binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP));
			binder->UAV(0, HGrid);
		}
	};


	gObj<VisPipeline> pipeline;
	gObj<ComputeRadii> radii;
	gObj<ComputeDeep> deep;
	gObj<ConservativeAndPresample> conservative;

public:

	virtual void OnLoad() override {
		auto desc = scene->getScene();

		Load(pipeline); // loads the pipeline.
		Load(radii);
		Load(deep);
		Load(conservative);

		if (desc->getGrids().Count > 0) {
			deep->Grid = LoadTexture3DVarMipMap(desc->getGrids().Data[0]);
		}
		deep->Deep = CreateTexture3DUAV<float>(deep->Grid->Width(), deep->Grid->Height(), deep->Grid->Depth());
		deep->RGN_STATES = CreateTexture2DUAV<int4>(deep->Grid->Width(), deep->Grid->Height());

		radii->Radii = CreateTexture3DUAV<float>(deep->Grid->Width(), deep->Grid->Height(), deep->Grid->Depth());
		radii->Grid = deep->Grid;
		
		conservative->Radii = radii->Radii;
		conservative->Grid = radii->Grid;
		conservative->HGrid = CreateTexture3DUAV<float4>(radii->Grid->Width(), radii->Grid->Height(), radii->Grid->Depth());

		// Allocate Memory for scene elements
		pipeline->Grid = conservative->HGrid;
		pipeline->Deep = deep->Deep;
		pipeline->Camera = CreateBufferCB<float4x4>();

		pipeline->Output = CreateTexture2DUAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		Execute_OnGPU(LoadAssets);
	}

	void LoadAssets(gObj<GraphicsManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		UpdateBuffers(manager, elements);
	}

	virtual void UpdateBuffers(gObj<GraphicsManager> manager, SceneElement elements) {
		if (+(elements & SceneElement::Textures)) {
			manager->ToGPU(deep->Grid);

			manager->SetPipeline(deep);
			manager->Dispatch(deep->Grid->Width() * deep->Grid->Height() * deep->Grid->Depth() / 1024);

			manager->SetPipeline(radii);
			manager->Dispatch(radii->Grid->Width() * radii->Grid->Height() * radii->Grid->Depth() / 1024);

			manager->SetPipeline(conservative);
			manager->Dispatch(radii->Grid->Width() * radii->Grid->Height() * radii->Grid->Depth() / 1024);
		}

		if (+(elements & SceneElement::Camera))
		{
			float4x4 proj, view;
			scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
			pipeline->Camera->Write(mul(inverse(proj), inverse(view)));
			manager->ToGPU(pipeline->Camera);
		}
	}

	virtual void OnDispatch() override {
		Execute_OnGPU(UpdateAssets);
		Execute_OnGPU(DrawScene);
	}

	void UpdateAssets(gObj<GraphicsManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		UpdateBuffers(manager, elements);
	}

	void DrawScene(gObj<GraphicsManager> manager) {

		pipeline->Info.ShowComplexity = this->ShowComplexity;
		pipeline->Info.Slice = this->Slice;
		manager->SetPipeline(pipeline);

		manager->Dispatch((int)ceil(CurrentRenderTarget()->Width() / 32.0), (int)ceil(CurrentRenderTarget()->Height() / 32.0));

		manager->Copy(CurrentRenderTarget(), pipeline->Output);
	}
};
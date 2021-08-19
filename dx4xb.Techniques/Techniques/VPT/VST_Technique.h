#include "dx4xb.h" 
#include "..\..\gui_traits.h"
#include "..\ML\Pooling\MipMap3DTechnique.h"
#include "..\Tools\Parameters.h"

using namespace dx4xb;

//struct ComputeInitialGradientsPipeline : public ComputePipeline {
//	void Setup() {
//		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VPT\\ComputeInitialGradients_CS.cso"));
//	}
//
//	gObj<Texture3D> Grid;
//	gObj<Texture3D> Gradients;
//
//	virtual void Bindings(gObj<ComputeBinder> binder) {
//		binder->OnDispatch();
//		binder->SRV(0, Grid);
//		binder->UAV(0, Gradients);
//	}
//};

//struct ComputeParametersPipeline : public ComputePipeline {
//	void Setup() {
//		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VPT\\LeastSquaresApproximation_CS.cso"));
//	}
//
//	gObj<Texture3D> Grid;
//	gObj<Texture3D> Gradients;
//	gObj<Texture3D> Parameters;
//
//	int Level;
//
//	virtual void Bindings(gObj<ComputeBinder> binder) {
//		binder->OnDispatch();
//		binder->SRV(0, Grid);
//		binder->SRV(1, Gradients);
//		binder->UAV(0, Parameters);
//		binder->CBV(0, Level);
//	}
//};

struct ComputeRadiiPipeline : public ComputePipeline {
	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VPT\\ComputeRadii_CS.cso"));
	}

	gObj<Texture3D> Grid;
	gObj<Texture3D> Parameters;
	gObj<Texture3D> Radii;

	struct RadiiComputationParameters {
		float Threshold;
		int StartLevel;
	} Settings;

	virtual void Bindings(gObj<ComputeBinder> binder) {
		binder->OnDispatch();
		binder->SRV(0, Grid);
		binder->UAV(0, Parameters);
		binder->UAV(1, Radii);
		binder->CBV(0, Settings);
		binder->SMP_Static(0, Sampler::Linear(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER)); // Grid Sampler
	}
};

struct VPT_Pipeline : public ComputePipeline {

	void Setup() {
		set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\VPT\\VST_PT_CS.cso"));
	}

	gObj<Texture3D> Grid;
	gObj<Texture3D> Parameters;
	gObj<Texture3D> Radii;

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
		binder->SRV(1, Parameters);
		binder->SRV(2, Radii);
		binder->CBV(0, Camera);
		binder->CBV(1, AccumulativeInfo);
		binder->CBV(2, VolumeMaterial);
		binder->CBV(3, Lighting);

		//binder->SMP_Static(0, Sampler::PointWithoutMipMaps(D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER));

		binder->UAV(0, Output);
		binder->UAV(1, Accumulation);
		binder->UAV(2, SqrAccumulation);
		binder->UAV(3, Complexity);

		binder->Space(2);
		binder->UAV(0, RngStates);
	}
};

class VST_GB_Technique : public Technique, public IManageScene, public IGatherImageStatistics {

	gObj<MipMap3DTechnique> gridMipMapping;
	//gObj<MipMap3DTechnique> gradientMipMapping;
	gObj<Texture3D> Radii;
	gObj<Texture3D> Parameters;
	float errors[6] { 0.0, 0.005, 0.01, 0.02, 0.05, 0.1 };

	//gObj<ComputeInitialGradientsPipeline> initialGradientsPipeline;
	gObj<ComputeRadiiPipeline> computingRadii;
	//gObj<ComputeParametersPipeline> parametersPipeline;
	gObj<VPT_Pipeline> pipeline;
	
public:
	~VST_GB_Technique() {}

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

		//Load(initialGradientsPipeline); // initial gradients
		//Load(parametersPipeline);
		Load(computingRadii);
		Load(pipeline);

		if (desc->getGrids().Count > 0) {
			pipeline->Grid = LoadGrid(desc->getGrids().Data[0], true);
		
			//initialGradientsPipeline->Grid = pipeline->Grid;
			//initialGradientsPipeline->Gradients = CreateTexture3DUAV<float4>(pipeline->Grid->Width(), pipeline->Grid->Height(), pipeline->Grid->Depth(), 0);

			//computingRadii->Gradients = initialGradientsPipeline->Gradients;
			computingRadii->Grid = pipeline->Grid;

			Parameters = CreateTexture3DUAV<float4>(pipeline->Grid->Width(), pipeline->Grid->Height(), pipeline->Grid->Depth(), ARRAYSIZE(errors));
			Radii = CreateTexture3DUAV<int>(pipeline->Grid->Width(), pipeline->Grid->Height(), pipeline->Grid->Depth(), ARRAYSIZE(errors));

			pipeline->Radii = Radii;
			pipeline->Parameters = Parameters;

			Load(gridMipMapping,
				PoolingType::AveFloat,
				int3(pipeline->Grid->Width(), pipeline->Grid->Height(), pipeline->Grid->Depth())
			); // build grid mipmap

			//Load(gradientMipMapping,
			//	PoolingType::AveFloat4,
			//	int3(initialGradientsPipeline->Gradients->Width(), initialGradientsPipeline->Gradients->Height(), initialGradientsPipeline->Gradients->Depth())
			//); // build gradient mipmap

			//parametersPipeline->Parameters = pipeline->Parameters;
			//parametersPipeline->Gradients = pipeline->Gradients;
			//parametersPipeline->Grid = pipeline->Grid;
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

			/*manager->SetPipeline(initialGradientsPipeline);
			manager->Dispatch_Threads<8, 8, 8>(
				initialGradientsPipeline->Gradients->Width(),
				initialGradientsPipeline->Gradients->Height(),
				initialGradientsPipeline->Gradients->Depth()
				);*/

			gridMipMapping->Grid = pipeline->Grid;
			gridMipMapping->BuildMipMaps(manager);

			/*gradientMipMapping->Grid = initialGradientsPipeline->Gradients;
			gradientMipMapping->BuildMipMaps(manager);*/

			manager->SetPipeline(computingRadii);
			for (int i = 0; i < ARRAYSIZE(errors); i++)
			{
				computingRadii->Radii = Radii->SliceMips(i);
				computingRadii->Parameters = Parameters->SliceMips(i);
				computingRadii->Settings = { errors[i], i };
				manager->Dispatch_Threads<8, 8, 8>(
					computingRadii->Radii->Width(),
					computingRadii->Radii->Height(),
					computingRadii->Radii->Depth()
					);
			}

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

		manager->Dispatch_Threads<32, 32>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Copy(CurrentRenderTarget(), pipeline->Output);

		pipeline->AccumulativeInfo.Pass++;
	}
};

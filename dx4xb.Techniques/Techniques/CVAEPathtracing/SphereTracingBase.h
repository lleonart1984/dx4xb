#pragma once

#include "..\Pathtracing\PathtracingBase.h"

struct SphereTracingBase : public PathtracingTechniqueBase {

#pragma region Grid Construction Compute Shaders

	struct TriangleGrid : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\CVAEPathtracing\\TriangleGrid_CS.cso"));
		}

		gObj<Buffer> VertexBuffer;
		gObj<Buffer> IndexBuffer;
		gObj<Buffer> Triangles;
		gObj<Texture3D> Head;
		gObj<Buffer> NextBuffer;
		gObj<Texture1D> Malloc;

		float4x4 GridTransform;

		virtual void Bindings(gObj<ComputeBinder> binder) override {
			binder->SRV(0, VertexBuffer);
			binder->SRV(1, IndexBuffer);
				  
			binder->UAV(0, Triangles);
			binder->UAV(1, Head);
			binder->UAV(2, NextBuffer);
			binder->UAV(3, Malloc);
				  
			binder->CBV(0, GridTransform);
		}
	};

	struct DistanceFieldInitial : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\CVAEPathtracing\\DistanceFieldInitial_CS.cso"));
		}

		gObj<Buffer> VertexBuffer;
		gObj<Buffer> IndexBuffer;
		gObj<Buffer> Triangles;
		gObj<Texture3D> Head;
		gObj<Buffer> NextBuffer;

		gObj<Texture3D> DistanceField;

		float4x4 GridTransform;

		virtual void Bindings(gObj<ComputeBinder> binder) override {
			binder->SRV(0, VertexBuffer);
			binder->SRV(1, IndexBuffer);
			binder->SRV(2, Triangles);
			binder->SRV(3, Head);
			binder->SRV(4, NextBuffer);
				  
			binder->UAV(0, DistanceField);
				  
			binder->CBV(0, GridTransform);
		}
	};

	struct DistanceFieldSpread : public ComputePipeline {
		void Setup() {
			set->ComputeShader(ShaderLoader::FromFile(".\\Techniques\\CVAEPathtracing\\DistanceFieldSpread_CS.cso"));
		}

		gObj<Texture3D> GridSrc;
		gObj<Texture3D> GridDst;
		int LevelInfo;

		virtual void Bindings(gObj<ComputeBinder> binder) override {
			binder->UAV(0, GridDst);
			binder->SRV(0, GridSrc);
			binder->CBV(0, LevelInfo);
		}
	};

#pragma endregion

	struct STPathtracingPipeline : public PathtracingTechniqueBase::RTXPathtracingPipelineBase {

		struct Program :
			public RTXPathtracingPipelineBase::ProgramBase
		{

			void Bindings(gObj<RaytracingBinder> binder) {
				ProgramBase::Bindings(binder);
				binder->SRV(1, Context().Dynamic_Cast<STPathtracingPipeline>()->GridInfos);
				binder->SRV_Array(2,
					Context().Dynamic_Cast<STPathtracingPipeline>()->DistanceFields,
					Context().Dynamic_Cast<STPathtracingPipeline>()->NumberOfDFs);
			}
		};

		virtual void CreateProgram(gObj<ProgramBase>& program) {
			program = new Program();
		}

		// Grids in Space 0
		gObj<Buffer> GridInfos;
		gObj<Texture3D>* DistanceFields;
		int NumberOfDFs;
	};

	gObj<TriangleGrid> creatingGrid;
	gObj<DistanceFieldInitial> computingInitialDistances;
	gObj<DistanceFieldSpread> spreadingDistances;

#pragma region Grid related fields

	// Transform from geometry space to world space for each
	// Instanced_geometry.
	float4x4* G2WTransforms;

	// Array with a Grid for every geometry.
	gObj<Texture3D>* perGeometryDF;
	gObj<Texture3D> tempGrid;
	int GridSize = 256;

	struct GridInfo {
		// Index of the base geometry (grid).
		int GridIndex;
		// Transform from the world space to the grid.
		// Considers Instance Transform, Geometry Transform and Grid transform.
		float4x4 FromWorldToGrid;
		// Scaling factor to convert distances from grid (cell is unit) to world.
		float FromGridToWorldScaling;
	};
	// Grid information for each Instanced_Geometry.
	gObj<Buffer> GridInfos;
	GridInfo* gridInfosData;
	int globalGeometryCount;
	// Grid transform for each geometry.
	float4x4* gridTransforms;

#pragma endregion

	// Inherited via PathtracingTechniqueBase to include grid construction
	virtual void OnLoad() override {

		auto desc = scene->getScene();

		globalGeometryCount = 0;
		for (int i = 0; i < desc->Instances().Count; i++)
			globalGeometryCount += desc->Instances().Data[i].Count;

		G2WTransforms = new float4x4[globalGeometryCount];

		perGeometryDF = new gObj<Texture3D>[desc->Geometries().Count];
		for (int i = 0; i < desc->Geometries().Count; i++)
		{
			perGeometryDF[i] = CreateTexture3DUAV<float>(GridSize, GridSize, GridSize);
			perGeometryDF[i]->SetDebugName(L"Distance Field");
		}
		tempGrid = CreateTexture3DUAV<float>(GridSize, GridSize, GridSize);
		tempGrid->SetDebugName(L"Temporal Grid for DF");
		GridInfos = CreateBufferSRV<GridInfo>(globalGeometryCount);
		GridInfos->SetDebugName(L"Grid Infos");
		gridInfosData = new GridInfo[globalGeometryCount];

#pragma region Computing Per-Geometry Grid Dimensions and Transforms

		gridTransforms = new float4x4[desc->Geometries().Count];

		for (int i = 0; i < desc->Geometries().Count; i++)
		{
			auto geom = desc->Geometries().Data[i];

#pragma region Compute AABB of geometry and Transform
			float3 minim = float3(10000, 10000, 10000), maxim = float3(-10000, -10000, -10000);
			for (int j = 0; j < geom.IndexCount; j++)
			{
				float3 vPos = desc->Vertices().Data[
					desc->Indices().Data[
						geom.StartIndex + j
					] + geom.StartVertex
				].Position;
				minim = minf(minim, vPos);
				maxim = maxf(maxim, vPos);
			}
			float3 dimensions = maxim - minim;
			maxim = minim + dimensions + float3(0.01, 0.01, 0.01);
			minim = minim - float3(0.01, 0.01, 0.01);
			float maxSize = maxf(maxim.x - minim.x, maxf(maxim.y - minim.y, maxim.z - minim.z));

			gridTransforms[i] = mul(Transforms::Translate(-minim), Transforms::Scale(GridSize / maxSize));
#pragma endregion
		}

#pragma endregion

		PathtracingTechniqueBase::OnLoad();

		auto pipeline = this->pipeline.Dynamic_Cast<STPathtracingPipeline>();

		pipeline->GridInfos = GridInfos;
		pipeline->DistanceFields = perGeometryDF;
		pipeline->NumberOfDFs = desc->Geometries().Count;

		Load(creatingGrid);
		Load(computingInitialDistances);
		Load(spreadingDistances);

		// The grid for triangle hashing in space and build initial distances.
		creatingGrid->Head = CreateTexture3DUAV<int>(GridSize, GridSize, GridSize);
		creatingGrid->Head->SetDebugName(L"Head Grid");
		creatingGrid->Triangles = CreateBufferUAV<int>(10000000);
		creatingGrid->Triangles->SetDebugName(L"Triangles Buffer");
		creatingGrid->NextBuffer = CreateBufferUAV<int>(10000000);
		creatingGrid->NextBuffer->SetDebugName(L"Next Buffer");
		creatingGrid->Malloc = CreateTexture1DUAV<int>(1);
		creatingGrid->Malloc->SetDebugName(L"Malloc Buffer");

		computingInitialDistances->Head = creatingGrid->Head;
		computingInitialDistances->Triangles = creatingGrid->Triangles;
		computingInitialDistances->NextBuffer = creatingGrid->NextBuffer;
		
		Execute_OnGPU(BuildGrids);
	}

	void BuildGrids(gObj<GraphicsManager> manager) {
		auto desc = scene->getScene();

		for (int i = 0; i < desc->Geometries().Count; i++)
		{
			auto geom = desc->Geometries().Data[i];

#pragma region creating Grid for Geometry i
			// Create grid with triangles linked lists
			creatingGrid->GridTransform = gridTransforms[i];
			creatingGrid->VertexBuffer = pipeline->VertexBuffer->Slice(geom.StartVertex, geom.VertexCount);
			creatingGrid->IndexBuffer = pipeline->IndexBuffer->Slice(geom.StartIndex, geom.IndexCount);

			manager->SetPipeline(creatingGrid);
			manager->ClearUAV(creatingGrid->Head, uint4(-1));
			manager->ClearUAV(creatingGrid->Malloc, uint4(0));
			manager->Dispatch((int)ceil(geom.IndexCount / 3.0 / 1024));

			// Compute initial distances
			computingInitialDistances->VertexBuffer = pipeline->VertexBuffer->Slice(geom.StartVertex, geom.VertexCount);
			computingInitialDistances->IndexBuffer = pipeline->IndexBuffer->Slice(geom.StartIndex, geom.IndexCount);
			computingInitialDistances->DistanceField = perGeometryDF[i];
			computingInitialDistances->GridTransform = gridTransforms[i];
			manager->SetPipeline(computingInitialDistances);
			manager->Dispatch(GridSize * GridSize * GridSize / 1024);

			// Spread distance for each possible level
			for (int level = 0; level < ceil(log(GridSize) / log(3)); level++)
			{
				spreadingDistances->GridSrc = perGeometryDF[i];
				spreadingDistances->GridDst = tempGrid;
				spreadingDistances->LevelInfo = level;
				manager->SetPipeline(spreadingDistances);

				manager->Dispatch(GridSize * GridSize * GridSize / 1024);

				perGeometryDF[i] = spreadingDistances->GridDst;
				tempGrid = spreadingDistances->GridSrc;
			}
#pragma endregion
		}
	}

	void UpdateBuffers(gObj<GraphicsManager> manager, SceneElement elements)
	{
		PathtracingTechniqueBase::UpdateBuffers(manager, elements);

		auto desc = scene->getScene();

		if (+(elements & SceneElement::GeometryTransforms) ||
			+(elements & SceneElement::InstanceTransforms))
		{ // Update Geometry2World transforms and Grid2World transforms in GridInfos
			int transformIndex = 0;
			for (int i = 0; i < desc->Instances().Count; i++)
			{
				auto instance = desc->Instances().Data[i];
				for (int j = 0; j < instance.Count; j++) {
					auto geometry = desc->Geometries().Data[instance.GeometryIndices[j]];

					float4x4 geometryTransform = geometry.TransformIndex == -1 ?
						float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) :
						Transforms::FromAffine(desc->getTransformsBuffer().Data[geometry.TransformIndex]);

					G2WTransforms[transformIndex] = mul(geometryTransform, instance.Transform);

					pipeline->Transforms->WriteElement(transformIndex,
						(float4x3)G2WTransforms[transformIndex]
					);

					int gridIndex = instance.GeometryIndices[j];
					gridInfosData[transformIndex].GridIndex = gridIndex;
					gridInfosData[transformIndex].FromWorldToGrid
						= mul(
							inverse(G2WTransforms[transformIndex]),
							gridTransforms[gridIndex]
						);
					float scale = length(gridInfosData[transformIndex].FromWorldToGrid[0].get_xyz());
					gridInfosData[transformIndex].FromGridToWorldScaling = 1 / scale;

					transformIndex++;
				}
			}
			GridInfos->Write(gridInfosData);
			manager->ToGPU(GridInfos);
			manager->ToGPU(pipeline->Transforms);
		}
	}
};

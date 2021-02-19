#pragma once

#include "SphereTracingBase.h"

// HG factor [-1,1] linear
#define BINS_G 200

// Scattering albedo [0, 0.999] linear in log(1 / (1 - alpha))
#define BINS_SA 1000

// Radius [1, 256] linear in log(r)
#define BINS_R 9

// Theta [0,pi] linear in cos(theta)
#define BINS_THETA 45

class STFTechnique : public SphereTracingBase {

public:
	~STFTechnique() {}

	struct STFPathtracing : public STPathtracingPipeline {

		struct Program : public STPathtracingPipeline::Program {

			void Bindings(gObj<RaytracingBinder> binder) {

				STPathtracingPipeline::Program::Bindings(binder);

				binder->Space(2);
				binder->SRV(0, Context().Dynamic_Cast<STFPathtracing>()->STF);
				binder->SRV(1, Context().Dynamic_Cast<STFPathtracing>()->OneTimeSA);
				binder->SRV(2, Context().Dynamic_Cast<STFPathtracing>()->MultiTimeSA);
			}
		};

		virtual const char* ShaderFile() {
			return "./Techniques/CVAEPathtracing/STFPathtracing_RT.cso";
		}

		virtual void CreateProgram(gObj<ProgramBase>& program) {
			program = new Program();
		}

		// Tabular method buffers...
		gObj<Buffer> STF;
		gObj<Buffer> OneTimeSA;
		gObj<Buffer> MultiTimeSA;
	};

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		pipeline = new STFPathtracing();
	}

	// Inherited via Technique
	virtual void OnLoad() override {

		SphereTracingBase::OnLoad();
		
#pragma region Load Table Data from File

		auto pipeline = this->pipeline.Dynamic_Cast<STFPathtracing>();

		// Creating TABLES
		pipeline->STF = CreateBufferSRV<float>(BINS_G * BINS_SA * BINS_R * BINS_THETA);
		pipeline->OneTimeSA = CreateBufferSRV<float>(BINS_R * BINS_SA * BINS_G);
		pipeline->MultiTimeSA = CreateBufferSRV<float>(BINS_R * BINS_SA * BINS_G);

		FILE* stfFile;
		errno_t err;
		if (err = fopen_s(&stfFile, "stf2.bin", "rb"))
		{
			return;
		}

		float* tempSTable = new float[BINS_G * BINS_SA * BINS_R];
		fread((void*)tempSTable, 4, BINS_G * BINS_SA * BINS_R, stfFile);
		pipeline->OneTimeSA->Write(tempSTable);
		fread((void*)tempSTable, 4, BINS_G * BINS_SA * BINS_R, stfFile);
		pipeline->MultiTimeSA->Write(tempSTable);
		delete[] tempSTable;
		tempSTable = new float[BINS_G * BINS_SA * BINS_R * BINS_THETA];
		fread((void*)tempSTable, 4, BINS_G * BINS_SA * BINS_R * BINS_THETA, stfFile);
		pipeline->STF->Write(tempSTable);
		delete[] tempSTable;

		Execute_OnGPU(LoadTables);

#pragma endregion
	}

	void LoadTables(gObj<GraphicsManager> manager) {
		auto pipeline = this->pipeline.Dynamic_Cast<STFPathtracing>();
		manager->ToGPU(pipeline->STF);
		manager->ToGPU(pipeline->OneTimeSA);
		manager->ToGPU(pipeline->MultiTimeSA);
	}
};
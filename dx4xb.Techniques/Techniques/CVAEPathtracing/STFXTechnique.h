#pragma once

#include "SphereTracingBase.h"

// HG factor [-1,1] linear
#define BINS_G 100

// Log of the number of Scatters between 0..8
#define BINS_LOGN 100

// Radius [0, 127] linear in log(r)
#define BINS_R 8

// Theta [0,pi] linear in cos(theta) beta[-1,1] alpha[-1,1]
//40x20x10
#define BINS_THETA 40
#define BINS_BETA 20
#define BINS_ALPHA 10
#define BINS_X (BINS_THETA * BINS_BETA * BINS_ALPHA)

class STFXTechnique : public SphereTracingBase {

public:
	~STFXTechnique() {}

	struct STFXPathtracing : public STPathtracingPipeline {

		struct Program : public STPathtracingPipeline::Program {

			void Bindings(gObj<RaytracingBinder> binder) {
				
				STPathtracingPipeline::Program::Bindings(binder);

				binder->Space(2);
				binder->SRV(0, Context().Dynamic_Cast<STFXPathtracing>()->CDF_LogN);
				binder->SRV(1, Context().Dynamic_Cast<STFXPathtracing>()->CDF_XW_L);
				binder->SRV(2, Context().Dynamic_Cast<STFXPathtracing>()->CDF_XW_H);
			}
		};

		virtual const char* ShaderFile() {
			return "./Techniques/CVAEPathtracing/STFXPathtracing_RT.cso";
		}

		virtual void CreateProgram(gObj<ProgramBase>& program) {
			program = new Program();
		}

		// Tabular method buffers...
		gObj<Buffer> CDF_LogN;
		gObj<Buffer> CDF_XW_L;
		gObj<Buffer> CDF_XW_H;
	};

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		pipeline = new STFXPathtracing();
	}

	// Inherited via Technique
	virtual void OnLoad() override {

		SphereTracingBase::OnLoad();

		auto pipeline = this->pipeline.Dynamic_Cast<STFXPathtracing>();

#pragma region Load Table Data from File

		// Creating TABLES
		pipeline->CDF_LogN = CreateBufferSRV<float>(BINS_G * BINS_R * BINS_LOGN);
		pipeline->CDF_XW_L = CreateBufferSRV<float>(BINS_G * BINS_R * BINS_LOGN * BINS_X / 2); // Spliting 2.7 GB in two tables
		pipeline->CDF_XW_H = CreateBufferSRV<float>(BINS_G * BINS_R * BINS_LOGN * BINS_X / 2);

		FILE* stfFile;
		errno_t err;
		if (err = fopen_s(&stfFile, "stfx.bin", "rb"))
		{
			return;
		}

		float* tempSTable = new float[BINS_G * BINS_R * BINS_LOGN];
		fread((void*)tempSTable, 4, BINS_G * BINS_R * BINS_LOGN, stfFile);
		pipeline->CDF_LogN->Write(tempSTable);
		delete[] tempSTable;

		tempSTable = new float[BINS_G * BINS_R * BINS_LOGN * BINS_X / 2];
		fread((void*)tempSTable, 4, BINS_G * BINS_R * BINS_LOGN * BINS_X / 2, stfFile);
		pipeline->CDF_XW_L->Write(tempSTable);
		fread((void*)tempSTable, 4, BINS_G * BINS_R * BINS_LOGN * BINS_X / 2, stfFile);
		pipeline->CDF_XW_H->Write(tempSTable);
		delete[] tempSTable;

#pragma endregion

		Execute_OnGPU(LoadTables);
	}

	void LoadTables(gObj<GraphicsManager> manager) {

		auto pipeline = this->pipeline.Dynamic_Cast<STFXPathtracing>();

		manager->ToGPU(pipeline->CDF_LogN);
		manager->ToGPU(pipeline->CDF_XW_L);
		manager->ToGPU(pipeline->CDF_XW_H);
	}
};
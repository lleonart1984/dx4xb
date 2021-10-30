#pragma once

#include "SphereTracingBase.h"

class CVAEPathtracingTechnique : public SphereTracingBase {

public:
	~CVAEPathtracingTechnique() {}

	struct CVAEPathtracing : public STPathtracingPipeline {
		virtual const char* ShaderFile() {
			//return "./Techniques/CVAEPathtracing/CVAEPathtracing_RT.cso";
			return "./Techniques/CVAEPathtracing/CVAEST_RT.cso";
		}
	};

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		pipeline = new CVAEPathtracing();
	}
};
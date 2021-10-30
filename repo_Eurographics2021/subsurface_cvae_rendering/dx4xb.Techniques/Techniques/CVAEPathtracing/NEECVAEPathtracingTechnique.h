#pragma once

#include "SphereTracingBase.h"

class NEECVAEPathtracingTechnique : public SphereTracingBase {

public:
	~NEECVAEPathtracingTechnique() {}

	struct NEECVAEPathtracing : public STPathtracingPipeline {
		virtual const char* ShaderFile() {
			return "./Techniques/CVAEPathtracing/NEECVAEPathtracing_RT.cso";
		}
	};

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		pipeline = new NEECVAEPathtracing();
	}
};
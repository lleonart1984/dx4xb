#pragma once

#include "PathtracingBase.h"

using namespace dx4xb;

class NEEPathtracingTechnique : public PathtracingTechniqueBase {

public:
	~NEEPathtracingTechnique() {}

	struct RTXNEEPathtracing : public RTXPathtracingPipelineBase {
		virtual const char* ShaderFile() {
			return "./Techniques/Pathtracing/NEEPathtracing_RT.cso";
		}
	};

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		pipeline = new RTXNEEPathtracing();
	}
};
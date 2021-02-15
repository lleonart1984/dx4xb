#pragma once

#include "PathtracingBase.h"

using namespace dx4xb;

class PathtracingTechnique : public PathtracingTechniqueBase{

public:
	~PathtracingTechnique() {}

	struct RTXPathtracing : public RTXPathtracingPipelineBase {
		virtual const char* ShaderFile() {
			return "./Techniques/Pathtracing/Pathtracing_RT.cso";
		}
	};

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		pipeline = new RTXPathtracing();
	}
};
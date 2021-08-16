#pragma once
#include "dx4xb.h"
#include "PoolingPipeline.h"

using namespace dx4xb;

class MipMap3DTechnique : public Technique
{
	gObj<Pooling3DPipeline> pipeline;
	PoolingType poolingType;
	int3 capacity;
	gObj<Texture3D> temporal;
public:
	MipMap3DTechnique(PoolingType poolingType, int3 capacity) {
		this->poolingType = poolingType;
		this->capacity = capacity;
	}

	gObj<Texture3D> Grid;

	virtual void OnLoad() override {
		Load(this->pipeline, this->poolingType);

		switch (this->poolingType) {
		case PoolingType::AveFloat:
		case PoolingType::MinFloat:
		case PoolingType::MaxFloat:
			this->temporal = CreateTexture3DUAV<float>(this->capacity.x, this->capacity.y, this->capacity.z, 1);
			break;
		default:
			this->temporal = CreateTexture3DUAV<float4>(this->capacity.x, this->capacity.y, this->capacity.z, 1);
			break;
		}
	}

	virtual void OnDispatch() override {
		Execute_OnGPU(BuildMipMaps);
	}

	void BuildMipMaps(gObj<GraphicsManager> manager) {
		// compute mip maps if grid is updated
		manager->SetPipeline(pipeline);

		int w = Grid->Width();
		int h = Grid->Height();
		int d = Grid->Depth();
		pipeline->Src = Grid;
		pipeline->Dst = temporal;
		int currentMipLevel = 1;
		while (w > 1 && h > 1 && d > 1) {
			w /= 2;
			h /= 2;
			d /= 2;
			manager->Dispatch((int)ceil(w / 8.0), (int)ceil(h / 8.0), (int)ceil(d / 8.0));
			// Copy from Temporal Resources to Specific Mip.
			// This is because can not be bound at same time two mips from same resource as SRV and UAV.
			pipeline->Src = Grid->SliceMips(currentMipLevel);
			manager->Copy(pipeline->Src, 0, 0, 0, temporal, 0, 0, 0, w, h, d);
			currentMipLevel++;
		}
	}
};

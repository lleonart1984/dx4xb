#pragma once

#include "dx4xb.h"

using namespace dx4xb;

struct MyTechnique : public Technique {
	void OnLoad() {
	}

	void ClearRenderTarget(gObj<GraphicsManager> manager) {
		manager->ClearRenderTarget(CurrentRenderTarget(), float4(1, 1, 0, 1));
	}

	void OnDispatch() {
		Execute_OnGPU(ClearRenderTarget);
	}
};
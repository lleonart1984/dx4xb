#pragma once

#include "dx4xb_scene.h"

using namespace dx4xb;

struct IManageScene {
	gObj<SceneManager> scene = nullptr;
	SceneVersion sceneVersion;
	virtual void SetSceneManager(gObj<SceneManager> scene) {
		this->scene = scene;
	}
};


struct IGatherImageStatistics {
	virtual void getAccumulators(gObj<Texture2D>& sum, gObj<Texture2D>& sqrSum, int& frames) = 0;
};

struct VISInfo {
	float Slice;
	bool ShowComplexity;
};
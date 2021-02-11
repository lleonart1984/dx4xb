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
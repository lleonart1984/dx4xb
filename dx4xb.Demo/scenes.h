#pragma once

#include "dx4xb_scene.h"
#include "shlobj.h"

using namespace dx4xb;

//typedef class BunnyScene main_scene;
//typedef class LucyFarScene main_scene;
//typedef class PitagorasScene main_scene;
//typedef class LucyScene main_scene;
//typedef class BuddhaScene main_scene;
//typedef class NEEBuddhaScene main_scene;
//typedef class LucyAndDrago main_scene;
typedef class LucyAndDrago2 main_scene;

dx4xb::string desktop_directory()
{
	static char path[MAX_PATH + 1];
	SHGetSpecialFolderPathA(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE);
	return dx4xb::string(path);
}

#ifdef EUROGRAPHICS

class BunnyScene : public SceneManager {
public:
	BunnyScene() :SceneManager() {
	}
	~BunnyScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0, 1.6);
		lights[0].Direction = normalize(float3(1, 1, 1));
		lights[0].Intensity = float3(10, 10, 10);

		CA4G::string desktopPath = desktop_directory();

		CA4G::string lucyPath = desktopPath + CA4G::string("\\Models\\bunny.obj");

		auto bunnyScene = OBJLoader::Load(lucyPath);
		bunnyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(bunnyScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass bunny
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(500, 500, 500) * 0.25, // sigma
			float3(0.999, 0.99995, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class LucyAndDrago : public SceneManager {
public:
	LucyAndDrago() :SceneManager() {
	}
	~LucyAndDrago() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0.5, 1.7);
		camera.Target = float3(0, 0.4, 0);
		//camera.Position = float3(0, 0.5, 1.7);
		//camera.Target = float3(0, 0.4, 0);

		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(10, 10, 10);

		CA4G::string desktopPath = desktop_directory();
		CA4G::string lucyPath = desktopPath + CA4G::string("\\Models\\newLucy.obj");

		auto lucyScene = OBJLoader::Load(lucyPath);
		lucyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(lucyScene);

		CA4G::string dragoPath = desktopPath + CA4G::string("\\Models\\newDragon.obj");
		auto dragoScene = OBJLoader::Load(dragoPath);
		dragoScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(dragoScene);

		CA4G::string platePath = desktopPath + CA4G::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass lucy
		setGlassMaterial(1, 1, 1 / 1.5); // glass drago
		setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(150, 250, 500), // sigma
			float3(0.999, 0.999, 0.995),
			float3(0.1, 0.1, 0.1)
		};

		scene->VolumeMaterials().Data[1] = VolumeMaterial{
			float3(500, 500, 500), // sigma
			float3(0.995, 1, 0.999),
			float3(0.8, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SetTransforms(0);

		SceneManager::SetupScene();
	}

	void SetTransforms(float time) {
		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], mul(Transforms::RotateY(time), Transforms::Translate(0.4, 0.0, 0)));
		scene->Instances().Data[1].Transform =
			mul(InitialTransforms[1], mul(Transforms::RotateY(time), Transforms::Translate(-0.3, 0.0, 0)));
		OnUpdated(SceneElement::InstanceTransforms);
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

	}
};

class LucyAndDrago2 : public SceneManager {
public:
	LucyAndDrago2() :SceneManager() {
	}
	~LucyAndDrago2() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0.5, 1.7);
		camera.Target = float3(0, 0.4, 0);
		//camera.Position = float3(0, 0.5, 1.7);
		//camera.Target = float3(0, 0.4, 0);

		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(10, 10, 10);

		CA4G::string desktopPath = desktop_directory();
		CA4G::string lucyPath = desktopPath + CA4G::string("\\Models\\newLucy.obj");

		auto lucyScene = OBJLoader::Load(lucyPath);
		lucyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(lucyScene);

		CA4G::string dragoPath = desktopPath + CA4G::string("\\Models\\newDragon.obj");
		auto dragoScene = OBJLoader::Load(dragoPath);
		dragoScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(dragoScene);

		CA4G::string platePath = desktopPath + CA4G::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass lucy
		setGlassMaterial(1, 1, 1 / 1.5); // glass drago
		setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(400, 600, 800), // sigma
			float3(0.999, 0.999, 0.995),
			float3(0.6, 0.6, 0.6)
		};

		scene->VolumeMaterials().Data[1] = VolumeMaterial{
			float3(500, 500, 500), // sigma
			float3(0.995, 1, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SetTransforms(0);

		SceneManager::SetupScene();
	}

	void SetTransforms(float time) {
		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], mul(Transforms::RotateY(time), Transforms::Translate(0.4, 0.0, 0)));
		scene->Instances().Data[1].Transform =
			mul(InitialTransforms[1], mul(Transforms::RotateY(time), Transforms::Translate(-0.3, 0.0, 0)));
		OnUpdated(SceneElement::InstanceTransforms);
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//
	}
};

class PitagorasScene : public SceneManager {
public:
	PitagorasScene() :SceneManager() {
	}
	~PitagorasScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, -0.05, -1.6);
		camera.Target = float3(0, -0.05, 0);
		lights[0].Direction = normalize(float3(0, 1, 0));
		lights[0].Intensity = float3(6, 6, 6);

		CA4G::string desktopPath = desktop_directory();

		CA4G::string modelPath = desktopPath + CA4G::string("\\Models\\pitagoras\\model2.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.6); // glass
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			0.125 * float3(1000, 1000, 1000) * float3(1.2, 1.5, 1.0), // sigma
			float3(1,1,1) - float3(0.002, 0.001, 0.1) * 0.1f,
			float3(1, 1, 1) * 0.875
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class LucyScene : public SceneManager {
public:
	LucyScene() :SceneManager() {
	}
	~LucyScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(-0.0, 0.3, 0.5);
		camera.Target = float3(0.0, 0.2, 0.0);
		lights[0].Direction = normalize(float3(0, 1, -1));
		lights[0].Intensity = float3(6, 6, 6);

		CA4G::string desktopPath = desktop_directory();

		CA4G::string modelPath = desktopPath + CA4G::string("\\Models\\newLucy.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
					float3(500, 614, 768) * 0.25, // sigma for low
					//float3(500, 614, 768), // sigma for high
					float3(0.99999, 0.99995, 0.975),
					float3(0.1, 0.1, 0.1)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class BuddhaScene : public SceneManager {
public:
	BuddhaScene() :SceneManager() {
	}
	~BuddhaScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, -0.08, 1.4);
		camera.Target = float3(0.0, -0.08, 0.0);
		lights[0].Direction = normalize(float3(1, 1, 1));
		lights[0].Intensity = float3(4, 4, 4);

		CA4G::string desktopPath = desktop_directory();

		CA4G::string modelPath = desktopPath + CA4G::string("\\Models\\Jade_buddha.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
					float3(500, 500, 500), // sigma for high
					float3(1,1,1) - float3(0.002, 0.0002, 0.002) * 16, // absorption multiplier
					float3(0.1, 0.1, 0.1)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class LucyFarScene : public SceneManager {
public:
	LucyFarScene() :SceneManager() {
	}
	~LucyFarScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(-0.0, 0.0, 1.4);
		camera.Target = float3(0.0, 0.0, 0.0);
		lights[0].Direction = normalize(float3(0, 1, -1));
		lights[0].Intensity = float3(6, 6, 6);

		CA4G::string desktopPath = desktop_directory();

		CA4G::string modelPath = desktopPath + CA4G::string("\\Models\\newLucy.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(500, 500, 500) * 0.25, // sigma
			float3(0.999, 0.99995, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

#else

class ClassicCornellScene: public SceneManager{
public:
	ClassicCornellScene() :SceneManager() {
	}
	~ClassicCornellScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0, 1.6);
		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(5, 5, 5) * 2;

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\CornellBox\\BoxBox.obj");

		auto bunnyScene = OBJLoader::Load(lucyPath);
		bunnyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(bunnyScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass bunny
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(500, 500, 500) * 0.25, // sigma
			float3(0.99, 0.995, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class BunnyCornellScene : public SceneManager {
public:
	BunnyCornellScene() :SceneManager() {
	}
	~BunnyCornellScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0, 1.6);
		lights[0].Direction = normalize(float3(0.2, 0.2, 1));
		lights[0].Intensity = float3(5, 5, 5);

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\BunnyInCornell\\bunnyScene.obj");

		auto bunnyScene = OBJLoader::Load(lucyPath);
		bunnyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(bunnyScene);

		setGlassMaterial(5, 1, 1 / 1.5); // glass bunny
		//setMirrorMaterial(2, 0.3); // reflective plate

		//scene->VolumeMaterials().Data[0] = VolumeMaterial{
		//	float3(500, 500, 500) * 0.25, // sigma
		//	float3(0.99, 0.995, 0.999),
		//	float3(0.9, 0.9, 0.9)
		//};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};


class BunnyScene : public SceneManager {
public:
	BunnyScene() :SceneManager() {
	}
	~BunnyScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0, 1.6);
		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(5, 5, 5) * 2;

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\bunny.obj");

		auto bunnyScene = OBJLoader::Load(lucyPath);
		bunnyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(bunnyScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass bunny
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(500, 500, 500) * 0.25, // sigma
			float3(0.99, 0.995, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class LucyAndDrago : public SceneManager {
public:
	LucyAndDrago() :SceneManager() {
	}
	~LucyAndDrago() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0.5, 1.7);
		camera.Target = float3(0, 0.4, 0);
		//camera.Position = float3(0, 0.5, 1.7);
		//camera.Target = float3(0, 0.4, 0);

		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(10, 10, 10);

		dx4xb::string desktopPath = desktop_directory();
		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\newLucy.obj");

		auto lucyScene = OBJLoader::Load(lucyPath);
		lucyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(lucyScene);

		dx4xb::string dragoPath = desktopPath + dx4xb::string("\\Models\\newDragon.obj");
		auto dragoScene = OBJLoader::Load(dragoPath);
		dragoScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(dragoScene);

		dx4xb::string platePath = desktopPath + dx4xb::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass lucy
		setGlassMaterial(1, 1, 1 / 1.5); // glass drago
		setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(150, 250, 500), // sigma
			float3(0.999, 0.999, 0.995),
			float3(0.1, 0.1, 0.1)
		};

		scene->VolumeMaterials().Data[1] = VolumeMaterial{
			float3(500, 500, 500), // sigma
			float3(0.995, 1, 0.999),
			float3(0.8, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SetTransforms(0);

		SceneManager::SetupScene();
	}

	void SetTransforms(float time) {
		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], mul(Transforms::RotateY(time), Transforms::Translate(0.4, 0.0, 0)));
		scene->Instances().Data[1].Transform =
			mul(InitialTransforms[1], mul(Transforms::RotateY(time), Transforms::Translate(-0.3, 0.0, 0)));
		OnUpdated(SceneElement::InstanceTransforms);
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

	}
};

class LucyAndDrago2 : public SceneManager {
public:
	LucyAndDrago2() :SceneManager() {
	}
	~LucyAndDrago2() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 0.5, 1.7);
		camera.Target = float3(0, 0.4, 0);
		//camera.Position = float3(0, 0.5, 1.7);
		//camera.Target = float3(0, 0.4, 0);

		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(10, 10, 10);

		dx4xb::string desktopPath = desktop_directory();
		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\newLucy.obj");

		auto lucyScene = OBJLoader::Load(lucyPath);
		lucyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(lucyScene);

		dx4xb::string dragoPath = desktopPath + dx4xb::string("\\Models\\newDragon.obj");
		auto dragoScene = OBJLoader::Load(dragoPath);
		dragoScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(dragoScene);

		dx4xb::string platePath = desktopPath + dx4xb::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass lucy
		setGlassMaterial(1, 1, 1 / 1.5); // glass drago
		setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(400, 600, 800), // sigma
			float3(0.999, 0.999, 0.995),
			float3(0.6, 0.6, 0.6)
		};

		scene->VolumeMaterials().Data[1] = VolumeMaterial{
			float3(500, 500, 500), // sigma
			float3(0.995, 1, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SetTransforms(0);

		SceneManager::SetupScene();
	}

	void SetTransforms(float time) {
		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], mul(Transforms::RotateY(time), Transforms::Translate(0.4, 0.0, 0)));
		scene->Instances().Data[1].Transform =
			mul(InitialTransforms[1], mul(Transforms::RotateY(time), Transforms::Translate(-0.3, 0.0, 0)));
		OnUpdated(SceneElement::InstanceTransforms);
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//
	}
};

class PitagorasScene : public SceneManager {
public:
	PitagorasScene() :SceneManager() {
	}
	~PitagorasScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, -0.05, -1.6);
		camera.Target = float3(0, -0.05, 0);
		lights[0].Direction = normalize(float3(0, 1, 0));
		lights[0].Intensity = float3(6, 6, 6);

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string modelPath = desktopPath + dx4xb::string("\\Models\\pitagoras\\model2.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.6); // glass
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			0.125 * float3(1000, 1000, 1000) * float3(1.2, 1.5, 1.0), // sigma
			float3(1,1,1) - float3(0.002, 0.001, 0.1) * 0.1f,
			float3(1, 1, 1) * 0.875
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class LucyScene : public SceneManager {
public:
	LucyScene() :SceneManager() {
	}
	~LucyScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(-0.0, 0.3, 0.5);
		camera.Target = float3(0.0, 0.2, 0.0);
		lights[0].Direction = normalize(float3(0, 1, -1));
		lights[0].Intensity = float3(6, 6, 6);

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string modelPath = desktopPath + dx4xb::string("\\Models\\newLucy.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
					float3(500, 614, 768) * 0.25, // sigma for low
					//float3(500, 614, 768), // sigma for high
					float3(0.99999, 0.99995, 0.975),
					float3(0.1, 0.1, 0.1)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};

class BuddhaScene : public SceneManager {
public:
	BuddhaScene() :SceneManager() {
	}
	~BuddhaScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, -0.08, 1.4);
		camera.Target = float3(0.0, -0.08, 0.0);
		lights[0].Direction = normalize(float3(1, 1, 1));
		lights[0].Intensity = float3(4, 4, 4);

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string modelPath = desktopPath + dx4xb::string("\\Models\\Jade_buddha.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		//setGlassMaterial(0, 1, 1/1.5); // glass
		setGlassMaterial(0, 1, 1.0); // no refraction

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
					float3(500, 500, 500), // sigma for high
					float3(1,1,1) - float3(0.002, 0.0002, 0.002) * 1, // absorption multiplier
					float3(0.1, 0.1, 0.1)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};


class NEEBuddhaScene : public SceneManager {
public:
	NEEBuddhaScene() :SceneManager() {
	}
	~NEEBuddhaScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, -0.08, 1.4);
		camera.Target = float3(0.0, -0.08, 0.0);
		lights[0].Direction = normalize(float3(1, 1, 1));
		lights[0].Intensity = float3(4, 4, 4);

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string modelPath = desktopPath + dx4xb::string("\\Models\\Jade_buddha.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		//setGlassMaterial(0, 1, 1/1.5); // glass
		setGlassMaterial(0, 1, 1.0); // no refraction

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
					float3(300, 300, 300), // sigma for high
					float3(1,1,1) - float3(0.002, 0.0002, 0.002) * 1, // absorption multiplier
					float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};


class LucyFarScene : public SceneManager {
public:
	LucyFarScene() :SceneManager() {
	}
	~LucyFarScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(-0.0, 0.0, 1.4);
		camera.Target = float3(0.0, 0.0, 0.0);
		lights[0].Direction = normalize(float3(0, 1, -1));
		lights[0].Intensity = float3(6, 6, 6);

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string modelPath = desktopPath + dx4xb::string("\\Models\\newLucy.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		setGlassMaterial(0, 1, 1 / 1.5); // glass

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(500, 500, 500) * 0.25, // sigma
			float3(0.999, 0.99995, 0.999),
			float3(0.9, 0.9, 0.9)
		};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};


class Sponza : public SceneManager {
public:
	Sponza() :SceneManager() {
	}
	~Sponza() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0.3f, 0.05f, -0.028);
		camera.Target = float3(0, 0.07f, 0);

		lights[0].Direction = normalize(float3(0, 1, 0));
		lights[0].Intensity = float3(6, 6, 6)*10;

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string modelPath = desktopPath + dx4xb::string("\\Models\\sponza\\SponzaMoreMeshes.obj");

		auto modelScene = OBJLoader::Load(modelPath);
		modelScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum //|
			//SceneNormalization::MinX |
			//SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			//SceneNormalization::Center
		);
		scene->appendScene(modelScene);

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
		OnUpdated(SceneElement::InstanceTransforms);
	}
};



#endif
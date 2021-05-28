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

		camera.Position = float3(0, 1.0, 1.6);
		camera.Target = float3(0, 0.4, 0);
		lights[0].Direction = normalize(float3(1, 0.5, 0));
		lights[0].Intensity = float3(1,1,1) * 3.14159;

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\bunny.obj");

		auto bunnyScene = OBJLoader::Load(lucyPath);
		bunnyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(bunnyScene);
		dx4xb::string platePath = desktopPath + dx4xb::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		setGlassMaterial(0, 1, 1 / 1.2); // glass bunny
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(400, 600, 800), // sigma
			float3(0.9995, 0.9995, 0.995),
			float3(0.7, 0.7, 0.68)
		};
		//scene->VolumeMaterials().Data[0] = VolumeMaterial{
		//	float3(400, 600, 800) * 0.4, // sigma
		//	float3(0.999, 0.999, 0.995),
		//	float3(0.6, 0.6, 0.6)
		//};

		setMirrorMaterial(2, 0.1); // reflective plate

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	void SetWax(float alpha) {
		setGlassMaterial(0, alpha, 1 / 1.3); // wax interface
		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(400, 600, 800) * 0.5, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9995, 0.9995, 0.995), alpha),
			lerp(m.G, float3(0.6, 0.6, 0.6), alpha)
		};
	}

	void SetEmerald(float alpha) {
		setGlassMaterial(0, alpha, 1 / 2.1); // cloud interface (no refraction)
		setMirrorMaterial(0, alpha*0.1); // cloud interface (no refraction)

		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(500, 500, 500)*0.3, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.99, 0.9999, 0.99), alpha),
			lerp(m.G, float3(0.87, 0.87, 0.87), alpha)
		};
	}

	void SetMarble(float alpha) {
		setGlassMaterial(0, alpha, 1 / 1.5); // marble interface
		setMirrorMaterial(0, alpha*0.1); // marble interface
		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(600, 600, 600) * 0.5, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9999, 0.995, 0.995), alpha),
			lerp(m.G, float3(0.1, 0.1, 0.1), alpha)
		};
	}

	void SetMilk(float alpha) {
		setGlassMaterial(0, alpha, 1 / 1.06); // marble interface
		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(91, 107, 125) * 2, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9999, 0.9999, 0.9999), alpha),
			lerp(m.G, float3(0.8, 0.8, 0.8), alpha)
		};
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {

		float materialTimeline = time * 4;

		float materialAlpha = fmod(materialTimeline, 1);
		//materialAlpha = pow(materialAlpha, 0.125);

		switch (((int)materialTimeline) % 4) {
		case 0: // Milk
			SetWax(1);
			SetMilk(materialAlpha);
			break;
		case 1:
			SetMilk(1);
			SetMarble(materialAlpha);
			break;
		case 2:
			SetMarble(1);
			SetEmerald(materialAlpha);
			break;
		case 3:
			SetEmerald(1);
			SetWax(materialAlpha);
			break;
		}

		/*switch ((int)(time * 4)) {
		case 0:
			lights[0].Direction = normalize(float3(0, 0.5, 1));
			break;
		case 1:
		{
			float alpha = (time - 0.25) / 0.25;
			lights[0].Direction = normalize(float3(sinf(alpha * 3.14159), 0.5, cosf(alpha * 3.14159)));
			break;
		}
		case 2:
			lights[0].Direction = normalize(float3(0, 0.5, -1));
			break;
		default:
		{
			float alpha = (time - 0.75) / 0.25;
			lights[0].Direction = normalize(float3(sinf(alpha * 3.14159 + 3.141596), 0.5, cosf(alpha * 3.14159 + 3.141596)));
			break;
		}
		}*/
		//scene->Instances().Data[0].Transform =
		//	mul(InitialTransforms[0], Transforms::RotateY(time * 3.141596 * 2*2));

		//float x = cosf(time * 3.141596 * 8)* 0.5 + 0.5; // 0...1
		//float y = 1 / (1 + powf(x / (1 - x), -4));

		//float diffMagnitude = y;
		//scene->Materials().Data[0].Roulette = float4(diffMagnitude, 0, 0, 1 - diffMagnitude);
		OnUpdated(SceneElement::Materials);

		//OnUpdated(SceneElement::Lights);
		//OnUpdated(SceneElement::InstanceTransforms);
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
	}
};

class BunnySceneForPT : public SceneManager {
public:
	BunnySceneForPT() :SceneManager() {
	}
	~BunnySceneForPT() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(0, 1.0, 1.6);
		camera.Target = float3(0, 0.4, 0);
		lights[0].Direction = normalize(float3(0, 0.5, -1));
		lights[0].Intensity = float3(1, 1, 1) * 3.14159;

		dx4xb::string desktopPath = desktop_directory();

		dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\bunny.obj");

		auto bunnyScene = OBJLoader::Load(lucyPath);
		bunnyScene->Normalize(
			SceneNormalization::Scale |
			SceneNormalization::Maximum |
			//SceneNormalization::MinX |
			SceneNormalization::MinY |
			//SceneNormalization::MinZ |
			SceneNormalization::Center
		);
		scene->appendScene(bunnyScene);
		dx4xb::string platePath = desktopPath + dx4xb::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		setGlassMaterial(0, 1, 1 / 1.2); // glass bunny
		//setMirrorMaterial(2, 0.3); // reflective plate

		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			float3(400, 600, 800) * 0.4, // sigma
			float3(0.999, 0.999, 0.995),
			float3(0.6, 0.6, 0.6)
		};

		SetWax(1);

		setMirrorMaterial(2, 0.1); // reflective plate

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SceneManager::SetupScene();
	}

	void SetWax(float alpha) {
		setGlassMaterial(0, alpha, 1 / 1.3); // wax interface
		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(400, 600, 800) * 0.5, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9995, 0.9995, 0.995), alpha),
			lerp(m.G, float3(0.6, 0.6, 0.6), alpha)
		};
	}

	void SetEmerald(float alpha) {
		setGlassMaterial(0, alpha, 1 / 2.1); // cloud interface (no refraction)
		setMirrorMaterial(0, alpha * 0.1); // cloud interface (no refraction)

		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(500, 500, 500) * 0.3, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.99, 0.9999, 0.99), alpha),
			lerp(m.G, float3(0.87, 0.87, 0.87), alpha)
		};
	}

	void SetMarble(float alpha) {
		setGlassMaterial(0, alpha, 1 / 1.5); // marble interface
		setMirrorMaterial(0, alpha * 0.1); // marble interface
		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(600, 600, 600) * 0.5, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9999, 0.995, 0.995), alpha),
			lerp(m.G, float3(0.1, 0.1, 0.1), alpha)
		};
	}

	void SetMilk(float alpha) {
		setGlassMaterial(0, alpha, 1 / 1.06); // marble interface
		VolumeMaterial m = scene->VolumeMaterials().Data[0];
		scene->VolumeMaterials().Data[0] = VolumeMaterial{
			lerp(m.Extinction, float3(91, 107, 125) * 2, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9999, 0.9999, 0.9999), alpha),
			lerp(m.G, float3(0.8, 0.8, 0.8), alpha)
		};
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {

		float lightTimeline = time * 4;
		float alpha = fmod(lightTimeline, 1);

		switch ((int)lightTimeline) {
		case 0:
			lights[0].Direction = normalize(float3(0, 0.5, 1));
			break;
		case 1:
		{
			lights[0].Direction = normalize(float3(sinf(alpha * 3.14159), 0.5, cosf(alpha * 3.14159)));
			break;
		}
		case 2:
			lights[0].Direction = normalize(float3(0, 0.5, -1));
			break;
		default:
		{
			lights[0].Direction = normalize(float3(sinf(alpha * 3.14159 + 3.141596), 0.5, cosf(alpha * 3.14159 + 3.141596)));
			break;
		}
		}
		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time * 3.141596 * 2*2));

		//float x = cosf(time * 3.141596 * 8)* 0.5 + 0.5; // 0...1
		//float y = 1 / (1 + powf(x / (1 - x), -4));

		//float diffMagnitude = y;
		//scene->Materials().Data[0].Roulette = float4(diffMagnitude, 0, 0, 1 - diffMagnitude);
		//OnUpdated(SceneElement::Materials);

		OnUpdated(SceneElement::Lights);
		OnUpdated(SceneElement::InstanceTransforms);
		return;//

		scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], Transforms::RotateY(time));
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

class LucyAndDrago3 : public SceneManager {
public:
	LucyAndDrago3() :SceneManager() {
	}
	~LucyAndDrago3() {}

	float4x4* InitialTransforms;

	void SetWax(int index, float alpha) {
		setGlassMaterial(index, alpha, 1 / 1.3); // wax interface
		VolumeMaterial m = scene->VolumeMaterials().Data[index];
		scene->VolumeMaterials().Data[index] = VolumeMaterial{
			lerp(m.Extinction, float3(400, 600, 800) * 0.6, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9995, 0.9995, 0.995), alpha),
			lerp(m.G, float3(0.6, 0.6, 0.6), alpha)
		};
	}

	void SetEmerald(int index, float alpha) {
		setGlassMaterial(index, alpha, 1 / 2.1); // cloud interface (no refraction)
		setMirrorMaterial(index, alpha * 0.1); // cloud interface (no refraction)

		VolumeMaterial m = scene->VolumeMaterials().Data[index];
		scene->VolumeMaterials().Data[index] = VolumeMaterial{
			lerp(m.Extinction, float3(500, 500, 500) * 0.4, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.995, 0.99999, 0.995), alpha),
			lerp(m.G, float3(0.87, 0.87, 0.87), alpha)
		};
	}

	void SetMarble(int index, float alpha) {
		setGlassMaterial(index, alpha, 1 / 1.5); // marble interface
		setMirrorMaterial(index, alpha * 0.05); // marble interface
		VolumeMaterial m = scene->VolumeMaterials().Data[index];
		scene->VolumeMaterials().Data[index] = VolumeMaterial{
			lerp(m.Extinction, float3(700, 650, 600) * 0.6, alpha), // sigma
			lerp(m.ScatteringAlbedo, float3(0.9999, 0.999, 0.999), alpha),
			lerp(m.G, float3(0.1, 0.1, 0.1), alpha)
		};
	}

	void SetupScene() {

		camera.Position = float3(0.8, 0.8, 1.7);
		camera.Target = float3(0, 0.3, 0);
		//camera.Position = float3(0, 0.5, 1.7);
		//camera.Target = float3(0, 0.4, 0);

		lights[0].Direction = normalize(float3(-1, 1, 1));
		lights[0].Intensity = float3(1, 1, 1) * 3.14;

		dx4xb::string desktopPath = desktop_directory();

		//dx4xb::string lucyPath = desktopPath + dx4xb::string("\\Models\\newLucy.obj");
		//auto lucyScene = OBJLoader::Load(lucyPath);
		//lucyScene->Normalize(
		//	SceneNormalization::Scale |
		//	SceneNormalization::Maximum |
		//	//SceneNormalization::MinX |
		//	SceneNormalization::MinY |
		//	//SceneNormalization::MinZ |
		//	SceneNormalization::Center
		//);
		//scene->appendScene(lucyScene);

		
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
		scene->appendScene(dragoScene);
		scene->appendScene(dragoScene);
		scene->appendScene(dragoScene);

		dx4xb::string platePath = desktopPath + dx4xb::string("\\Models\\plate.obj");
		auto plateScene = OBJLoader::Load(platePath);
		scene->appendScene(plateScene);

		//dx4xb::string gridPath = desktopPath + dx4xb::string("\\clouds\\cloud-1940.xyz");
		//int gridIndex = scene->appendGrid(gridPath);
		//scene->appendMaterial(SceneMaterial());
		//int volMat = scene->appendVolumeMaterial(VolumeMaterial{
		//		float3(600, 600, 600), // sigma
		//		float3(0.9999, 0.99999, 0.99995),
		//		float3(0.9, 0.9, 0.9)
		//	});
		//scene->appendVolume(gridIndex, volMat, Transforms::Translate(0,0.5,0));


		setDiffuseMaterial(0, float3(1, 0.9, 0.5), 1); // diffuse drago

		SetWax(1, 1); // glossy lucy
		SetEmerald(2, 1);
		SetMarble(3, 1);

		//ComputeNormals();
		//ComputeTangets();

		//scene->Materials().Data[1].Emissive = float3(1, 1, 1)*6;

		setMirrorMaterial(4, 0.2); // reflective plate

		//scene->VolumeMaterials().Data[0] = VolumeMaterial{
		//	float3(400, 600, 800), // sigma
		//	float3(0.999, 0.999, 0.995),
		//	float3(0.6, 0.6, 0.6)
		//};

		//scene->VolumeMaterials().Data[1] = VolumeMaterial{
		//	float3(500, 500, 500), // sigma
		//	float3(0.995, 1, 0.999),
		//	float3(0.9, 0.9, 0.9)
		//};

		InitialTransforms = new float4x4[scene->Instances().Count];
		for (int i = 0; i < scene->Instances().Count; i++)
			InitialTransforms[i] = scene->Instances().Data[i].Transform;

		SetTransforms(0);

		SceneManager::SetupScene();
	}

	void SetTransforms(float time) {

		for (int i=0; i<4; i++)
			scene->Instances().Data[i].Transform =
				mul(InitialTransforms[i], mul(Transforms::RotateY(time + 0.9), Transforms::Translate(-1 + i*0.6, 0.0, 0)));

		/*scene->Instances().Data[0].Transform =
			mul(InitialTransforms[0], mul(Transforms::RotateY(time), Transforms::Translate(0.4, 0.0, 0)));
		scene->Instances().Data[1].Transform =
			mul(InitialTransforms[1], mul(Transforms::RotateY(time), Transforms::Translate(-0.3, 0.0, 0)));*/
		OnUpdated(SceneElement::InstanceTransforms);
	}

	virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) override {
		return;//
	}
};

class CloudScene : public SceneManager {
public:
	CloudScene() :SceneManager() {
	}
	~CloudScene() {}

	void SetupScene() {

		camera.Position = float3(0.4, 0.4, 1.1);
		//camera.Position = float3(0.4, 0.4, 0.65);
		camera.Target = float3(0.0, 0.0, 0);
		//camera.Target = float3(0.05, 0.0, 0);

		lights[0].Direction = normalize(float3(1, 1, -1));
		lights[0].Intensity = float3(1, 1, 1) * 3;

		dx4xb::string desktopPath = desktop_directory();

		//dx4xb::string platePath = desktopPath + dx4xb::string("\\Models\\plate.obj");
		//auto plateScene = OBJLoader::Load(platePath);
		//scene->appendScene(plateScene); // code no needed

		dx4xb::string gridPath = desktopPath + dx4xb::string("\\clouds\\cloud-1196.xyz");
		//dx4xb::string gridPath = desktopPath + dx4xb::string("\\clouds\\cloud-1940.xyz");
		int gridIndex = scene->appendGrid(gridPath);
		scene->appendMaterial(SceneMaterial());
		int volMat = scene->appendVolumeMaterial(VolumeMaterial{
				float3(1000, 1000, 1000)*1, // sigma
				float3(1, 1, 1),
				float3(0.875, 0.875, 0.875)
			});
		scene->appendVolume(gridIndex, volMat, Transforms::Translate(0, 0.5, 0));

		SetTransforms(0);

		SceneManager::SetupScene();
	}

	void SetTransforms(float time) {
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

		camera.Position = float3(0.3f, 0.01f, -0.02);
		camera.Target = float3(0.2f, 0.04f, 0);

		lights[0].Direction = normalize(float3(0, 1, 0));
		lights[0].Intensity = float3(6, 6, 6)*4;

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

		ComputeNormals();
		
		ComputeTangets();
		
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
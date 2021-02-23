#define IMAGE_WIDTH 1264
#define IMAGE_HEIGHT 761

//You must activate USE_NARROW_LIGHTSOURCE and deactivate USE_SKYBOX in Parameters.h

typedef class LucyAndDrago2 : public SceneManager {
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
} USED_SCENE;

#include "Techniques/Pathtracing/PathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/CVAEPathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/STFTechnique.h"
#include "Techniques/CVAEPathtracing/STFXTechnique.h"


//typedef PathtracingTechnique USED_TECHNIQUE;  // Pathtracing implementation
//typedef CVAEPathtracingTechnique USED_TECHNIQUE;  // Our CVAE
//typedef STFXTechnique USED_TECHNIQUE; // Our tabular method
typedef STFTechnique USED_TECHNIQUE; // Mueller tabular method
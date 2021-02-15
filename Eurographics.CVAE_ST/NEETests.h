#define IMAGE_WIDTH 700
#define IMAGE_HEIGHT 700

//You must activate USE_NARROW_LIGHTSOURCE and deactivate USE_SKYBOX in Parameters.h

typedef class BuddhaScene : public SceneManager {
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

		auto bunnyScene = OBJLoader::Load(modelPath);
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
			float3(500, 500, 500), // sigma
			float3(1,1,1) - float3(0.002, 0.0002, 0.002),
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
} USED_SCENE;


#include "Techniques/Pathtracing/PathtracingTechnique.h"
#include "Techniques/Pathtracing/NEEPathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/NEECVAEPathtracingTechnique.h"

//typedef PathtracingTechnique USED_TECHNIQUE;  // Normal PT
//typedef NEEPathtracingTechnique USED_TECHNIQUE;  // PT with Direct Ilumination
typedef NEECVAEPathtracingTechnique USED_TECHNIQUE;  // Our NEE CVAE



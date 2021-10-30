#define IMAGE_WIDTH 500
#define IMAGE_HEIGHT 500

//You must activate SHOW_COMPLEXITY in Parameters.h

// Vary from 2 to 0.25 to get complexities
#define SIGMA_MULTIPLIER 2

// Uncomment this to force a PT solution
//#define USE_PT


typedef class BunnyScene : public SceneManager {
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
			float3(500, 500, 500) * SIGMA_MULTIPLIER, // sigma
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
} USED_SCENE;



#ifdef USE_PT
#include "Techniques/Pathtracing/PathtracingTechnique.h"
typedef PathtracingTechnique USED_TECHNIQUE;  // PT
#else
#include "Techniques/CVAEPathtracing/CVAEPathtracingTechnique.h"
typedef CVAEPathtracingTechnique USED_TECHNIQUE;  // Our CVAE
#endif


#define IMAGE_WIDTH 500
#define IMAGE_HEIGHT 500

//You must activate USE_NARROW_LIGHTSOURCE and deactivate USE_SKYBOX in Parameters.h

#define LOW_DENSITY
#define BACK_LIGHT
#define USE_PT

typedef class LucyScene : public SceneManager {
public:
	LucyScene() :SceneManager() {
	}
	~LucyScene() {}

	float4x4* InitialTransforms;

	void SetupScene() {

		camera.Position = float3(-0.0, 0.3, 0.5);
		camera.Target = float3(0.0, 0.2, 0.0);
#ifdef BACK_LIGHT
		lights[0].Direction = normalize(float3(0, 1, -1));
#else
		lights[0].Direction = normalize(float3(0, 1, 1));
#endif
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
#ifdef LOW_DENSITY
					float3(500, 614, 768) * 0.25, // sigma for low
#else
					float3(500, 614, 768), // sigma for high
#endif
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
} USED_SCENE;

#include "Techniques/Pathtracing/PathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/CVAEPathtracingTechnique.h"

#ifdef USE_PT
typedef PathtracingTechnique USED_TECHNIQUE;  // Pathtracing implementation
#else
typedef CVAEPathtracingTechnique USED_TECHNIQUE;  // Our CVAE
#endif

#define IMAGE_WIDTH 500
#define IMAGE_HEIGHT 700

// Change among 1,2,4 or 8 to get different Teasser settings
#define SIGMA_DIVIDER 1

typedef class PitagorasScene : public SceneManager {
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
			(1.0/SIGMA_DIVIDER) * float3(1000, 1000, 1000) * float3(1.2, 1.5, 1.0), // sigma
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
} USED_SCENE;

#include "Techniques/Pathtracing/PathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/CVAEPathtracingTechnique.h"

typedef PathtracingTechnique USED_TECHNIQUE;
//typedef CVAEPathtracingTechnique USED_TECHNIQUE;
#ifndef SCENE_H
#define SCENE_H

#include "dx4xb.h"

namespace dx4xb {

	struct GeometryDescription {
		int StartVertex;
		int VertexCount;
		int StartIndex;
		int IndexCount;
		int TransformIndex;
		int MaterialIndex;

		void OffsetReferences(int vertexOffset, int indexOffset, int materialOffset, int transformOffset) {
			this->StartVertex += vertexOffset;
			if (IndexCount > 0) this->StartIndex += indexOffset;
			if (MaterialIndex >= 0) this->MaterialIndex += materialOffset;
			if (TransformIndex >= 0) this->TransformIndex += transformOffset;
		}
	};

	struct VolumeDescription {
		int GridIndex;
		int MaterialIndex;
		float4x4 Transform;
	};

	struct InstanceDescription {
		int Count;
		int* GeometryIndices;
		float4x4 Transform;

		InstanceDescription Clone() {
			InstanceDescription newone;
			newone.Transform = Transform;
			newone.Count = Count;
			newone.GeometryIndices = new int[Count];
			memcpy(newone.GeometryIndices, GeometryIndices, 4 * Count);
			return newone;
		}

		void OffsetReferences(int geometryOffset) {
			for (int i = 0; i < Count; i++)
				GeometryIndices[i] += geometryOffset;
		}
	};

	struct SceneVertex {
		float3 Position;
		float3 Normal;
		float2 TexCoord;
		float3 Tangent;
		float3 Binormal;

		static std::initializer_list<VertexElement> Layout() {
			static std::initializer_list<VertexElement> result{
				VertexElement(VertexElementType::Float, 3, VertexElementSemantic::Position),
				VertexElement(VertexElementType::Float, 3, VertexElementSemantic::Normal),
				VertexElement(VertexElementType::Float, 2, VertexElementSemantic::TexCoord),
				VertexElement(VertexElementType::Float, 3, VertexElementSemantic::Tangent),
				VertexElement(VertexElementType::Float, 3, VertexElementSemantic::Binormal)
			};

			return result;
		}
	};

	struct SceneMaterial {
		float3 Diffuse;
		float RefractionIndex;
		float3 Specular;
		float SpecularPower;
		int DiffuseMap;
		int SpecularMap;
		int BumpMap;
		int MaskMap;
		float4 Roulette;
		float3 Emissive;

		SceneMaterial() {
			Diffuse = float3(1, 1, 1);
			RefractionIndex = 1;
			Specular = float3(0, 0, 0);
			SpecularPower = 1;
			DiffuseMap = -1;
			SpecularMap = -1;
			BumpMap = -1;
			MaskMap = -1;
			Emissive = float3(0, 0, 0);
			Roulette = float4(1, 0, 0, 0);
		}

		void OffsetReferences(int offset) {
			DiffuseMap = DiffuseMap == -1 ? -1 : DiffuseMap + offset;
			SpecularMap = SpecularMap == -1 ? -1 : SpecularMap + offset;
			BumpMap = BumpMap == -1 ? -1 : BumpMap + offset;
			MaskMap = MaskMap == -1 ? -1 : MaskMap + offset;
		}
	};

	struct VolumeMaterial
	{
		// Gets or sets the density of the medium.
		float3 Extinction;
		// Gets or sets the ratio of scattering events. 1- full scattering, 0- full absorption.
		float3 ScatteringAlbedo;
		// Gets or sets the Anisotropy factor for a HG phase function approximation of the scattering phase function in this medium.
		float3 G;
	};

	template<typename T>
	struct SceneData {
		T* Data;
		int Count;

		T& item(int index) {
			return Data[index];
		}
	};

	enum class SceneNormalization : int {
		None = 0,
		Center = 1,
		MinX = 2,
		MinY = 4,
		MinZ = 8,
		Scale = 16,
		ScaleX = 32,
		ScaleY = 64,
		ScaleZ = 128,
		Maximum = ScaleX | ScaleY | ScaleZ
	};

	static SceneNormalization operator&(const SceneNormalization& a, const SceneNormalization& b) {
		return (SceneNormalization)(((int)a) & ((int)b));
	}

	static SceneNormalization operator|(const SceneNormalization& a, const SceneNormalization& b) {
		return (SceneNormalization)(((int)a) | ((int)b));
	}

	static bool operator +(const SceneNormalization& a) {
		return a != SceneNormalization::None;
	}

	class SceneBuilder;

	class IScene {
		friend SceneBuilder;
	protected:
		list<SceneVertex> vertices = {};
		list<int> indices = {};
		list<SceneMaterial> materials = {};
		list<VolumeMaterial> volumeMaterials = { };
		list<float4x3> transforms = {};
		list<string> textures = {};
		list<string> grids = {};

		list<GeometryDescription> geometries = {};
		list<InstanceDescription> instances = {};
		list<VolumeDescription> volumes = {};

		IScene() {}
	public:
		virtual ~IScene() {}

		bool computeAABB(float3& minimum, float3& maximum) const {
			minimum = float3(10000000000);
			maximum = float3(-10000000000);

			auto UpdateMinMax = [&minimum, &maximum](SceneVertex v, const float4x3& localTransform, const float4x4& globalTransform) {

				float4 p = float4(v.Position.x, v.Position.y, v.Position.z, 1);
				float3 t = mul(p, localTransform);
				t = ((float4)mul(float4(t.x, t.y, t.z, 1), globalTransform)).get_xyz();

				minimum = minf(minimum, t);
				maximum = maxf(maximum, t);
			};

			for (int i = 0; i < instances.size(); i++)
			{
				InstanceDescription& instance = instances[i];
				for (int j = 0; j < instance.Count; j++)
				{
					GeometryDescription& geometry = geometries[instance.GeometryIndices[j]];

					float4x3 transform = geometry.TransformIndex == -1 ?
						float4x3(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0) :
						transforms[geometry.TransformIndex];

					if (geometry.IndexCount > 0) // indexed geometry
					{
						for (int k = 0; k < geometry.IndexCount; k++)
						{
							SceneVertex v = vertices[indices[geometry.StartIndex + k]];
							UpdateMinMax(v, transform, instance.Transform);
						}
					}
				}
			}

			return maximum.x >= minimum.x;
		}

		SceneData<SceneVertex> Vertices() const
		{
			return SceneData<SceneVertex>{
				&vertices.first(),
					vertices.size()
			};
		}
		SceneData<int> Indices() const
		{
			return SceneData<int>{
				&indices.first(),
					indices.size()
			};
		}
		SceneData<SceneMaterial> Materials() const
		{
			return SceneData<SceneMaterial>{
				&materials.first(),
					materials.size()
			};
		}
		SceneData<VolumeMaterial> VolumeMaterials() const
		{
			return SceneData<VolumeMaterial>{
				&volumeMaterials.first(),
					volumeMaterials.size()
			};
		}
		SceneData<float4x3> getTransformsBuffer() const
		{
			return SceneData<float4x3>{
				&transforms.first(),
					transforms.size()
			};
		}
		SceneData<string> getTextures() const
		{
			return SceneData<string>{
				&textures.first(),
				textures.size()
			};
		}
		SceneData<string> getGrids() const {
			return SceneData<string>{
				&grids.first(),
				grids.size()
			};
		}
		SceneData<GeometryDescription> Geometries() const
		{
			return SceneData<GeometryDescription>{
				&geometries.first(),
					geometries.size()
			};
		}
		SceneData<InstanceDescription> Instances() const
		{
			return SceneData<InstanceDescription>{
				&instances.first(),
					instances.size()
			};
		}
		SceneData<VolumeDescription> Volumes() {
			return SceneData<VolumeDescription>{
				&volumes.first(),
				volumes.size()
			};
		}
	};

	class SceneBuilder : public IScene {
	public:
		SceneBuilder() { }

		inline int appendTexture(string texture) {
			return textures.add(texture);
		}

		inline int appendGrid(string gridFile) {
			return grids.add(gridFile);
		}

		inline int appendMaterial(SceneMaterial material) {
			return materials.add(material);
		}

		inline int appendVolumeMaterial(VolumeMaterial material) {
			return volumeMaterials.add(material);
		}

		inline int appendTransform(float4x3 transform) {
			return transforms.add(transform);
		}

		int appendVertices(SceneVertex* vertices, int vertexCount) {
			int startVertex = this->vertices.size();
			for (int i = 0; i < vertexCount; i++)
				this->vertices.add(vertices[i]);
			return startVertex;
		}

		int appendIndices(int* indices, int indexCount) {
			int startIndex = this->indices.size();
			for (int i = 0; i < indexCount; i++)
				this->indices.add(indices[i]);
			return startIndex;
		}

		int appendGeometry(int vertexBufferOffset, int indexBufferOffset,
			int startVertex, int vertexCount,
			int startIndex, int indexCount = 0,
			int materialIndex = -1, int transformIndex = -1) {
			GeometryDescription geom;
			geom.StartVertex = startVertex + vertexBufferOffset;
			geom.VertexCount = vertexCount;
			geom.StartIndex = startIndex + indexBufferOffset;
			geom.IndexCount = indexCount;
			geom.MaterialIndex = materialIndex;
			geom.TransformIndex = transformIndex;
			return geometries.add(geom);
		}

		int appendInstance(int* geometries, int count, float4x4 transform = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)) {
			InstanceDescription instance;
			instance.Count = count;
			instance.GeometryIndices = new int[count];
			for (int i = 0; i < count; i++)
				instance.GeometryIndices[i] = geometries[i];
			instance.Transform = transform;
			return instances.add(instance);
		}

		int appendVolume(int volumeGrid, int materialIndex, float4x4 transform = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)) {
			VolumeDescription volume;
			volume.GridIndex = volumeGrid;
			volume.MaterialIndex = materialIndex;
			volume.Transform = transform;
			return volumes.add(volume);
		}

		void appendScene(gObj<IScene> other) {
			int materialOffset = this->materials.size();
			int textureOffset = this->textures.size();
			int transformOffset = this->transforms.size();
			int geometryOffset = this->geometries.size();

			for (int i = 0; i < other->textures.size(); i++)
				this->textures.add(other->textures[i]);

			for (int i = 0; i < other->materials.size(); i++)
			{
				SceneMaterial material = other->materials[i];
				material.OffsetReferences(textureOffset);
				this->materials.add(material);
				this->volumeMaterials.add(other->volumeMaterials[i]);
			}

			for (int i = 0; i < other->transforms.size(); i++)
				this->transforms.add(other->transforms[i]);

			int vertexOffset = this->appendVertices(&other->vertices.first(), other->vertices.size());
			int indexOffset = this->appendIndices(&other->indices.first(), other->indices.size());
			for (int i = 0; i < other->geometries.size(); i++)
			{
				GeometryDescription geom = other->geometries[i];
				geom.OffsetReferences(vertexOffset, indexOffset, materialOffset, transformOffset);
				this->geometries.add(geom);
			}

			for (int i = 0; i < other->instances.size(); i++)
			{
				InstanceDescription instance = other->instances[i].Clone();
				instance.OffsetReferences(geometryOffset);
				this->instances.add(instance);
			}
		}

		// Applies a specific transform to every instance in this scene
		void applyTransform(float4x4 globalTransform) {
			for (int i = 0; i < instances.size(); i++)
				instances[i].Transform = mul(instances[i].Transform, globalTransform);
		}

		void Normalize(SceneNormalization normalization = SceneNormalization::Center | SceneNormalization::Scale) {
			float3 minimum, maximum;
			if (!computeAABB(minimum, maximum))
				return;

			float3 scaling = float3(1, 1, 1);
			if (+(normalization & SceneNormalization::Scale)) {
				float max = 0;
				if (+(normalization & SceneNormalization::ScaleX))
					max = maxf(max, maximum.x - minimum.x);
				if (+(normalization & SceneNormalization::ScaleY))
					max = maxf(max, maximum.y - minimum.y);
				if (+(normalization & SceneNormalization::ScaleZ))
					max = maxf(max, maximum.z - minimum.z);
				if (max == 0) // Stretch
					scaling = float3(
						1.0f / maxf(0.0000001f, maximum.x - minimum.x),
						1.0f / maxf(0.0000001f, maximum.y - minimum.y),
						1.0f / maxf(0.0000001f, maximum.z - minimum.z)
					);
				else
					scaling = float3(1.0f) / max;
			}
			minimum = minimum * scaling;
			maximum = maximum * scaling;

			float3 translate = float3(0, 0, 0);
			if (+(normalization & SceneNormalization::Center))
			{
				if (+(normalization & SceneNormalization::MinX))
					translate.x = -minimum.x;
				else
					translate.x = -0.5f * (maximum.x + minimum.x);

				if (+(normalization & SceneNormalization::MinY))
					translate.y = -minimum.y;
				else
					translate.y = -0.5f * (maximum.y + minimum.y);

				if (+(normalization & SceneNormalization::MinZ))
					translate.z = -minimum.z;
				else
					translate.z = -0.5f * (maximum.z + minimum.z);
			}

			float4x4 transform = float4x4(
				scaling.x, 0, 0, 0,
				0, scaling.y, 0, 0,
				0, 0, scaling.z, 0,
				translate.x, translate.y, translate.z, 1);

			this->applyTransform(transform);
		}
	};

	enum class OBJImportMode {
		// All groups of geometries are treated as a single instance
		SingleInstance,
		// Every group is a different instance.
		MultipleInstances
	};

	class OBJLoader {
	public:
		static gObj<SceneBuilder> Load(string filePath, OBJImportMode mode = OBJImportMode::SingleInstance);
	};

	class Camera
	{
	public:
		// Gets or sets the position of the observer
		float3 Position = float3(0, 0, 2);
		// Gets or sets the target position of the observer
		// Can be used normalize(Target-Position) to refer to look direction
		float3 Target = float3(0, 0, 0);
		// Gets or sets a vector used as constraint of camera normal.
		float3 Up = float3(0, 1, 0);
		// Gets or sets the camera field of view in radians (vertical angle).
		float FoV = PI / 4;
		// Gets or sets the distance to the nearest visible plane for this camera.
		float NearPlane = 0.001f;
		// Gets or sets the distance to the farthest visible plane for this camera.
		float FarPlane = 100.0f;

		// Rotates this camera around Position
		void Rotate(float hor, float vert) {
			float3 dir = normalize(Target - Position);
			float3x3 rot = (float3x3)Transforms::Rotate(hor, Up);
			dir = mul(dir, rot);
			float3 R = cross(dir, Up);
			if (length(R) > 0)
			{
				rot = (float3x3)Transforms::Rotate(vert, R);
				dir = mul(dir, rot);
				Target = Position + dir;
			}
		}

		// Rotates this camera around Target
		void RotateAround(float hor, float vert) {
			float3 dir = (Position - Target);
			float3x3 rot = (float3x3)Transforms::Rotate(hor, float3(0, 1, 0));
			dir = mul(dir, rot);
			Position = Target + dir;
		}

		// Move this camera in front direction
		void MoveForward(float speed = 0.01) {
			float3 dir = normalize(Target - Position);
			Target = Target + dir * speed;
			Position = Position + dir * speed;
		}

		// Move this camera in backward direction
		void MoveBackward(float speed = 0.01) {
			float3 dir = normalize(Target - Position);
			Target = Target - dir * speed;
			Position = Position - dir * speed;
		}

		// Move this camera in right direction
		void MoveRight(float speed = 0.01) {
			float3 dir = Target - Position;
			float3 R = normalize(cross(dir, Up));

			Target = Target + R * speed;
			Position = Position + R * speed;
		}

		// Move this camera in left direction
		void MoveLeft(float speed = 0.01) {
			float3 dir = Target - Position;
			float3 R = normalize(cross(dir, Up));

			Target = Target - R * speed;
			Position = Position - R * speed;
		}

		// Gets the matrices for view and projection transforms
		void GetMatrices(int screenWidth, int screenHeight, float4x4& view, float4x4& projection) const
		{
			view = Transforms::LookAtRH(Position, Target, Up);
			projection = Transforms::PerspectiveFovRH(FoV, screenHeight / (float)screenWidth, NearPlane, FarPlane);
		}

		// Gets the matrices for view and projection transforms
		void GetMatricesLH(int screenWidth, int screenHeight, float4x4& view, float4x4& projection) const
		{
			view = Transforms::LookAtLH(Position, Target, Up);
			projection = Transforms::PerspectiveFovLH(FoV, screenHeight / (float)screenWidth, NearPlane, FarPlane);
		}
	};

	// Represents a light source definition
	// Point sources (Position: position of the light source, Direction = 0, SpotAngle = 0)
	// Directional sources (Position = 0, Direction: direction of the light source, SpotAngle = 0)
	// Spot lights (Position: position of the light source, Direction: direction of the spot light, SpotAngle: fov)
	class LightSource
	{
	public:
		float3 Position;
		float3 Direction;
		float SpotAngle;
		// Gets the intensity of the light
		float3 Intensity;
	};

	enum class SceneElement {
		None = 0,
		Camera = 1,
		Lights = 2,
		Vertices = 4,
		Indices = 8,
		Geometries = 16,
		Instances = 32,
		GeometryTransforms = 64,
		InstanceTransforms = 128,
		Materials = 256,
		Textures = 512,
		All = Camera | Lights | Vertices | Indices | Geometries | Instances | GeometryTransforms | InstanceTransforms | Materials | Textures
	};

	static SceneElement operator&(const SceneElement& a, const SceneElement& b) {
		return (SceneElement)(((int)a) & ((int)b));
	}

	static SceneElement operator|(const SceneElement& a, const SceneElement& b) {
		return (SceneElement)(((int)a) | ((int)b));
	}

	static bool operator +(const SceneElement& a) {
		return a != SceneElement::None;
	}

	class SceneManager;
	class SceneInfo;

	struct SceneVersion {
		friend SceneInfo;
		friend SceneManager;
	private:
		long versions[10];

		void Upgrade(SceneElement elements) {
			for (int i = 0; i < 10; i++)
				if (+(elements & (SceneElement)(1 << i)))
					versions[i]++;
		}
	public:
		SceneVersion() {
			ZeroMemory(versions, 10 * sizeof(long));
		}
	};

	class SceneInfo {
	protected:
		Camera camera;
		list<LightSource> lights;
		gObj<SceneBuilder> scene;
		SceneVersion currentVersion;

		void OnUpdated(SceneElement elements) {
			currentVersion.Upgrade(elements);
		}

		SceneInfo() {
			scene = new SceneBuilder();
			camera = Camera();
			currentVersion.Upgrade(SceneElement::Camera);
			lights.add(LightSource());
			currentVersion.Upgrade(SceneElement::Lights);
		}
	public:
		// Gets the updated scene elements respect to a specific scene version.
		// This method will update the versions of filtered elements in the parameter.
		SceneElement Updated(SceneVersion& version, SceneElement filter = SceneElement::All) {
			SceneElement update = SceneElement::None;
			for (int i = 0; i < 10; i++)
				if ((1 << i) & ((int)filter))
					if (currentVersion.versions[i] != version.versions[i])
					{
						version.versions[i] = currentVersion.versions[i];
						update = update | ((SceneElement)(1 << i));
					}
			return update;
		}

		inline const Camera& getCamera() {
			return camera;
		}

		inline const LightSource& getMainLight() {
			return lights[0];
		}

		inline int getLightCount() {
			return lights.size();
		}

		inline const LightSource& getLight(int index) {
			return lights[index];
		}

		inline gObj<IScene> getScene() {
			return scene.Dynamic_Cast<IScene>();
		}

		virtual ~SceneInfo() {
		}
	};

	class SceneManager : public SceneInfo {

	protected:

		SceneManager() : SceneInfo() {
		}

		void setGlassMaterial(int index, float alpha, float refractionIndex) {
			SceneMaterial& material = scene->Materials().Data[index];

			material.RefractionIndex = refractionIndex;
			material.Specular = lerp(material.Specular, float3(1, 1, 1), alpha);
			material.Roulette = lerp(material.Roulette, float4(0, 0, 0, 1), alpha);
		}

		void setMirrorMaterial(int index, float alpha) {
			SceneMaterial& material = scene->Materials().Data[index];

			material.Specular = lerp(material.Specular, float3(1, 1, 1), alpha);
			material.Roulette = lerp(material.Roulette, float4(0, 0, 1, 0), alpha);
		}

		void setDiffuseMaterial(int index, float3 diffuse, float alpha) {
			SceneMaterial& material = scene->Materials().Data[index];

			material.Diffuse = lerp(material.Diffuse, diffuse, alpha);
			material.Roulette = lerp(material.Roulette, float4(1, 0, 0, 0), alpha);
		}

		void setGlossyMaterial(int index, float3 specular, float power, float alpha) {
			SceneMaterial& material = scene->Materials().Data[index];

			material.Specular = lerp(material.Specular, specular, alpha);
			material.SpecularPower = lerp(material.SpecularPower, power, alpha);
			material.Roulette = lerp(material.Roulette, float4(0, 1, 0, 0), alpha);
		}

	public:
		virtual ~SceneManager() {}

		/// <summary>
		/// Uses position information to average per-vertex normals
		/// </summary>
		void ComputeNormals(bool useCCW = true, bool weightedNormals = false);

		/// <summary>
		/// Uses position and texture coordinates information to compute tangent vectors.
		/// </summary>
		void ComputeTangets();

		virtual void SetupScene() {
			if (this->scene->Vertices().Count > 0)
				currentVersion.Upgrade(SceneElement::Vertices);
			if (this->scene->Indices().Count > 0)
				currentVersion.Upgrade(SceneElement::Indices);
			if (this->scene->Materials().Count > 0)
				currentVersion.Upgrade(SceneElement::Materials);
			if (this->scene->getTransformsBuffer().Count > 0)
				currentVersion.Upgrade(SceneElement::GeometryTransforms);
			if (this->scene->getTextures().Count > 0)
				currentVersion.Upgrade(SceneElement::Textures);
			if (this->scene->Geometries().Count > 0)
				currentVersion.Upgrade(SceneElement::Geometries);
			if (this->scene->Instances().Count > 0)
			{
				currentVersion.Upgrade(SceneElement::Instances);
				currentVersion.Upgrade(SceneElement::InstanceTransforms);
			}
		}

		virtual void Animate(float time, int frame, SceneElement freeze = SceneElement::None) { }

		void setCamera(const Camera& camera) {
			this->camera = camera;
			OnUpdated(SceneElement::Camera);
		}

		void setMainLightSource(const LightSource& light) {
			lights[0] = light;
			OnUpdated(SceneElement::Lights);
		}

		void MakeDirty(SceneElement element) {
			OnUpdated(element);
		}
	};


}

#endif


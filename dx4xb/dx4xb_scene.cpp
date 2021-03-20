#include "dx4xb_scene.h"

namespace dx4xb {

#pragma region Tokenizer

	class Tokenizer
	{
		char* buffer;
		size_t count;
		size_t pos = 0;
	public:
		Tokenizer(FILE* stream) {
			fseek(stream, 0, SEEK_END);
			fpos_t count;
			fgetpos(stream, &count);
			fseek(stream, 0, SEEK_SET);
			buffer = new char[count];
			size_t offset = 0;
			size_t read;
			do
			{
				read = fread_s(&buffer[offset], count, 1, min(count, 1024 * 1024 * 20), stream);
				count -= read;
				offset += read;
			} while (read > 0);
			this->count = offset;
		}
		~Tokenizer() {
			delete[] buffer;
		}
		inline bool isEof() {
			return pos == count;
		}

		inline bool isEol()
		{
			return isEof() || peek() == 10;
		}

		void skipCurrentLine() {
			while (!isEol()) pos++;
			if (!isEof()) pos++;
		}

		inline char peek() {
			return buffer[pos];
		}

		bool match(const char* token)
		{
			while (!isEof() && (buffer[pos] == ' ' || buffer[pos] == '\t'))
				pos++;
			size_t initialPos = pos;
			size_t p = 0;
			while (!isEof() && token[p] == buffer[pos]) {
				p++; pos++;
			}
			if (token[p] == '\0')
				return true;
			pos = initialPos;
			return false;
		}
		bool matchDigit(int& d) {
			char ch = peek();

			if (ch >= '0' && ch <= '9')
			{
				d = ch - '0';
				pos++;
				return true;
			}
			return false;
		}
		bool matchSymbol(char c)
		{
			if (!isEof() && buffer[pos] == c)
			{
				pos++;
				return true;
			}
			return false;
		}

		string readTextToken() {

			size_t start = pos;
			while (!isEol() && buffer[pos] != ' ' && buffer[pos] != '/' && buffer[pos] != ';' && buffer[pos] != ':' && buffer[pos] != '.' && buffer[pos] != ',' && buffer[pos] != '(' && buffer[pos] != ')')
				pos++;
			size_t end = pos - 1;
			return string((char*)(buffer + start), end - start + 1);
		}

		string readToEndOfLine()
		{
			size_t start = pos;
			while (!isEol())
				pos++;
			size_t end = pos - 1;
			if (!isEof()) pos++;
			return string((char*)(buffer + start), end - start + 1);
		}

		inline bool endsInteger(char c)
		{
			return (c < '0') || (c > '9');
		}

		void ignoreWhiteSpaces() {
			while (!isEol() && (buffer[pos] == ' ' || buffer[pos] == '\t'))
				pos++;
		}

		bool readIntegerToken(long& i) {
			i = 0;
			if (isEol())
				return false;
			size_t initialPos = pos;
			ignoreWhiteSpaces();
			int sign = 1;
			if (buffer[pos] == '-')
			{
				sign = -1;
				pos++;
			}
			ignoreWhiteSpaces();
			size_t end = pos;
			while (pos < count && !endsInteger(buffer[pos])) {
				i = i * 10 + (buffer[pos] - '0');
				pos++;
			}
			i *= sign;

			if (pos > end)
				return true;
			pos = initialPos;
			return false;
		}

		bool readFracPart(float& i)
		{
			i = 0;
			float p = 1;
			if (isEol())
				return false;
			size_t initialPos = pos;
			ignoreWhiteSpaces();
			size_t end = pos;
			while (pos < count && !endsInteger(buffer[pos])) {
				p /= 10.0f;
				i = i + (buffer[pos] - '0') * p;
				pos++;
			}
			if (pos > end)
				return true;
			pos = initialPos;
			return false;
		}


		bool readFloatToken(float& f) {
			size_t initialPos = pos;
			int sign = 1;
			ignoreWhiteSpaces();
			if (buffer[pos] == '-')
			{
				sign = -1;
				pos++;
			}
			long intPart = 0;
			float scale = 1;
			if ((pos < count && buffer[pos] == '.') || readIntegerToken(intPart))
			{
				f = intPart;
				if (pos < count && buffer[pos] == '.')
				{
					int fracPos = pos;
					float fracPart;
					pos++;
					if (!readFracPart(fracPart))
					{
						pos = initialPos;
						return false;
					}
					else
					{
						long expPart = 0;
						if (buffer[pos] == 'e') {
							pos++;
							readIntegerToken(expPart);
							scale = pow(10, expPart);
						}
					}
					f += fracPart;
				}
				f *= sign * scale;
				return true;
			}
			pos = initialPos;
			return false;
		}
	};

#pragma endregion

#pragma region Importing OBJ

	struct VertexById
	{
		int posId;
		int norId;
		int texId;

		int IndexValue = -1;
	};

	struct VertexAllocator {
		int count;
		int capacity;
		VertexById* Allocator;

		VertexAllocator(int capacity) {
			this->capacity = capacity;
			this->count = 0;
			Allocator = new VertexById[capacity];
			for (int i = 0; i < capacity; i++)
				Allocator[i].IndexValue = -1;
		}
		~VertexAllocator() {
			delete[] Allocator;
		}

		bool resolve(int posId, int norId, int texId, int& index) {
			unsigned int hash = (posId << 15) * 13 ^ (norId + 4) * (~texId);
			int tableIndex = hash % capacity;
			while (Allocator[tableIndex].IndexValue >= 0)
			{
				if (Allocator[tableIndex].posId == posId && Allocator[tableIndex].norId == norId && Allocator[tableIndex].texId == texId)
				{
					index = Allocator[tableIndex].IndexValue;
					return true;
				}
				tableIndex = (tableIndex + 1) % capacity;
			}
			// Combination not found in hash table
			Allocator[tableIndex].posId = posId;
			Allocator[tableIndex].norId = norId;
			Allocator[tableIndex].texId = texId;
			index = Allocator[tableIndex].IndexValue = count++;
			return false;
		}
	};

	struct OBJLoaderState {
		list<string> materialNames = { };
		list<string> textureNames = {};
		list<string> usedMaterials = { };

		gObj<SceneBuilder> scene = new SceneBuilder();

		int resolveTexture(string subdir, string fileName) {
			for (int i = 0; i < textureNames.size(); i++)
				if (textureNames[i] == fileName)
					return i;

			string full = subdir;
			full = full + fileName;

			textureNames.add(fileName);
			scene->appendTexture(full);

			return scene->getTextures().Count - 1;
		}

		int getMaterialIndex(string materialName) {
			for (int i = 0; i < materialNames.size(); i++)
				if (materialNames[i] == materialName)
					return i;
			materialName = "";
			return -1;
		}

		void addMaterial(string name, SceneMaterial material) {
			materialNames.add(name);
			scene->appendMaterial(material);
			scene->appendVolumeMaterial(VolumeMaterial{
				float3(0,0,0),
				float3(1,1,1),
				float3(0,0,0)
				});
		}

		void importMTLFile(string subdir, string fileName) {
			string file = subdir;
			file = file + fileName;

			string currentMaterialName = "";
			SceneMaterial currentMaterial = {};

			FILE* f;
			if (fopen_s(&f, file.c_str(), "r"))
				return;
			Tokenizer t(f);
			while (!t.isEof())
			{
				if (t.match("newmtl "))
				{
					if (currentMaterialName != "")
						addMaterial(currentMaterialName, currentMaterial);

					currentMaterialName = t.readToEndOfLine();
					currentMaterial = {};
					continue;
				}

				if (t.match("Kd "))
				{
					float r, g, b;
					t.readFloatToken(r);
					t.readFloatToken(g);
					t.readFloatToken(b);
					float norm = max(0.0001f, max(r, max(g, b)));
					currentMaterial.Roulette.x = norm;
					currentMaterial.Diffuse = float3(r / norm, g / norm, b / norm);
					float rouletteXY = currentMaterial.Roulette.x + currentMaterial.Roulette.y;
					currentMaterial.Roulette.x /= rouletteXY;
					currentMaterial.Roulette.y /= rouletteXY;
					t.skipCurrentLine();
					continue;
				}

				if (t.match("Ks "))
				{
					float r, g, b;
					t.readFloatToken(r);
					t.readFloatToken(g);
					t.readFloatToken(b);
					float norm = max(0.0001f, max(r, max(g, b)));
					currentMaterial.Roulette.y = norm;
					currentMaterial.Specular = float3(r / norm, g / norm, b / norm);
					float rouletteXY = currentMaterial.Roulette.x + currentMaterial.Roulette.y;
					currentMaterial.Roulette.x /= rouletteXY;
					currentMaterial.Roulette.y /= rouletteXY;
					t.skipCurrentLine();
					continue;
				}

				if (t.match("Tr "))
				{
					float r, g, b;
					t.readFloatToken(r);
					if (r > 0) {
						//t.readFloatToken(g);
						//t.readFloatToken(b);
						currentMaterial.Roulette.x = (1 - r) * 0.5;
						currentMaterial.Roulette.y = (1 - r) * 0.5;
						currentMaterial.Roulette.z = r;
					}
					t.skipCurrentLine();
					continue;
				}

				if (t.match("d "))
				{
					float r, g, b;
					t.readFloatToken(r);
					if (r < 1) {
						//t.readFloatToken(g);
						//t.readFloatToken(b);
						currentMaterial.Roulette.x = r * 0.5;
						currentMaterial.Roulette.y = r * 0.5;
						currentMaterial.Roulette.w = 1 - r;
					}
					t.skipCurrentLine();
					continue;
				}

				/*if (t.match("Tf "))
				{
					float r, g, b;
					t.readFloatToken(r);
					t.readFloatToken(g);
					t.readFloatToken(b);
					float fresnel = (r + g + b) / 3;
					if (fresnel > 0)
					{
						MaterialsData.last().Roulette.x = (1 - fresnel);
						MaterialsData.last().Roulette.y = (1 - fresnel);
						MaterialsData.last().Roulette.w = fresnel;
					}
					t.skipCurrentLine();
					continue;
				}*/

				if (t.match("Ni "))
				{
					float ni;
					t.readFloatToken(ni);
					currentMaterial.RefractionIndex = ni;
					t.skipCurrentLine();
					continue;
				}

				if (t.match("Ns "))
				{
					float power;
					t.readFloatToken(power);
					currentMaterial.SpecularPower = power;
					t.skipCurrentLine();
					continue;
				}

				if (t.match("map_Kd "))
				{
					string textureName = t.readToEndOfLine();
					currentMaterial.DiffuseMap = resolveTexture(subdir, textureName);
					continue;
				}

				if (t.match("map_Ks "))
				{
					string textureName = t.readToEndOfLine();
					currentMaterial.SpecularMap = resolveTexture(subdir, textureName);
					continue;
				}

				if (t.match("map_bump "))
				{
					string textureName = t.readToEndOfLine();
					currentMaterial.BumpMap = resolveTexture(subdir, textureName);
					continue;
				}

				if (t.match("map_d "))
				{
					string textureName = t.readToEndOfLine();
					currentMaterial.MaskMap = resolveTexture(subdir, textureName);
					continue;
				}

				t.skipCurrentLine();
			}

			if (currentMaterialName != "")
				addMaterial(currentMaterialName, currentMaterial);

			fclose(f);
		}

		void addLineIndex(list<int>& indices, int index, int pos, int total) {
			if (index <= 0)
				index = total + index;
			else
				index = index - 1;

			if (pos > 1) // repeat last index to convert a strip 0,1,2,3,... into 01 12 23 3...
				indices.add(indices[indices.size() - 1]);
			indices.add(index);
		}

		void addTriangleIndex(list<int>& indices, int index, int pos, int total) {
			if (index <= 0)
				index = total + index;
			else
				index = index - 1;

			if (pos > 2)
			{
				indices.add(indices[indices.size() - pos]); // vertex 0
				indices.add(indices[indices.size() - 1 - 1]); // vertex last
			}
			indices.add(index);
		}

		void ReadLineIndices(Tokenizer& t, list<int>& posIndices, list<int>& texIndices, list<int>& norIndices, int vCount, int vnCount, int vtCount)
		{
			int indexRead = 0;
			int pos = 0;
			int type = 0;

			bool p = false;
			bool n = false;
			bool te = false;
			while (!t.isEol())
			{
				long indexRead;
				if (t.readIntegerToken(indexRead))
				{
					switch (type)
					{
					case 0: addLineIndex(posIndices, indexRead, pos, vCount);
						p = true;
						break;
					case 1: addLineIndex(texIndices, indexRead, pos, vtCount);
						te = true;
						break;
					case 2: addLineIndex(norIndices, indexRead, pos, vnCount);
						n = true;
						break;
					}
				}
				else
				{
					int a = 4;
					a = a + 5;
				}
				if (t.isEol())
				{
					if (p) {
						if (!n)
							addLineIndex(norIndices, 0, pos, vnCount);
						if (!te)
							addLineIndex(texIndices, 0, pos, vtCount);
						n = false;
						te = false;
						p = false;
					}
					t.skipCurrentLine();
					return;
				}
				if (t.matchSymbol('/'))
				{
					type++;
				}
				else
					if (t.matchSymbol(' '))
					{
						if (p) {
							if (!n)
								addLineIndex(norIndices, 0, pos, vnCount);
							if (!te)
								addLineIndex(texIndices, 0, pos, vtCount);
							n = false;
							te = false;
							p = false;
						}
						pos++;
						type = 0;
					}
					else
					{
						if (p) {
							if (!n)
								addLineIndex(norIndices, 0, pos, vnCount);
							if (!te)
								addLineIndex(texIndices, 0, pos, vtCount);
							n = false;
							te = false;
							p = false;
						}
						t.skipCurrentLine();
						return;
					}
			}
			if (p) {
				if (!n)
					addLineIndex(norIndices, 0, pos, vnCount);
				if (!te)
					addLineIndex(texIndices, 0, pos, vtCount);
			}
		}

		void ReadFaceIndices(Tokenizer& t, list<int>& posIndices, list<int>& texIndices, list<int>& norIndices, int vCount, int vnCount, int vtCount)
		{
			int indexRead = 0;
			int pos = 0;
			int type = 0;

			bool p = false;
			bool n = false;
			bool te = false;
			while (!t.isEol())
			{
				long indexRead;
				if (t.readIntegerToken(indexRead))
				{
					switch (type)
					{
					case 0: addTriangleIndex(posIndices, indexRead, pos, vCount);
						p = true;
						break;
					case 1: addTriangleIndex(texIndices, indexRead, pos, vtCount);
						te = true;
						break;
					case 2: addTriangleIndex(norIndices, indexRead, pos, vnCount);
						n = true;
						break;
					}
				}
				if (t.isEol())
				{
					if (p) {
						if (!n)
							addTriangleIndex(norIndices, 0, pos, vnCount);
						if (!te)
							addTriangleIndex(texIndices, 0, pos, vtCount);
						n = false;
						te = false;
						p = false;
					}
					t.skipCurrentLine();
					return;
				}
				if (t.matchSymbol('/'))
				{
					type++;
				}
				else
					if (t.matchSymbol(' '))
					{
						if (p) {
							if (!n)
								addTriangleIndex(norIndices, 0, pos, vnCount);
							if (!te)
								addTriangleIndex(texIndices, 0, pos, vtCount);
							n = false;
							te = false;
							p = false;
						}
						pos++;
						type = 0;
					}
					else
					{
						if (p) {
							if (!n)
								addTriangleIndex(norIndices, 0, pos, vnCount);
							if (!te)
								addTriangleIndex(texIndices, 0, pos, vtCount);
							n = false;
							te = false;
							p = false;
						}
						t.skipCurrentLine();
						return;
					}
			}
			if (p) {
				if (!n)
					addTriangleIndex(norIndices, 0, pos, vnCount);
				if (!te)
					addTriangleIndex(texIndices, 0, pos, vtCount);
			}
		}

		void Load(string filePath, OBJImportMode mode)
		{
			list<float3> positions;
			list<float3> normals;
			list<float2> texcoords;

			list<int> positionIndices;
			list<int> textureIndices;
			list<int> normalIndices;

			list<int> lpositionIndices;
			list<int> ltextureIndices;
			list<int> lnormalIndices;

			int offsetPos = 0;
			int offsetNor = 0;
			int offsetCoo = 0;

			string full(filePath);

			string subDir = full.substr(0, full.find_last_of("\\") + 1);
			string name = full.substr(full.find_last_of("\\") + 1);

			FILE* stream;

			errno_t err;
			if (err = fopen_s(&stream, filePath.c_str(), "r"))
			{
				return;
			}

			list<int> groupLimits;
			list<int> materialLimits;

			Tokenizer t(stream);
			static int facecount = 0;

			while (!t.isEof())
			{
				if (t.match("v "))
				{
					float3 pos;
					t.readFloatToken(pos.x);
					t.readFloatToken(pos.y);
					t.readFloatToken(pos.z);
					positions.add(pos);
					t.skipCurrentLine();
					continue;
				}

				if (t.match("vn ")) {
					float3 nor;
					t.readFloatToken(nor.x);
					t.readFloatToken(nor.y);
					t.readFloatToken(nor.z);
					normals.add(nor);
					t.skipCurrentLine();
					continue;
				}

				if (t.match("vt ")) {
					float2 coord;
					t.readFloatToken(coord.x);
					t.readFloatToken(coord.y);
					float z;
					t.readFloatToken(z);
					texcoords.add(coord);
					t.skipCurrentLine();
					continue;
				}

				if (t.match("l "))
				{
					ReadLineIndices(t, lpositionIndices, ltextureIndices, lnormalIndices, positions.size(), normals.size(), texcoords.size());
					continue;
				}

				if (t.match("f "))
				{
					ReadFaceIndices(t, positionIndices, textureIndices, normalIndices, positions.size(), normals.size(), texcoords.size());
					continue;
				}

				if (t.match("usemtl "))
				{
					// split groups by material used.
					materialLimits.add(positionIndices.size());

					string materialName = t.readToEndOfLine();
					usedMaterials.add(materialName);
					continue;
				}

				if (t.match("g ")) {
					// split groups by g
					groupLimits.add(positionIndices.size());
				}

				if (t.match("mtllib ")) {
					string materialLibName = t.readToEndOfLine();
					importMTLFile(subDir, materialLibName);
					continue;
				}

				t.skipCurrentLine(); // any other line, Comment, white line, etc.
			}

			fclose(stream);

#pragma region Prepare vertex buffer and index buffer

			int maximVertices = positionIndices.size(); // maxim number of vertices posible. (3 per face)

			SceneVertex* vertices = new SceneVertex[maximVertices];
			int* indices = new int[positionIndices.size()];

			VertexAllocator allocator(maximVertices * 2);

			int vertexCount = 0;

			// Copy indices
			for (int i = 0; i < positionIndices.size(); i++)
			{
				int posId = positionIndices[i];
				int norId = normalIndices[i];
				int texId = textureIndices[i];

				int index;
				if (!allocator.resolve(posId, norId, texId, index))
				{
					vertexCount++;
					// Create a new vertex at index
					vertices[index].Position = positions[positionIndices[i]];
					vertices[index].Normal = normals[normalIndices[i]];
					vertices[index].Tangent = float3(1, 0, 0);
					vertices[index].Binormal = float3(0, 1, 0);
					vertices[index].TexCoord = texcoords[textureIndices[i]];
				}

				indices[i] = index;
			}
			
			int vertexOffset = scene->appendVertices(vertices, vertexCount); // bind vertices (vertexOffset should be 0).
			int indexOffset = scene->appendIndices(indices, positionIndices.size()); // bind indices (indexOffset should be 0)

#pragma endregion

#pragma region Compute geometry limits and material indices by merging both OBJ limiting, groups (g) and used materials (usemtl).

			int currentMaterialIndex = -1; // no material used
			int currentGroup = 0;
			int currentMaterial = 0;
			int currentGroupStart = 0; // position index of current group
			int nextGroupStart = 0;

			list<int> geometries;

			while (true) {
				if (currentMaterial < materialLimits.size() && (currentGroup >= groupLimits.size() || materialLimits[currentMaterial] <= groupLimits[currentGroup]))
				{
					nextGroupStart = materialLimits[currentMaterial];
					if (nextGroupStart != currentGroupStart) {
						geometries.add(scene->appendGeometry(vertexOffset, indexOffset, 0, vertexCount, currentGroupStart, nextGroupStart - currentGroupStart, currentMaterialIndex, -1));
						currentGroupStart = nextGroupStart;
					}
					currentMaterialIndex = getMaterialIndex(usedMaterials[currentMaterial]);
					currentMaterial++;
				}
				else
					if (currentGroup < groupLimits.size())
					{
						nextGroupStart = groupLimits[currentGroup];
						if (nextGroupStart != currentGroupStart) {
							geometries.add(scene->appendGeometry(vertexOffset, indexOffset, 0, vertexCount, currentGroupStart, nextGroupStart - currentGroupStart, currentMaterialIndex, -1));
							currentGroupStart = nextGroupStart;
						}
						currentGroup++;
					}
					else break; // finish merging
			}
			nextGroupStart = positionIndices.size();
			if (nextGroupStart != currentGroupStart) { // last range of indices...
				geometries.add(scene->appendGeometry(vertexOffset, indexOffset, 0, vertexCount, currentGroupStart, nextGroupStart - currentGroupStart, currentMaterialIndex, -1));
			}

#pragma endregion

#pragma region Generate Identity transforms

			for (int i = 0; i < geometries.size(); i++)
				scene->appendTransform(float4x3(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0));

#pragma endregion

#pragma region Creating Instances

			switch (mode) {
			case OBJImportMode::SingleInstance:
				scene->appendInstance(&geometries.first(), geometries.size());
				break;
			case OBJImportMode::MultipleInstances:
				for (int i = 0; i < geometries.size(); i++)
					scene->appendInstance(&geometries[i], 1);
				break;
			}

#pragma endregion
		}
	};

#pragma endregion

	gObj<SceneBuilder> OBJLoader::Load(string filePath, OBJImportMode mode)
	{
		OBJLoaderState state;
		state.Load(filePath, mode);
		return state.scene;
	}

	void SceneManager::ComputeNormals(bool useCCW, bool weightedNormals) {
		auto vertices = this->scene->Vertices().Data;
		int count = this->scene->Vertices().Count;

		auto indices = this->scene->Indices().Data;
		int indexCount = this->scene->Indices().Count;

		// Clear normals
		//for (int i = 0; i < count; i++)
		//	vertices[i].Normal = float3(0, 0, 0);

		// Accumulate per triangle normals
		for (int i = 0; i < indexCount / 3; i++)
		{
			float3 p0 = vertices[indices[i * 3 + 0]].Position;
			float3 p1 = vertices[indices[i * 3 + 1]].Position;
			float3 p2 = vertices[indices[i * 3 + 2]].Position;
			float3 n = cross(p1 - p0, p2 - p0);
			if (!weightedNormals)
				n = normalize(n);

			if (dot(vertices[indices[i * 3 + 0]].Normal, n) <= 0.9)
				vertices[indices[i * 3 + 0]].Normal = n;
			if (dot(vertices[indices[i * 3 + 1]].Normal, n) <= 0.9)
				vertices[indices[i * 3 + 1]].Normal = n;
			if (dot(vertices[indices[i * 3 + 2]].Normal, n) <= 0.9)
				vertices[indices[i * 3 + 2]].Normal = n;
		}

		// Normalize
		for (int i = 0; i < count; i++)
			vertices[i].Normal = normalize(vertices[i].Normal);
	}

	void SceneManager::ComputeTangets() {
		int vertexCount = this->scene->Vertices().Count;
		SceneVertex* VerticesData = this->scene->Vertices().Data;

		for (int i = 0; i < vertexCount; i += 3)
		{
			//Getting vertex from indices
			SceneVertex& vertex1 = VerticesData[i];
			SceneVertex& vertex2 = VerticesData[i + 1];
			SceneVertex& vertex3 = VerticesData[i + 2];

			//making 
			float3 edge_1_2 = vertex2.Position - vertex1.Position;
			float3 edge_1_3 = vertex3.Position - vertex1.Position;

			float2 coordinate_2_1 = vertex2.TexCoord - vertex1.TexCoord;

			if (length(coordinate_2_1) <= 0.000001)
				coordinate_2_1 = float2(1, 0);

			float2 coordinate_3_1 = vertex3.TexCoord - vertex1.TexCoord;
			if (length(coordinate_3_1) <= 0.000001)
				coordinate_3_1 = float2(0, 1);

			float alpha = (coordinate_2_1.x * coordinate_3_1.y - coordinate_2_1.y * coordinate_3_1.x);
			if (abs(alpha) < 0.00001)
				alpha = 0.00001 * sign(alpha);

			float3 T = (float3(
				((edge_1_2.y * coordinate_3_1.y) - (edge_1_3.y * coordinate_2_1.y)) * alpha,
				((edge_1_2.x * coordinate_3_1.y) - (edge_1_3.x * coordinate_2_1.y)) * alpha,
				((edge_1_2.z * coordinate_3_1.y) - (edge_1_3.z * coordinate_2_1.y)) * alpha
			));
			T = length(T) > 0.0001 ? T : float3(0, 1, 0);

			vertex1.Tangent = vertex1.Tangent + T;
			vertex2.Tangent = vertex2.Tangent + T;
			vertex3.Tangent = vertex3.Tangent + T;

			float3 B = (float3(
				((edge_1_2.x * coordinate_3_1.x) - (edge_1_3.x * coordinate_2_1.x)) * alpha,
				((edge_1_2.y * coordinate_3_1.x) - (edge_1_3.y * coordinate_2_1.x)) * alpha,
				((edge_1_2.z * coordinate_3_1.x) - (edge_1_3.z * coordinate_2_1.x)) * alpha
			));

			B = length(B) > 0.0001 ? B : float3(1, 0, 0);

			vertex1.Binormal = vertex1.Binormal + B;
			vertex2.Binormal = vertex2.Binormal + B;
			vertex3.Binormal = vertex3.Binormal + B;
		}

		for (int i = 0; i < vertexCount; i++)
		{
			VerticesData[i].Binormal = normalize(VerticesData[i].Binormal);
			VerticesData[i].Tangent = normalize(VerticesData[i].Tangent);
		}
	}

}
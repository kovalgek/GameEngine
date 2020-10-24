#include <string>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <unordered_map>

#include <DirectXMath.h>

struct MeshData;


#pragma once
class OBJFileLoader
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 textureCoordinate;
	};

	enum LineType
	{
		Ignore,
		VertexPosition,
		VertexTexCoord
	};

public:
	HRESULT loadFile(const wchar_t* szFileName, MeshData& meshData, bool ccw);
private:
	void processLine(std::wifstream& InFile, std::vector<DirectX::XMFLOAT3>& positions, LineType lineType);

	using VertexCache = std::unordered_multimap<uint32_t, uint32_t>;

	uint32_t AddVertex(uint32_t hash, const Vertex* pVertex, VertexCache& cache);

	std::vector<Vertex>     vertices;

};


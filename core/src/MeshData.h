#include <string>
#include <vector>
#include "MeshVertex.h"

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

#pragma once
struct MeshData
{
	MeshData() = default;
	~MeshData() = default;

	std::string Name;

	std::vector<MeshVertex> Vertices;
	std::vector<uint32> Indices32;

	std::vector<uint16>& GetIndices16()
	{
		if (mIndices16.empty())
		{
			mIndices16.resize(Indices32.size());
			for (size_t i = 0; i < Indices32.size(); ++i)
				mIndices16[i] = static_cast<uint16>(Indices32[i]);
		}

		return mIndices16;
	}

private:
	std::vector<uint16> mIndices16;
};
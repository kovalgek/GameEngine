#include "OBJFileLoader.h"
#include "MeshData.h"


using namespace DirectX;

HRESULT OBJFileLoader::loadFile(const wchar_t* szFileName, MeshData &meshData, bool ccw)
{
    static const size_t MAX_POLY = 64;


    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT3>   normals;
    std::vector<XMFLOAT2>   texCoords;
    VertexCache  vertexCache;

	std::wifstream InFile(szFileName);
	if (!InFile)
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    for (;; )
    {
        std::wstring strCommand;
        InFile >> strCommand;
        if (!InFile)
            break;

        if (*strCommand.c_str() == L'#' ||
            0 == wcscmp(strCommand.c_str(), L"o") || // Object name ignored
            0 == wcscmp(strCommand.c_str(), L"g") || // Group name ignored
            0 == wcscmp(strCommand.c_str(), L"s")    // Smoothing group ignored
            )
        {
            processLine(InFile, positions, Ignore);
        }
        else if (0 == wcscmp(strCommand.c_str(), L"v"))
        {
            //processLine(InFile, positions, VertexPosition);

            float x, y, z;
            InFile >> x >> y >> z;
            positions.emplace_back(XMFLOAT3(x, y, z));
        }
        else if (0 == wcscmp(strCommand.c_str(), L"vt"))
        {

            // Vertex TexCoord
            float u, v;
            InFile >> u >> v;
            texCoords.emplace_back(XMFLOAT2(u, v));

            //hasTexcoords = true;
        }
        else if (0 == wcscmp(strCommand.c_str(), L"vn"))
        {
            // Vertex Normal
            float x, y, z;
            InFile >> x >> y >> z;
            normals.emplace_back(XMFLOAT3(x, y, z));

            //hasNormals = true;
        }
        else if (0 == wcscmp(strCommand.c_str(), L"f"))
        {
            // Face
            INT iPosition, iTexCoord, iNormal;
            Vertex vertex;

            uint32_t faceIndex[MAX_POLY];
            size_t iFace = 0;

            for (;;)
            {
                if (iFace >= MAX_POLY)
                {
                    // Too many polygon verts for the reader
                    return E_FAIL;
                }

                memset(&vertex, 0, sizeof(vertex));

                InFile >> iPosition;

                uint32_t vertexIndex = 0;
                if (!iPosition)
                {
                    // 0 is not allowed for index
                    return E_UNEXPECTED;
                }
                else if (iPosition < 0)
                {
                    // Negative values are relative indices
                    vertexIndex = uint32_t(ptrdiff_t(positions.size()) + iPosition);
                }
                else
                {
                    // OBJ format uses 1-based arrays
                    vertexIndex = uint32_t(iPosition - 1);
                }

                if (vertexIndex >= positions.size())
                    return E_FAIL;

                vertex.position = positions[vertexIndex];

                if ('/' == InFile.peek())
                {
                    InFile.ignore();

                    if ('/' != InFile.peek())
                    {
                        // Optional texture coordinate
                        InFile >> iTexCoord;

                        uint32_t coordIndex = 0;
                        if (!iTexCoord)
                        {
                            // 0 is not allowed for index
                            return E_UNEXPECTED;
                        }
                        else if (iTexCoord < 0)
                        {
                            // Negative values are relative indices
                            coordIndex = uint32_t(ptrdiff_t(texCoords.size()) + iTexCoord);
                        }
                        else
                        {
                            // OBJ format uses 1-based arrays
                            coordIndex = uint32_t(iTexCoord - 1);
                        }

                        if (coordIndex >= texCoords.size())
                            return E_FAIL;

                        vertex.textureCoordinate = texCoords[coordIndex];
                    }

                    if ('/' == InFile.peek())
                    {
                        InFile.ignore();

                        // Optional vertex normal
                        InFile >> iNormal;

                        uint32_t normIndex = 0;
                        if (!iNormal)
                        {
                            // 0 is not allowed for index
                            return E_UNEXPECTED;
                        }
                        else if (iNormal < 0)
                        {
                            // Negative values are relative indices
                            normIndex = uint32_t(ptrdiff_t(normals.size()) + iNormal);
                        }
                        else
                        {
                            // OBJ format uses 1-based arrays
                            normIndex = uint32_t(iNormal - 1);
                        }

                        if (normIndex >= normals.size())
                            return E_FAIL;

                        vertex.normal = normals[normIndex];
                    }
                }

                // If a duplicate vertex doesn't exist, add this vertex to the Vertices
                // list. Store the index in the Indices array. The Vertices and Indices
                // lists will eventually become the Vertex Buffer and Index Buffer for
                // the mesh.
                uint32_t index = AddVertex(vertexIndex, &vertex, vertexCache);
                if (index == uint32_t(-1))
                    return E_OUTOFMEMORY;

//#pragma warning( suppress : 4127 )
//                if (sizeof(index_t) == 2 && (index >= 0xFFFF))
//                {
//                    // Too many indices for 16-bit IB!
//                    return E_FAIL;
//                }
//#pragma warning( suppress : 4127 )
//                else if (sizeof(index_t) == 4 && (index >= 0xFFFFFFFF))
//                {
//                    // Too many indices for 32-bit IB!
//                    return E_FAIL;
//                }

                faceIndex[iFace] = index;
                ++iFace;

                // Check for more face data or end of the face statement
                bool faceEnd = false;
                for (;;)
                {
                    wchar_t p = InFile.peek();

                    if ('\n' == p || !InFile)
                    {
                        faceEnd = true;
                        break;
                    }
                    else if (isdigit(p) || p == '-' || p == '+')
                        break;

                    InFile.ignore();
                }

                if (faceEnd)
                    break;
            }

            //if (iFace < 3)
            //{
            //    // Need at least 3 points to form a triangle
            //    return E_FAIL;
            //}

            //// Convert polygons to triangles
            //uint32_t i0 = faceIndex[0];
            //uint32_t i1 = faceIndex[1];

            //for (size_t j = 2; j < iFace; ++j)
            //{
            //    uint32_t index = faceIndex[j];
            //    indices.emplace_back(static_cast<index_t>(i0));
            //    if (ccw)
            //    {
            //        indices.emplace_back(static_cast<index_t>(i1));
            //        indices.emplace_back(static_cast<index_t>(index));
            //    }
            //    else
            //    {
            //        indices.emplace_back(static_cast<index_t>(index));
            //        indices.emplace_back(static_cast<index_t>(i1));
            //    }

            //    attributes.emplace_back(curSubset);

            //    i1 = index;
            //}

            //assert(attributes.size() * 3 == indices.size());
        }
    }

	return S_OK;
}

uint32_t OBJFileLoader::AddVertex(uint32_t hash, const Vertex* pVertex, VertexCache& cache)
{
    auto f = cache.equal_range(hash);

    for (auto it = f.first; it != f.second; ++it)
    {
        auto& tv = vertices[it->second];

        if (0 == memcmp(pVertex, &tv, sizeof(Vertex)))
        {
            return it->second;
        }
    }

    auto index = static_cast<uint32_t>(vertices.size());
    vertices.emplace_back(*pVertex);

    VertexCache::value_type entry(hash, index);
    cache.insert(entry);
    return index;
}

void OBJFileLoader::processLine(std::wifstream& InFile, std::vector<XMFLOAT3>& positions, LineType lineType)
{
    switch (lineType)
    {
    case Ignore:
        break;
    case VertexPosition:

        // Vertex Position
        float x, y, z;
        InFile >> x >> y >> z;
        positions.emplace_back(XMFLOAT3(x, y, z));

        break;
    default:
        break;
    }
}
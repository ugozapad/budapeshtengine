#include "pch.h"
#include "level_mesh.h"

// name collision inside OpenFBX
#ifdef VOID
#	undef VOID
#endif

#include <OpenFBX/ofbx.h>

using namespace ofbx;

#define fclose_s(file) do { fclose(file); file = NULL; } while (0)

static bool fbxconv_process_geometry(FILE* pOutFile, Geometry const* pGeom, ConversionOptions options)
{
	print("  INFO: Processing geometry..." NL);

	print("    INFO: Processing vertices..." NL);
	uint32_t const dwVertCount		= pGeom->getVertexCount();
	uint32_t const dwIndicesCount	= pGeom->getIndexCount();
	Vec3 const* pVertex				= pGeom->getVertices();
	Vec2 const* pUVVert				= pGeom->getUVs();
	LevelMeshVertex_LM vertex;

	if (!pUVVert)
	{
		print("      ERROR: Mesh have no UV!");
		return (false);
	}
	if (dwVertCount != dwIndicesCount)
	{
		print("      WARNING: Mesh is not/incorrectly triangulated, UV's might be invalid!");
	}

#ifndef TEST_NO_OUTPUT
	fwrite(&dwVertCount, sizeof(dwVertCount), 1, pOutFile);
#endif
	for (uint32_t I = 0; I < dwVertCount; ++I)
	{
		// position
		vertex.position.x = float(pVertex[I].x); 
		vertex.position.y = float(pVertex[I].y); 
		vertex.position.z = float(pVertex[I].z); 

		// NOTE : UVs mapped to indices, not vertices, so if model is not triangulated dwIndicesCount < dwVertCount
		if (I < dwIndicesCount)
		{
			// diffuse texture UV
			vertex.texcoord0.x = float(pUVVert[I].x);
			vertex.texcoord0.y = float(pUVVert[I].y);

			// lightmap texture UV
			vertex.texcoord1.x = float(pUVVert[I].x);
			vertex.texcoord1.y = float(pUVVert[I].y);
		}
		else
		{
			// diffuse texture UV
			vertex.texcoord0.x = 0.f;
			vertex.texcoord0.y = 0.f;

			// lightmap texture UV
			vertex.texcoord1.x = 0.f;
			vertex.texcoord1.y = 0.f;
		}
#ifndef TEST_NO_OUTPUT
		fwrite(&vertex, sizeof(vertex), 1, pOutFile);
#endif
	}

	print("    INFO: Processing indices..." NL);
#ifndef TEST_NO_OUTPUT
	uint16_t * pTmp = new uint16_t[dwIndicesCount];
#endif
	int const* pIndex = pGeom->getFaceIndices();
#ifndef TEST_NO_OUTPUT
	fwrite(&dwIndicesCount, sizeof(dwIndicesCount), 1, pOutFile);
#endif
	for (uint32_t I = 0; I < dwIndicesCount; ++I)
	{
		int index = pIndex[I];
		if (index < 0)
			index = -index;
		else ++index;
		uint16_t const corIndex = index & 0xFFFF;
		if (index > USHORT_MAX)
		{
			print("    WARNING: index[%u] is out of bound, (%u => %hu)" NL, I, index, corIndex);
		}
#ifndef TEST_NO_OUTPUT
		pTmp[I] = corIndex;
#endif
	}

#ifndef TEST_NO_OUTPUT
	fwrite(pTmp, sizeof(uint16_t) * dwIndicesCount, 1, pOutFile);
	delete[] pTmp; pTmp = NULL;
#endif
	return (true);
}

static bool fbxconv_process_lightmap(FILE* pOutFile, Mesh const* pMesh, Material const* pMat, ConversionOptions options)
{
#ifndef TEST_NO_OUTPUT
	char texturePath[MAX_PATH];
	uint32_t dwTexturePathLen;
	// lightmap texture
	strcpy_s(texturePath, pMesh->name);
	strcat_s(texturePath, TEXTURE_EXTENSION);
	dwTexturePathLen = uint32_t(strlen(texturePath));
	fwrite(&dwTexturePathLen, sizeof(dwTexturePathLen), 1, pOutFile);
	fwrite(texturePath, dwTexturePathLen, 1, pOutFile);
#endif
	return (true);
}

static bool fbxconv_process_diffuse(FILE* pOutFile, Mesh const* pMesh, Material const* pMat, ConversionOptions options)
{
#ifndef TEST_NO_OUTPUT
	char texturePath[MAX_PATH];
	uint32_t dwTexturePathLen;
#endif
	if (Texture const* pDiffuse = pMat->getTexture(Texture::DIFFUSE))
	{
#ifndef TEST_NO_OUTPUT
		pDiffuse->getFileName().toString(texturePath);
		dwTexturePathLen = uint32_t(strlen(texturePath));
#endif
	}
	else
	{
		print("    WARNING: Material[%s] do not have diffuse texture!", pMat->name);
#ifndef TEST_NO_OUTPUT
		strcpy_s(texturePath, DEFAULT_NO_TEXTURE);
		dwTexturePathLen = sizeof(DEFAULT_NO_TEXTURE) - 1;
#endif
	}

#ifndef TEST_NO_OUTPUT
	// diffuse texture
	fwrite(&dwTexturePathLen, sizeof(dwTexturePathLen), 1, pOutFile);
	fwrite(texturePath, dwTexturePathLen, 1, pOutFile);
#endif
	return (true);
}

static bool fbxconv_process_material(FILE* pOutFile, Mesh const* pMesh, Material const* pMat, ConversionOptions options)
{
	dbg_print("    DEBUG: ... %s" NL, pMat->name);
	if (fbxconv_process_diffuse(pOutFile, pMesh, pMat, options))
		return (fbxconv_process_lightmap(pOutFile, pMesh, pMat, options));
	return (false);
}

static bool fbxconv_process_materials(FILE* pOutFile, Mesh const* pMesh, ConversionOptions options)
{
	print("  INFO: Processing materials..." NL);
	uint32_t const dwMatCount = pMesh->getMaterialCount();
	if (dwMatCount == 0)
	{
		print("    ERROR: Mesh[%s] has no materials!", pMesh->name);
		return (false);
	}
	Material const* pMat = pMesh->getMaterial(0);
	if (dwMatCount > 1)
	{
		print("    WARNING: Mesh[%s] has more than one material!", pMesh->name);
		print("             Material[%s] will be used as main!", pMat->name);
	}
	return (fbxconv_process_material(pOutFile, pMesh, pMat, options));
}

static bool fbxconv_process_mesh(FILE* pOutFile, Mesh const* pMesh, ConversionOptions options)
{
	bool bAllOk = true;
	print("  INFO: ... %s" NL, pMesh->name);
	// LevelMeshMaterial
	bAllOk = fbxconv_process_materials(pOutFile, pMesh, options);
	if (!bAllOk) return (bAllOk);

	// LevelMesh
#ifndef TEST_NO_OUTPUT
	uint32_t const dwTmpSize = uint32_t(strlen(pMesh->name));
	fwrite(&dwTmpSize, sizeof(dwTmpSize), 1, pOutFile);
	fwrite(pMesh->name, dwTmpSize, 1, pOutFile);
#endif
	bAllOk = fbxconv_process_geometry(pOutFile, pMesh->getGeometry(), options);
	return (bAllOk);
}

static bool fbxconv_process_meshes(FILE* pOutFile, IScene* pScene, ConversionOptions options)
{
	bool bAllOk = true;
	uint32_t const dwMeshesCount = pScene->getMeshCount();

	print("INFO: Processing meshes..." NL);
	for (uint32_t I = 0; I < dwMeshesCount && bAllOk; ++I)
	{
		bAllOk = fbxconv_process_mesh(pOutFile, pScene->getMesh(I), options);
	}
	return (bAllOk);
}

static bool fbxconv_process_header(FILE* pOutFile, IScene* pScene, ConversionOptions options)
{
	// check limits
	uint32_t const dwMeshesCount = pScene->getMeshCount();
	if (dwMeshesCount >= MAX_MESHES_ON_LEVEL)
	{
		print("ERROR: Too many meshes (max is %u)!" NL, MAX_MESHES_ON_LEVEL);
		return (false);
	}
	if (dwMeshesCount == 0)
	{
		print("ERROR: There is no meshes in scene!");
		return (false);
	}

#ifndef TEST_NO_OUTPUT
	LevelMeshHeader header;
	header.version = LEVEL_MESH_VERSION;
	header.mesh_count = uint16_t(dwMeshesCount);
	fwrite(&header, sizeof(header), 1, pOutFile);
#endif
	return (true);
}

bool fbxconv_perform(const char* inFileName, const char* outFileName, ConversionOptions options)
{
	errno_t err		= 0;
	char errBuff[128];
	bool bAllOk		= false;
	IScene* pScene	= NULL;
	FILE* pFile		= NULL;
	if (0 == (err = fopen_s(&pFile, inFileName, "rb")))
	{
		print("INFO: Reading FBX..." NL);

		ofbx::u8* pFileData = NULL;
		size_t szFile = 0;
		
		_fseeki64(pFile, 0, SEEK_END);
		szFile = _ftelli64(pFile);
		_fseeki64(pFile, 0, SEEK_SET);

		pFileData = new ofbx::u8[szFile];
		fread_s(pFileData, szFile, szFile, 1, pFile);

		LoadFlags flags =	LoadFlags::IGNORE_LIGHTS | LoadFlags::IGNORE_SKIN | LoadFlags::IGNORE_CAMERAS |
							LoadFlags::IGNORE_BONES | LoadFlags::IGNORE_LIMBS | LoadFlags::IGNORE_ANIMATIONS |
							LoadFlags::IGNORE_BLEND_SHAPES | LoadFlags::IGNORE_POSES | LoadFlags::IGNORE_PIVOTS |
							LoadFlags::IGNORE_VIDEOS;
		if (options & ConversionOptions_FBX_Triangulate) flags |= LoadFlags::TRIANGULATE;

		pScene = ofbx::load(
			pFileData,
			szFile,
			ofbx::u16(flags)
		);

		if (!pScene)
		{
			print("ERROR: %s" NL, ofbx::getError());
		}

		delete[] pFileData; pFileData = NULL;
		fclose_s(pFile);
	}
	else
	{
		strerror_s(errBuff, err);
		print("ERROR: Unable to open input file \"%s\", error: %s" NL, inFileName, errBuff);
	}

	if (pScene)
	{
		if (0 == (err = fopen_s(&pFile, outFileName, "wb")))
		{
			if (bAllOk = fbxconv_process_header(pFile, pScene, options))
				bAllOk = fbxconv_process_meshes(pFile, pScene, options);
			fclose_s(pFile);
		}
		else
		{
			strerror_s(errBuff, err);
			print("ERROR: Unable to open output file \"%s\", error: %s" NL, inFileName, errBuff);
		}
	}

	if (pScene)
	{
		pScene->destroy();
		pScene = NULL;
	}

	return (bAllOk);
}


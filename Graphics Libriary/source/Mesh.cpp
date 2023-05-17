#include <Mesh.h>
#include <fstream>

void createSphere(Vertex* &vertices, WORD* &indices, int slices, int stacks)
{
	int vertices_count = slices * stacks + 2;
	int indices_count = slices * stacks * 6;
	vertices = new demo::Vertex[vertices_count];
	indices = new WORD[indices_count];
	float delta = 1E-3;

	vertices[0] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) };
	vertices[slices * stacks + 1] = { XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f) };
	int cc = 0;

	for (int m = 1; m <= slices; m++)
	for (int n = 1; n <= stacks; n++)
	{
		float x = sin(XM_PI * m / slices) * cos(XM_2PI * n / stacks);
		float z = sin(XM_PI * m / slices) * sin(XM_2PI * n / stacks);
		float y = cos(XM_PI * m / slices);

		XMVECTOR norm = XMVector3Normalize(XMVectorSet(x, y, z, 1.0f));

		vertices[(m - 1) * stacks + n] = { XMFLOAT4(x, y, z, 1.0f), XMFLOAT4(XMVectorGetX(norm), XMVectorGetY(norm), XMVectorGetZ(norm), 1.0f) };
	}

	int counter = 0;  

	int first, second, third;

	for (int i = 1; i < stacks; i++)
	{
		indices[counter++] = 0;
		indices[counter++] = i;
		indices[counter++] = i + 1;
	}

	indices[counter++] = 0;
	indices[counter++] = stacks;
	indices[counter++] = 1;

	for (int i = 0; i < slices - 1; i++)
	{
		for (int j = 1; j < stacks; j++)
		{
			first = i * stacks + j;
			second = first + stacks;
			third = second + 1;

			indices[counter++] = first;
			indices[counter++] = second;
			indices[counter++] = third;

			second = third;
			third = first + 1;

			indices[counter++] = first;
			indices[counter++] = second;
			indices[counter++] = third;
		}

		first = i * stacks + stacks;
		second = first + stacks;
		third = first + 1;

		indices[counter++] = first;
		indices[counter++] = second;
		indices[counter++] = third;

		second = third;
		third = i * stacks + 1;

		indices[counter++] = first;
		indices[counter++] = second;
		indices[counter++] = third;

	}

	for (int i = 1; i < stacks; i++)
	{
		indices[counter++] = slices * stacks + 1;
		indices[counter++] = slices * stacks - i + 1;
		indices[counter++] = slices * stacks - i;
	}

	indices[counter++] = slices * stacks + 1;
	indices[counter++] = slices * stacks - stacks + 1;
	indices[counter++] = slices * stacks;
}

string fileExtension(string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

Mesh loadMeshFromFile_ply(string fileName)
{
	demo::Mesh mesh;
	ifstream plyFile(fileName);
	
	XMFLOAT4* coords = nullptr;
	XMFLOAT4* normals = nullptr;
	XMFLOAT2* uvs = nullptr;
	WORD* indices = nullptr;

	string str;
	while (str != "vertex")
		plyFile >> str;

	int vertices_count;
	plyFile >> vertices_count;
	coords = new XMFLOAT4[vertices_count];
	normals = new XMFLOAT4[vertices_count];

	int formatCounter = 0;
	while (str != "face")
	{
		if (str == "float")
			formatCounter++;
		plyFile >> str;
	}

	bool hasTexCoords = formatCounter == 8;
	if (hasTexCoords)
		uvs = new XMFLOAT2 [vertices_count];


	int faces_count;
	plyFile >> faces_count;
	indices = new WORD[faces_count * 3];

	while (str != "end_header")
		plyFile >> str;

	for (int i = 0; i < vertices_count; i++)
	{
		plyFile >> coords[i].x >> coords[i].y >> coords[i].z;
		plyFile >> normals[i].x >> normals[i].y >> normals[i].z;
		if (hasTexCoords)
		{
			plyFile >> uvs[i].x >> uvs[i].y;
			uvs[i].y = 1 - uvs[i].y;
		}
		coords[i].w = normals[i].w = 1.0f;
	}

	int n;
	for (int i = 0; i < faces_count; i++)
	{
		plyFile >> n;
		for (int j = 0; j < n; j++)
			plyFile >> indices[i * 3 + j];
	}

	mesh.coords = coords;
	mesh.normals = normals;
	mesh.uvs = uvs;
	mesh.verts_count = vertices_count;
	mesh.inds = indices;
	mesh.inds_count = faces_count * 3;
	plyFile.close();

	return mesh;
}
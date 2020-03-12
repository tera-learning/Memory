#pragma once
#include <vector>

struct Vertex {
	float pos[2];
	float tex[2];
};

class VertexBuffer final
{
private:
	int m_VertexNum;
	std::vector<Vertex> m_VertexList;

public:
	VertexBuffer();
	VertexBuffer(const VertexBuffer& vertexManager) = delete;
	VertexBuffer& operator=(const VertexBuffer& vertexManager) = delete;
	/*virtual*/ ~VertexBuffer();

	const Vertex* GetVertexList(void);
	int GetVertexNum(void);
	void AddVertex(const Vertex& vertex);
	int GetSize(void) const;
	void ClearVertex(void);
};
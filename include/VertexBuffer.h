#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H
#include "GL/glew.h"


class VertexBuffer{
	unsigned int m_RenderID;
public:
	VertexBuffer(const void* data,unsigned int size);
	~VertexBuffer();
	void bind();
	void unbind();
};

#endif
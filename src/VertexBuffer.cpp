#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data,unsigned int size){
	    glGenBuffers(1, &m_RenderID);
	    glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(GL_ARRAY_BUFFER, &m_RenderID);
}

void VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
}


void VertexBuffer::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const unsigned int* data,unsigned int count):m_Count(count){
    glGenBuffers(1, &m_RenderID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &m_RenderID);
}

void IndexBuffer::bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
}


void IndexBuffer::unbind()const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

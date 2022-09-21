#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H
#include "GL/glew.h"


class IndexBuffer{
	unsigned int m_RenderID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data,unsigned int count);
	~IndexBuffer();
	void bind() const;
	void unbind() const;
	inline unsigned int getCount(){return this->m_Count;}
};

#endif
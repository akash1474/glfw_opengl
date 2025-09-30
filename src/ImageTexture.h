#pragma once

#include <gl/gl.h>
#include "Types.h"
#include <future>
#include "imgui.h"

#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLAMP 0x2900
#define GL_CLAMP_TO_BORDER 0x812D

struct ImageSize
{
    int width = 0;
    int height = 0;
};

class ImageTexture
{
    fs::path mFilePath;
    GLuint mTextureId;
    bool mIsLoaded = false;

    unsigned char* img = nullptr; // raw RGBA buffer

  public:
    ImageSize mSize;
    bool isLoading = false;

    ImageTexture() {}

    // Moving is allowed
    ImageTexture(ImageTexture&& img) noexcept = default;
    ImageTexture& operator=(ImageTexture&& other) noexcept = default;


    // Copying is not allowed
    ImageTexture(const ImageTexture& img) = delete;
    ImageTexture& operator=(const ImageTexture&) = delete;

    void setFilePath(const fs::path& aPath) { this->mFilePath = aPath; }
    const fs::path getFilePath()const{return mFilePath;}
    void setPropertiesEqual(ImageTexture* pImageTexture);

    bool isLoaded() const { return mIsLoaded; }
    void setIsLoaded(bool aIsLoaded) { mIsLoaded = aIsLoaded; }

    unsigned int getTextureId() const { return mTextureId; }
    void setTextureId(GLuint aTextureId) { mTextureId = aTextureId; }

    static void LoadAsync(ImageTexture* img, std::future<bool>& future);
    static void AsyncImage(ImageTexture* img, const ImVec2& size);

    void loadFromMemory(unsigned char* img_data, size_t size);
    bool loadFromFile(const fs::path& filename);
    void bindTexture();

};

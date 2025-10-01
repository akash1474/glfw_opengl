#include "pch.h"
#include "Timer.h"
#include <cstddef>
#include <gl/gl.h>
#include "ImageTexture.h"
#include "utilities/ColorUtils.hpp"
#include "utils.h"

// STB Implementation Defines (only include in one .cpp file)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

void ImageTexture::loadFromMemory(unsigned char* img_data, size_t size)
{
    OpenGL::ScopedTimer timer("ImageTexture::LoadFromMemory");
    int width = 0, height = 0, channels = 0;

    // Decode the image buffer (force 4 channels = RGBA)
    unsigned char* img
        = stbi_load_from_memory(img_data, static_cast<int>(size), &width, &height, &channels, 4);
    if(!img)
    {
        GL_ERROR("Failed to load app image using stb_image");
        return;
    }

    // Create OpenGL texture
    glGenTextures(1, &this->mTextureId);
    glBindTexture(GL_TEXTURE_2D, this->mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    mSize.width = width;
    mSize.height = height;

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(img);
    this->mIsLoaded = true;
}


bool ImageTexture::loadFromFile(const fs::path& aFilePath)
{
    OpenGL::ScopedTimer timer("ImageTexture::LoadTexture");
    int width = 0, height = 0, channels = 0;
    if(!fs::exists(aFilePath))
    {
        GL_ERROR("InvalidPath:{}", aFilePath.u8string());
        return false;
    }

    unsigned char* originalData = stbi_load(
        aFilePath.u8string().c_str(),
        &width,
        &height,
        &channels,
        4
    ); // Force RGBA

    if(!originalData)
    {
        GL_ERROR("ImageTexture::LoadTexture : Empty Image file - {}", aFilePath.string());
        fs::remove(aFilePath);
        return false;
    }


    img = originalData;

    mSize.width = width;
    mSize.height = height;
    return true;
}

void ImageTexture::bindTexture()
{
    // Delete old texture if it exists
    if(glIsTexture(mTextureId))
    {
        glDeleteTextures(1, &mTextureId);
    }

    glGenTextures(1, &this->mTextureId);
    glBindTexture(GL_TEXTURE_2D, this->mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        mSize.width,
        mSize.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        img
    );

    // Freeing the resources
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(img);
    img = nullptr;
    mIsLoaded = true;
    isLoading = false;
}


void ImageTexture::LoadAsync(ImageTexture* img, std::future<bool>& mFuture)
{
    if(!img)
        return;
    if(!mFuture.valid())
        mFuture = std::async(std::launch::async, &ImageTexture::loadFromFile, img, img->mFilePath);

    if(!img->isLoaded() && mFuture.valid()
       && mFuture.wait_for(std::chrono::milliseconds(5)) == std::future_status::ready)
    {
        if(mFuture.get())
            img->bindTexture();
    }
}


void ImageTexture::AsyncImage(ImageTexture* img, const ImVec2& size)
{
    if(img->isLoaded())
        ImGui::Image(img->getTextureId(), size);
    else
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if(window->SkipItems)
            return;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(img->mFilePath.c_str());
        float threshold = window->Size.x - 80.0f;

        ImVec2 pos = window->DC.CursorPos;
        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImGui::ItemSize(bb, 0);
        if(!ImGui::ItemAdd(bb, id))
            return;
        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(
            pos,
            { pos.x + size.x, pos.y + size.y },
            ImColor(61, 61, 69, 255),
            5.0f
        );
    }
}

void ImageTexture::setPropertiesEqual(ImageTexture* pImageTexture)
{
    this->mTextureId = pImageTexture->getTextureId();
    this->mSize = pImageTexture->mSize;
}

#include "pch.h"
#include "MultiThreading.h"
#include "ImageTexture.h"

void MultiThreading::ImageLoader::AddImagesToQueue(
    std::vector<ImageTexture *> &images
)
{
    auto *self = Get();
    size_t size = images.size();
    for(ImageTexture *img : images)
    {
        self->mQueue.push(img);
        img->isLoading = true;
    }

    GL_INFO("Size:{}", self->mQueue.size());
    if(!self->mQueue.empty())
        MultiThreading::IsRequiredForImageLoader = true;
}

void MultiThreading::ImageLoader::PushImageToQueue(ImageTexture *img)
{
    if(img->isLoading || img->isLoaded())
        return;

    auto *self = Get();

    img->isLoading = true;
    self->mQueue.push(img);

    if(!self->mQueue.empty())
        MultiThreading::IsRequiredForImageLoader = true;
}

void MultiThreading::ImageLoader::LoadImages()
{
    if(!MultiThreading::IsRequiredForImageLoader)
        return;

    auto *self = Get();
    static bool isReserved = false;
    if(!isReserved)
    {
        self->mActiveTasks.resize(self->mThreadCount);
        isReserved = true;
    }

    auto &que = self->mQueue;
    auto &imgs = self->mActiveTasks;
    if(que.empty() && imgs.empty())
        return;

    for(size_t i = 0; i < self->mThreadCount; i++)
    {
        if(!imgs[i].mItem && !que.empty())
        {
            imgs[i] = TaskContainer(que.front());
            que.pop();
        }
        ImageTexture::LoadAsync(imgs[i].mItem, imgs[i].mFuture);
        if(imgs[i].mItem && imgs[i].mItem->isLoaded())
        {
            imgs[i] = nullptr;
            bool isFinished = true;

            for(size_t i = 0; i < self->mThreadCount; i++)
            {
                if(imgs[i].mItem && !imgs[i].mItem->isLoaded())
                    isFinished = false;
            }

            if(isFinished && que.empty())
            {
                MultiThreading::IsRequiredForImageLoader = false;
                GL_INFO("MultiThreading::ImageLoader Done Loading");
            }
        }
    }
}

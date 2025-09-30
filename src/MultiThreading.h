#pragma once

#include "ImageTexture.h"
#include <stdint.h>
#include <queue>


namespace MultiThreading
{
    inline static bool IsRequiredForImageLoader = false;

    template <typename T> struct TaskContainer
    {
        T mItem = 0;
        std::future<bool> mFuture;
        bool mIsDone = false;
        bool mHasFailed = false;
        TaskContainer() {}
        TaskContainer(T item) : mItem(item) {}
    };

    class ImageLoader
    {
        std::vector<TaskContainer<ImageTexture *>> mActiveTasks;
        uint8_t mThreadCount = 2;
        std::queue<ImageTexture*> mQueue;
        ImageLoader() {}

      public:
        ImageLoader(const ImageLoader &) = delete; // copy

        static ImageLoader *Get()
        {
            static ImageLoader mInstance;
            return &mInstance;
        }

        static void AddImagesToQueue(std::vector<ImageTexture *> &images);
        static void PushImageToQueue(ImageTexture *img);
        static void LoadImages();
        static void SetThreadCount(uint8_t thread_count)
        {
            Get()->mThreadCount = thread_count;
        }
    };
}

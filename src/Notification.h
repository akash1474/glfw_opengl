#pragma once
#include "FloatAnim.h"
#include "SequentialAnimationGroup.h"
#include "string"
#include "imgui.h"

namespace Notification
{

    enum class NotificationType
    {
        None,
        Success,
        Error,
        Info,
        Warning,
        Async
    };

    enum class AsyncNotificationStatus
    {
        None,
        Pending,
        Failed,
        Success
    };


    // --- State Variables ---
    inline bool mDisplayNotification = false;
    inline bool mIsSetupCompleted = false;
    inline std::string mTitle;
    inline NotificationType mType = NotificationType::None;
    inline AsyncNotificationStatus mAsyncStatus = AsyncNotificationStatus::None;
    inline const char* mIcon = "";
    inline ImVec4 mTxColor;
    inline ImVec4 mBgColor;
    inline ImVec4 mIconColor;

    // --- New Animation System ---
    // A single animation group to manage the entire notification lifecycle.
    inline ImAnim::SequentialAnimationGroup mNotificationLifecycle;

    // The three stages of the notification, controlled by FloatAnim.
    inline ImAnim::FloatAnim mAppearAnimation;    // Controls slide-in and fade-in
    inline ImAnim::FloatAnim mWaitAnimation;      // Acts as a timer
    inline ImAnim::FloatAnim mDisappearAnimation; // Controls slide-out and fade-out

    void Show(NotificationType type, const char* msg);
    void UpdateNotificationColor(Notification::NotificationType type);
    void SetAsyncNotificationStatus(AsyncNotificationStatus status, const std::string& aMessage);
    void Setup();


    void Render();
    void Close();
    bool IsOpen();

#ifdef GL_DEBUG
    void ShowDummyNotification();
    void ShowDummyAsyncNotification();
#endif
};
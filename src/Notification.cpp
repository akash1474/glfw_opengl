#include "pch.h"
#include "AbstractAnimation.h"
#include "imgui.h"
#include <Notification.h>
#include <chrono>
#include <ratio>
#include <thread>
#include "Application.h"
#include "imspinners.h"
#include "managers/FontManager.h"

void Notification::Setup()
{
    // 1. Appear Animation (0.5 seconds, EaseOutCubic)
    mAppearAnimation.setDuration(0.5f);
    mAppearAnimation.setEasingCurve(ImAnim::EasingCurve::Type::OutCubic);
    mAppearAnimation.setStartValue(0.0f);
    mAppearAnimation.setEndValue(1.0f);
    mNotificationLifecycle.addAnimation(&mAppearAnimation);

    // 2. Wait Animation (5 seconds, Linear) - acts as a delay.
    mWaitAnimation.setDuration(5.0f);
    mWaitAnimation.setStartValue(1.0f);
    mWaitAnimation.setEndValue(1.0f);
    mWaitAnimation.setEasingCurve(ImAnim::EasingCurve::Type::Linear);
    mNotificationLifecycle.addAnimation(&mWaitAnimation);

    // 3. Disappear Animation (0.5 seconds, EaseInCubic)
    mDisappearAnimation.setDuration(0.5f);
    mDisappearAnimation.setEasingCurve(ImAnim::EasingCurve::Type::InCubic);
    mDisappearAnimation.setStartValue(1.0f);
    mDisappearAnimation.setEndValue(0.0f);
    mNotificationLifecycle.addAnimation(&mDisappearAnimation);

    mIsSetupCompleted = true;
}

void Notification::Show(NotificationType type, const char* msg)
{
    if(!mIsSetupCompleted)
    {
        Setup();
    }

    mTitle = msg;
    mType = type;
    if(type == NotificationType::Async)
        mAsyncStatus = AsyncNotificationStatus::None;

    mDisplayNotification = true;
    UpdateNotificationColor(type);

    if(mNotificationLifecycle.isRunning())
    {
        mNotificationLifecycle.stop();
    }

    mNotificationLifecycle.start();
    GL_INFO("TotalAnimationsRunning:{}", ImAnim::totalAnimationsRunning);
}



void Notification::Close()
{
    mDisplayNotification = false;
    mNotificationLifecycle.stop();
    GL_INFO("TotalAnimationsRunning:{}", ImAnim::totalAnimationsRunning);
}

bool Notification::IsOpen() { return mDisplayNotification; }

void Notification::SetAsyncNotificationStatus(
    AsyncNotificationStatus status,
    const std::string& aMessage
)
{
    mAsyncStatus = status;
    mTitle = aMessage;
}

void Notification::Render()
{
    if(!mDisplayNotification)
    {
        return;
    }

    mNotificationLifecycle.update();

    // If the entire sequence is done, close the notification.
    if(!mNotificationLifecycle.isRunning() && mDisplayNotification)
    {
        Close();
        return;
    }

    // Determine the current animation value (progress).
    float value = 0.0f;
    auto* currentAnim = mNotificationLifecycle.getCurrentAnimation();
    if(currentAnim)
    {
        auto* floatAnim = dynamic_cast<ImAnim::FloatAnim*>(currentAnim);
        value = floatAnim->getCurrentValue();
    }


    ImVec2 wSize = ImGui::GetIO().DisplaySize;
    ImVec2 size(wSize.x - 30.0f, 50.0f);
    static ImVec2 iSize = ImGui::CalcTextSize(ICON_FA_X);
    static float iconSpace = 40.0f;
    if(size.x > 500.0f)
        size.x = 500.0f;
    float textSizeY = ImGui::CalcTextSize(mTitle.c_str(), 0, false, size.x - iconSpace - 40.0f).y;
    if(textSizeY > 50.0f)
        size.y = textSizeY + 20.0f;

    // Set color and position based on the current animation value
    mBgColor.w = value; // Fade effect
    ImGui::PushStyleColor(ImGuiCol_WindowBg, mBgColor);
    ImGui::SetNextWindowPos(
        { (wSize.x - size.x) * 0.5f, wSize.y - (size.y * value) - 10.0f }
    ); // Slide effect
    ImGui::SetNextWindowSize(size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

    ImGui::Begin(
        "##Notification",
        0,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoDecoration
    );


    ImGuiWindow* window = ImGui::GetCurrentWindow();

    if(mType == NotificationType::Async)
    {
        if(mAsyncStatus == Notification::AsyncNotificationStatus::Success)
            mType = NotificationType::Success;
        else if(mAsyncStatus == AsyncNotificationStatus::Failed)
            mType = NotificationType::Error;
        UpdateNotificationColor(mType);
    }


    if(mType == NotificationType::Async)
    {
        static float radius = 12.0f;
        static float thickness = 2.5f;
        ImGui::SetCursorPosY((window->Size.y - (radius * 2) - (2 * thickness)) * 0.5f);
        ImSpinner::SpinnerAng(
            "##sngLoader",
            radius,
            thickness,
            ImColor(255, 255, 255, 255),
            ImColor(255, 255, 255, 0),
            8.6f
        );
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, mIconColor);
        ImGui::SetCursorPosY((window->Size.y - iSize.y) * 0.5f);
        ImGui::Text("%s", mIcon);
        ImGui::PopStyleColor();
    }

    ImGui::SameLine(40.0f);


    ImGui::PushStyleColor(ImGuiCol_Text, mTxColor);
    ImGui::SetCursorPosY((window->Size.y - textSizeY) * 0.5f);
    ImGui::PushTextWrapPos(size.x - iconSpace);
    ImGui::TextWrapped("%s", mTitle.c_str());
    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(
        { size.x - iconSpace + ((iconSpace - iSize.x) * 0.5f), (size.y - iSize.y) * 0.5f }
    );
    ImGui::Text(ICON_FA_XMARK);
    if(ImGui::IsItemClicked())
        Notification::Close();


    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}



void Notification::UpdateNotificationColor(Notification::NotificationType type)
{
    switch(type)
    {
    case Notification::NotificationType::Async:
        mTxColor = ImColor(149, 166, 255);
        mBgColor = ImColor(40, 40, 40);
        break;
    case NotificationType::Info:
        mTxColor = ImColor(149, 166, 255);
        mBgColor = ImColor(40, 40, 40);
        mIcon = ICON_FA_CIRCLE_INFO;
        mIconColor = ImColor(96, 122, 255);
        break;
    case NotificationType::Error:
        mTxColor = ImColor(255, 182, 186);
        mBgColor = ImColor(40, 40, 40);
        mIcon = ICON_FA_CIRCLE_EXCLAMATION;
        mIconColor = ImColor(255, 129, 137);
        break;
    case NotificationType::Success:
        mTxColor = ImColor(116, 255, 230);
        mBgColor = ImColor(40, 40, 40);
        mIcon = ICON_FA_CIRCLE_CHECK;
        mIconColor = ImColor(82, 237, 209);
        break;
    case NotificationType::Warning:
        mTxColor = ImColor(255, 234, 81);
        mBgColor = ImColor(40, 40, 40);
        mIcon = ICON_FA_TRIANGLE_EXCLAMATION;
        mIconColor = ImColor(255, 200, 59);
        break;
    case NotificationType::None:
        mTxColor = ImColor(149, 166, 255);
        mBgColor = ImColor(40, 40, 40);
        mIcon = ICON_FA_CIRCLE_INFO;
        mIconColor = ImColor(96, 122, 255);
        break;
    }
}


#ifdef GL_DEBUG
void Notification::ShowDummyNotification()
{
    Show(NotificationType::Info, "Just a dummy notification!");
}

void Notification::ShowDummyAsyncNotification()
{
    Show(NotificationType::Async, "Starting...");
    std::thread([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        SetAsyncNotificationStatus(AsyncNotificationStatus::Pending, "Processing");
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        SetAsyncNotificationStatus(AsyncNotificationStatus::Success, "Done");
    }).detach();
}
#endif

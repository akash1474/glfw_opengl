#pragma once

class DebuggerWindow
{
  private:
    bool m_IsOpen = false;            // Controls if the window is visible
    bool m_AutoRefresh = true;        // Toggle for future async updates (currently always live)

    DebuggerWindow() = default;

    static void RenderFontsTab();



  public:
    DebuggerWindow(const DebuggerWindow&) = delete;
    DebuggerWindow& operator=(const DebuggerWindow&) = delete;

    static DebuggerWindow& Get()
    {
        static DebuggerWindow instance;
        return instance;
    }

    static void EventListener(bool aIsWindowFocused);

    /**
     * @brief The main render function to be called within your application's render loop.
     *
     * This function will draw the entire debug window and its contents.
     */
    static void Render();

    /**
     * @brief Sets the visibility of the debug window.
     * @param visible True to show the window, false to hide it.
     */
    static void SetVisible(bool visible) { Get().m_IsOpen = visible; }

    /**
     * @brief Checks if the debug window is currently visible.
     * @return True if the window is visible, false otherwise.
     */
    static bool IsVisible() { return Get().m_IsOpen; }
};
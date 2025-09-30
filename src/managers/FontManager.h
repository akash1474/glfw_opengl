#pragma once
#include "imgui.h"
#include <cstdint>
#include "Types.h"
#include <filesystem>
#include <map>

// Enum for a clean, type-safe way to request fonts
enum class FontSize:uint8_t
{
  Smaller,
  Small,
  Regular,
  Large,
  XLarge,
  Massive
};

enum class FontFamily:uint8_t
{
  Regular,
  Medium,
  SemiBold,
  Bold,
  Black,
  Unknown
};


class FontManager
{
  public:
    static FontManager& Get();

    // Initialize the manager, loading all fonts and applying DPI scale
    static void Init(float dpiScale = 1.0f);

    static ImFont* GetFont(FontFamily type);

    // New function to dynamically reload all fonts from stored paths
    static void ReloadFonts();
    static const fs::path& GetFontPath(FontFamily family);
    static void SetFontPath(FontFamily family, const fs::path& path);
    static void ProcessReloadRequests();
    static void LoadFontsFromDirectory(const fs::path& directoryPath);

    static void ApplyDpiScaling(float aDpiScale);
    static int GetGlobalFontSize(){return (int)(Get().mBaseRasterFontSize + Get().m_GlobalOffset);}
    static int GetBaseRasterFontSize(){return (int)Get().mBaseRasterFontSize;}

    // --- Font Stack Management ---
    // Push a font with its managed size onto the ImGui stack
    static void Push(FontFamily aFamily,FontSize aSize);

    // Pop the font from the stack
    static void Pop();

    // --- Global Size Control ---
    // Modify the global font size offset by a delta (e.g., +1.0f or -1.0f)
    static void ChangeSize(float delta);

    // --- State Caching ---
    static void CacheState(const fs::path& aPath);
    static void ReloadStateFromCache(const fs::path& aPath);

    static float GetDpiScale(){return Get().m_DpiScale;}
    static void SetDpiScale(float aDpiScale){Get().m_DpiScale=aDpiScale;}

  private:
    // Private constructor and destructor to enforce Singleton pattern
    FontManager() = default;
    ~FontManager() = default;
    const float mBaseRasterFontSize = 32.0f; // High-res size for quality
    std::map<FontFamily, fs::path> m_FontPaths;

    // Delete copy and assignment operators
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    float m_DpiScale=0.0f;
    bool m_FontsNeedReload = false;

    // Font pointers
    ImFont* m_FontRegular = nullptr;
    ImFont* m_FontMedium = nullptr;
    ImFont* m_FontSemiBold = nullptr;
    ImFont* m_FontBold = nullptr;
    ImFont* m_FontBlack = nullptr;

    // Base font sizes (before DPI scaling)
    float m_SizeSmaller = 12.0f;
    float m_SizeSmall = 14.0f;
    float m_SizeRegular = 16.0f;
    float m_SizeLarge = 18.0f;
    float m_SizeXLarge = 20.0f;
    float m_SizeMassive = 24.0f;

    // Final font sizes (after DPI scaling)
    float m_ScaledSizeSmaller = 12.0f;
    float m_ScaledSizeSmall = 14.0f;
    float m_ScaledSizeRegular = 16.0f;
    float m_ScaledSizeLarge = 18.0f;
    float m_ScaledSizeXLarge = 20.0f;
    float m_ScaledSizeMassive = 24.0f;

    // The global size offset, modified by user
    float m_GlobalOffset = 0.0f;
};
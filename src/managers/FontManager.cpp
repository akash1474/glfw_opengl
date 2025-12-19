#include "pch.h"
#include <filesystem>
#include <fstream>
#include "FontManager.h"

#include "Log.h"
#include "Timer.h"
#include "imgui.h"
#include "PathManager.h"
#include "imgui_freetype.h"
#include "resources/FontAwesomeSolid.embed"
#include "nlohmann/json.hpp"

FontManager& FontManager::Get()
{
    static FontManager instance;
    return instance;
}

const fs::path& FontManager::GetFontPath(FontFamily family)
{
    FontManager& fntManager = Get();
    if(fntManager.m_FontPaths.count(family))
    {
        return fntManager.m_FontPaths[family];
    }
    static const fs::path empty_string = "No Font Selected";
    return empty_string;
}

void FontManager::SetFontPath(FontFamily family, const fs::path& path)
{
    FontManager& fntManager = Get();
    fntManager.m_FontPaths[family] = path;

    // DON'T reload immediately. Just set the flag.
    fntManager.m_FontsNeedReload = true;
    CacheState(Stride::PathManager::Get().GetFontDataFile());
}

void FontManager::LoadFontsFromDirectory(const fs::path& directoryPath)
{
    if(!fs::is_directory(directoryPath))
    {
        GL_ERROR("FontManager: Provided path is not a directory: {}", directoryPath.string());
        return;
    }

    GL_INFO("FontManager: Scanning directory for fonts: {}", directoryPath.string());
    FontManager& fntManager = Get();
    bool pathsChanged = false;

    // A map to hold our findings. We'll update the main map at the end.
    std::unordered_map<FontFamily, fs::path> foundFonts;

    for(const auto& entry : fs::directory_iterator(directoryPath))
    {
        if(entry.is_regular_file() && entry.path().extension() == ".ttf")
        {
            std::string filename = entry.path().filename().string();
            // Convert to lower case for case-insensitive matching
            std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

            // Heuristic to detect font weight from filename. Order is important!
            // We check for more specific names before more generic ones (e.g., "semibold" before
            // "bold").
            if(filename.find("black") != std::string::npos)
            {
                foundFonts[FontFamily::Black] = entry.path();
            }
            else if(filename.find("extrabold") != std::string::npos)
            {
                foundFonts[FontFamily::Bold] = entry.path(); // Or Black, your choice
            }
            else if(filename.find("semibold") != std::string::npos)
            {
                foundFonts[FontFamily::SemiBold] = entry.path();
            }
            else if(filename.find("bold") != std::string::npos)
            {
                foundFonts[FontFamily::Bold] = entry.path();
            }
            else if(filename.find("medium") != std::string::npos)
            {
                foundFonts[FontFamily::Medium] = entry.path();
            }
            else if(filename.find("regular") != std::string::npos)
            {
                foundFonts[FontFamily::Regular] = entry.path();
            }
            else
            {
                // If no keywords match, assume it's the Regular font,
                // but only if we haven't found a "Regular" one already.
                if(foundFonts.find(FontFamily::Regular) == foundFonts.end())
                {
                    foundFonts[FontFamily::Regular] = entry.path();
                }
            }
        }
    }

    // Now, update the main font paths with our findings
    if(!foundFonts.empty())
    {
        for(const auto& [family, path] : foundFonts)
        {
            // Update the path in the main map
            fntManager.m_FontPaths[family] = path;
            pathsChanged = true;
            GL_INFO("FontManager: Mapped {} -> {}", path.filename().string(), (int)family);
        }
    }

    // If we updated any paths, trigger a reload and save the new state
    if(pathsChanged)
    {
        fntManager.m_FontsNeedReload = true;
        CacheState(Stride::PathManager::Get().GetFontDataFile());
    }
}

void FontManager::ProcessReloadRequests()
{
    FontManager& fntManager = Get();
    if(fntManager.m_FontsNeedReload)
    {
        ReloadFonts();
        fntManager.m_FontsNeedReload = false; // Reset the flag
    }
}

// --- MODIFIED ---
void FontManager::Init(float dpiScale)
{
    OpenGL::ScopedTimer timer("FontManager::Init");

    // Load saved settings first (this will populate m_GlobalOffset and m_FontPaths)
    ReloadStateFromCache(Stride::PathManager::Get().GetFontDataFile());

    // If no paths were loaded from cache, set some defaults
    FontManager& fntManager = Get();
    if(fntManager.m_FontPaths.empty())
    {
        fntManager.m_FontPaths[FontFamily::Regular]
            = "D:/Projects/c++/stride/assets/fonts/Recursive/Recursive-Regular.ttf";
        fntManager.m_FontPaths[FontFamily::Medium]
            = "D:/Projects/c++/stride/assets/fonts/Recursive/Recursive-Medium.ttf";
        fntManager.m_FontPaths[FontFamily::SemiBold]
            = "D:/Projects/c++/stride/assets/fonts/Recursive/Recursive-SemiBold.ttf";
        fntManager.m_FontPaths[FontFamily::Bold]
            = "D:/Projects/c++/stride/assets/fonts/Recursive/Recursive-Bold.ttf";
        fntManager.m_FontPaths[FontFamily::Black]
            = "D:/Projects/c++/stride/assets/fonts/Recursive/Recursive-Black.ttf";
    }

    fntManager.m_DpiScale = dpiScale;

    // Now perform the initial font load
    ReloadFonts();

    // Apply DPI scaling to sizes
    ApplyDpiScaling(dpiScale);
}

void FontManager::ReloadFonts()
{
    OpenGL::ScopedTimer timer("FontManager::ReloadFonts");
    FontManager& fntManager = Get();
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->Clear();

    // Config for standard fonts
    ImFontConfig font_config;
    font_config.FontDataOwnedByAtlas = false;

    // Config for merging Font Awesome icons
    ImFontConfig icon_config;
    icon_config.MergeMode = true;
    icon_config.PixelSnapH = true;
    icon_config.FontDataOwnedByAtlas = false;
    icon_config.FontLoaderFlags |= ImGuiFreeTypeLoaderFlags_LoadColor;
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    // A helper lambda to load a font and merge icons
    auto load_font = [&](FontFamily /*family*/, const std::string& path) -> ImFont* {
        if(!fs::exists(path))
            return nullptr;

        ImFont* font = io.Fonts->AddFontFromFileTTF(
            path.c_str(),
            fntManager.mBaseRasterFontSize,
            &font_config
        );
        if(font)
        {
            if(fs::exists("C:/Windows/Fonts/seguiemj.ttf"))
            {
                static const ImWchar emoji_ranges[] = { static_cast<ImWchar>(0x1F300), static_cast<ImWchar>(0x1FAFF), 0 }; // Emoji Unicode range
                io.Fonts->AddFontFromFileTTF(
                    "C:/Windows/Fonts/seguiemj.ttf",
                    fntManager.mBaseRasterFontSize, // Use the same size for better alignment
                    &icon_config,
                    emoji_ranges // Pass the emoji ranges here!
                );
            }
            else
            {
                GL_ERROR("Emoji Font File not found: C:\\Windows\\Fonts\\seguiemj.ttf");
            }

            io.Fonts->AddFontFromMemoryTTF(
                (void*)FontAwesomeSolid,
                IM_ARRAYSIZE(FontAwesomeSolid),
                fntManager.mBaseRasterFontSize,
                &icon_config,
                icons_ranges
            );
        }

        return font;
    };

    // Iterate over our stored paths and load each font
    for(auto const& [family, path] : fntManager.m_FontPaths)
    {
        ImFont* loaded_font = load_font(family, path.generic_u8string().c_str());
        if(loaded_font)
        {
            switch(family)
            {
            case FontFamily::Regular: fntManager.m_FontRegular = loaded_font; break;
            case FontFamily::Medium: fntManager.m_FontMedium = loaded_font; break;
            case FontFamily::SemiBold: fntManager.m_FontSemiBold = loaded_font; break;
            case FontFamily::Bold: fntManager.m_FontBold = loaded_font; break;
            case FontFamily::Black: fntManager.m_FontBlack = loaded_font; break;
            case FontFamily::Unknown: fntManager.m_FontRegular = loaded_font; break;
            }
        }
    }
}

ImFont* FontManager::GetFont(FontFamily family)
{
    FontManager& fntManager = Get();
    switch(family)
    {
    case FontFamily::Regular: return fntManager.m_FontRegular;
    case FontFamily::Medium: return fntManager.m_FontMedium;
    case FontFamily::SemiBold: return fntManager.m_FontSemiBold;
    case FontFamily::Bold: return fntManager.m_FontBold;
    case FontFamily::Black: return fntManager.m_FontBlack;
    default: return fntManager.m_FontRegular;
    }

    return fntManager.m_FontRegular;
}

void FontManager::ApplyDpiScaling(float aDpiScale)
{
    FontManager& fntManager = Get();
    fntManager.m_ScaledSizeSmaller = fntManager.m_SizeSmaller * aDpiScale;
    fntManager.m_ScaledSizeSmall = fntManager.m_SizeSmall * aDpiScale;
    fntManager.m_ScaledSizeRegular = fntManager.m_SizeRegular * aDpiScale;
    fntManager.m_ScaledSizeLarge = fntManager.m_SizeLarge * aDpiScale;
    fntManager.m_ScaledSizeXLarge = fntManager.m_SizeXLarge * aDpiScale;
    fntManager.m_ScaledSizeMassive = fntManager.m_SizeMassive * aDpiScale;
}

void FontManager::Push(FontFamily aFamily, FontSize aSize)
{
    FontManager& fntManager = Get();
    ImFont* font = GetFont(aFamily);
    float size = fntManager.m_ScaledSizeRegular;
    switch(aSize)
    {
    case FontSize::Smaller: size = fntManager.m_ScaledSizeSmaller; break;
    case FontSize::Small: size = fntManager.m_ScaledSizeSmall; break;
    case FontSize::Regular: size = fntManager.m_ScaledSizeRegular; break;
    case FontSize::Large: size = fntManager.m_ScaledSizeLarge; break;
    case FontSize::XLarge: size = fntManager.m_ScaledSizeXLarge; break;
    case FontSize::Massive: size = fntManager.m_ScaledSizeMassive; break;
    default: size = fntManager.m_ScaledSizeRegular;
    }


    ImGui::PushFont(font, size + fntManager.m_GlobalOffset);
}

void FontManager::Pop() { ImGui::PopFont(); }

void FontManager::ChangeSize(float delta)
{
    Get().m_GlobalOffset += delta;
    CacheState(Stride::PathManager::Get().GetFontDataFile());
}

static std::string FontFamilyToString(FontFamily family)
{
    switch(family)
    {
    case FontFamily::Regular: return "Regular";
    case FontFamily::Medium: return "Medium";
    case FontFamily::SemiBold: return "SemiBold";
    case FontFamily::Bold: return "Bold";
    case FontFamily::Black: return "Black";
    default: return "Unknown";
    }
}

// Helper to convert a string back to the FontFamily enum
static FontFamily StringToFontFamily(const std::string& str)
{
    if(str == "Regular")
        return FontFamily::Regular;
    if(str == "Medium")
        return FontFamily::Medium;
    if(str == "SemiBold")
        return FontFamily::SemiBold;
    if(str == "Bold")
        return FontFamily::Bold;
    if(str == "Black")
        return FontFamily::Black;
    return FontFamily::Unknown;
}

void FontManager::CacheState(const fs::path& aPath)
{
    FontManager& instance = Get();
    nlohmann::json cacheJson;

    cacheJson["global_offset"] = instance.m_GlobalOffset;
    cacheJson["font_paths"] = nlohmann::json::object();

    for(const auto& [family, path] : instance.m_FontPaths)
    {
        std::string familyStr = FontFamilyToString(family);
        cacheJson["font_paths"][familyStr] = path.generic_u8string();
    }

    std::ofstream file(aPath);
    if(!file.is_open())
    {
        GL_ERROR(
            "FontManager::CacheState - Failed to write font data to \"{}\"",
            aPath.generic_string()
        );
        return;
    }
#ifdef GL_DEBUG
    file << cacheJson.dump(4);
#else
    file << cacheJson;
#endif
}

void FontManager::ReloadStateFromCache(const fs::path& aPath)
{
    FontManager& instance = Get();
    std::ifstream ifs(aPath);
    if(!ifs.is_open())
    {
        GL_INFO(
            "FontManager::ReloadStateFromCache - No cache data found at: \"{}\"",
            aPath.generic_string()
        );
        return;
    }

    try
    {
        nlohmann::json cacheJson;
        ifs >> cacheJson;

        if(cacheJson.contains("global_offset"))
        {
            instance.m_GlobalOffset = cacheJson["global_offset"].get<float>();
        }

        if(cacheJson.contains("font_paths") && cacheJson["font_paths"].is_object())
        {
            instance.m_FontPaths.clear();
            for(const auto& item : cacheJson["font_paths"].items())
            {
                FontFamily family = StringToFontFamily(item.key());
                if(family != FontFamily::Unknown)
                {
                    instance.m_FontPaths[family] = item.value().get<std::string>();
                }
            }
        }
    }
    catch(const nlohmann::json::parse_error& e)
    {
        GL_ERROR("FontManager::ReloadStateFromCache - Failed to parse cache file: {}", e.what());
    }
}

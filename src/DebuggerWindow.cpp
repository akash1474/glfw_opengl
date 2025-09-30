#include "pch.h"

#include "DebuggerWindow.h"

#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

// Include your project's managers to access the data
#include "imgui.h"
#include "managers/FontManager.h"
#include "MultiThreading.h"
#include "utils.h"

// Helper function to convert a string to lowercase for case-insensitive search
static std::string toLower(const std::string& str)
{
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return lower_str;
}

static std::string FormatBytes(size_t bytes)
{
    if(bytes == 0)
        return "0 B";

    const char* suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    int i = 0;
    double dbl_bytes = static_cast<double>(bytes);

    while(dbl_bytes >= 1024 && i < 4)
    {
        dbl_bytes /= 1024;
        i++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << dbl_bytes << " " << suffixes[i];
    return oss.str();
}

// Helper to display boolean values with colored icons
static void RenderBool(bool value)
{
    if(value)
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "true");
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "false");
    }
}

void DebuggerWindow::EventListener(bool aIsWindowFocused)
{
    if(aIsWindowFocused && ImGui::IsKeyPressed(ImGuiKey_F11))
    {
        Get().m_IsOpen = !Get().m_IsOpen;
    }
}

void DebuggerWindow::Render()
{
    DebuggerWindow& dbgWindow = Get();
    if(!dbgWindow.m_IsOpen)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Debugger Window", &dbgWindow.m_IsOpen))
    {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Auto-refresh", &dbgWindow.m_AutoRefresh);
    ImGui::Separator();

    // --- Main Content: Tabs with Icons ---
    if(ImGui::BeginTabBar("ObjectTypesTabBar"))
    {

        if(ImGui::BeginTabItem(ICON_FA_FONT " Fonts"))
        {
            RenderFontsTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}



static void RenderFontSelector(const char* label, FontFamily family)
{
    ImGui::PushID(label); // Unique ID for each row

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label);

    ImGui::TableSetColumnIndex(1);
    ImGui::PushItemWidth(-FLT_MIN); // Use full column width
    // Get the current path from the FontManager
    fs::path currentPath = FontManager::GetFontPath(family);
    char buff[256];
    strcpy_s(buff,currentPath.generic_u8string().c_str());
    ImGui::InputText("##path", buff, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    ImGui::TableSetColumnIndex(2);
    if (ImGui::Button("Browse..."))
    {
        fs::path ttfFilePath = SelectTTFFile();
        if(!ttfFilePath.empty()){
            GL_WARN("DebuggerWindow::RenderFontSelector - Selected:{}",fs::path(ttfFilePath).generic_u8string());
            FontManager::SetFontPath(family, ttfFilePath);
        }
    }

    ImGui::PopID();
}

void DebuggerWindow::RenderFontsTab()
{
    ImGuiIO& io = ImGui::GetIO();
    FontManager& fntMgr = FontManager::Get();

    ImGui::Text(ICON_FA_FONT " Font Debugging");
    ImGui::Separator();

    // --- Global font size control ---
    static float fontSizeDelta = 0.0f;
    int globalSize = FontManager::GetGlobalFontSize();
    ImGui::Text("Global Font Size: %d", globalSize);

    if(ImGui::Button(" - "))
    {
        FontManager::ChangeSize(-1.0f);
    }
    ImGui::SameLine();
    if(ImGui::Button(" + "))
    {
        FontManager::ChangeSize(1.0f);
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset"))
    {
        FontManager::ChangeSize(
            -float(FontManager::GetGlobalFontSize() - FontManager::GetBaseRasterFontSize())
        );
    }

    ImGui::Separator();

    // --- DPI scaling ---
    static float dpiScale = FontManager::GetDpiScale();
    static float lastDpiScale = 1.0f;

    ImGui::PushItemWidth(100.0f);
    if(ImGui::InputFloat("DPI Scale", &dpiScale, 0.25f, 0.25f, "%.2f"))
    {
        float scaleFactor = dpiScale / lastDpiScale;

        // scale relative to previous
        ImGui::GetStyle().ScaleAllSizes(scaleFactor);

        // reload fonts with correct size
        FontManager::ApplyDpiScaling(dpiScale);
        FontManager::SetDpiScale(dpiScale);

        lastDpiScale = dpiScale;
    }
    ImGui::PopItemWidth();
    ImGui::Separator();

    ImGui::Text(ICON_FA_FOLDER_OPEN " Font File Selection");
    if (ImGui::Button(ICON_FA_FOLDER_PLUS " Select Font Folder..."))
    {
        fs::path selectedPath = SelectFolder();
        if (!selectedPath.empty())
        {
            FontManager::LoadFontsFromDirectory(selectedPath);
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Select a folder containing .ttf files (e.g., Inter-Regular.ttf, Inter-Bold.ttf).\nThe application will try to automatically assign them.");
    }

    if (ImGui::BeginTable("FontSelectors", 3, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Font Style", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("File Path", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed);

        // Render a selector row for each font family
        RenderFontSelector("Regular", FontFamily::Regular);
        RenderFontSelector("Medium", FontFamily::Medium);
        RenderFontSelector("SemiBold", FontFamily::SemiBold);
        RenderFontSelector("Bold", FontFamily::Bold);
        RenderFontSelector("Black", FontFamily::Black);

        ImGui::EndTable();
    }


    ImGui::Separator();

    // --- Editable preview text ---
    static char previewText[256] = "The quick brown fox jumps over the lazy dog 0123456789";
    ImGui::InputText("Preview Text", previewText, IM_ARRAYSIZE(previewText));

    ImGui::Separator();

    // Persistent per-font preview sizes
    static std::unordered_map<int, int> previewSizes;
    if(ImGui::Button("Reset Font Sizes"))
    {
        for(int i = 0; i < io.Fonts->Fonts.Size; i++)
        {
            previewSizes[i] = (int)io.Fonts->Fonts[i]->LegacySize;
        }
    }
    // --- List all fonts loaded in io.Fonts ---
    if(ImGui::BeginTable(
           "FontsTable",
           5,
           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
       ))
    {
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 180.0f);
        ImGui::TableSetupColumn("RasterSize", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Preview Size", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Preview");
        ImGui::TableHeadersRow();


        for(int i = 0; i < io.Fonts->Fonts.Size; i++)
        {
            ImFont* font = io.Fonts->Fonts[i];
            if(!previewSizes.count(i))
                previewSizes[i] = (int)font->LegacySize; // default to actual font size

            ImGui::TableNextRow();

            // Index
            ImGui::TableNextColumn();
            ImGui::Text("%d", i);

            // Name
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(font->GetDebugName());

            // Raster size
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", font->LegacySize);

            // Preview size slider
            ImGui::TableNextColumn();
            ImGui::PushID(i); // unique ID per row
            ImGui::InputInt("##PreviewSizex", &previewSizes[i]);
            ImGui::PopID();

            // Preview text
            ImGui::TableNextColumn();
            ImGui::PushFont(font, (float)previewSizes[i]);
            ImGui::TextUnformatted(previewText);
            ImGui::PopFont();
        }

        ImGui::EndTable();
    }
    ImGui::Separator();

    // --- Font Atlas Visualization ---
    if(io.Fonts->TexRef._TexData)
    {
        ImGui::Text("Font Atlas Texture Preview:");
        ImGui::Image(io.Fonts->TexRef, ImVec2(512, 512), ImVec2(0, 0), ImVec2(1, 1));
    }
}



#pragma once
#include <stdexcept>
#include <windows.h>
#include <shlobj.h> // SHGetKnownFolderPath

#include "Types.h"

namespace OpenGL
{
    class PathManager
    {
      public:
        PathManager(const PathManager&) = delete;
        PathManager& operator=(const PathManager&) = delete;
        PathManager(PathManager&&) = delete;
        PathManager& operator=(PathManager&&) = delete;

        // -------------------------------
        // Static API (for easy access)
        // -------------------------------
        static fs::path GetCacheDir() { return Instance().mCacheDir; }

        static fs::path GetSettingsFile() { return GetCacheDir() / "settings.json"; }

        static fs::path GetFontDataFile() { return GetCacheDir() / "fontData.json"; }

      private:
        fs::path mCacheDir;

        PathManager()
        {
#ifdef XP_DEBUG
            mCacheDir = fs::path("./.cache");
#else
            mCacheDir = GetAppDataDir() / "OpenGL";
#endif
            fs::create_directories(mCacheDir);
        }

        static PathManager& Instance()
        {
            static PathManager instance;
            return instance;
        }

        // Helper: Get LocalAppData (Windows)
        static fs::path GetAppDataDir()
        {
            PWSTR path_tmp = nullptr;
            if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path_tmp)))
            {
                fs::path path(path_tmp);
                CoTaskMemFree(path_tmp);
                return path;
            }
            throw std::runtime_error("Failed to retrieve AppData path.");
        }
    };
}

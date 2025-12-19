#include "pch.h"
#include "PathManager.h"
#include "Log.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
    #include <knownfolders.h>
#elif __APPLE__
    #include <pwd.h>
    #include <unistd.h>
    #include <mach-o/dyld.h>
#else // Linux
    #include <pwd.h>
    #include <unistd.h>
    #include <limits.h>
#endif

namespace Stride
{
    PathManager& PathManager::Get()
    {
        static PathManager instance;
        return instance;
    }

    void PathManager::Initialize(const std::string& appName)
    {
        mAppName = appName;
        SetupDefaultPaths();
        EnsureAllDirectoriesExist();
    }

    void PathManager::SetupDefaultPaths()
    {
        mPaths[AppDirectory::Cache] = GetDefaultPath(AppDirectory::Cache);
        mPaths[AppDirectory::Config] = GetDefaultPath(AppDirectory::Config);
        mPaths[AppDirectory::Data] = GetDefaultPath(AppDirectory::Data);
        mPaths[AppDirectory::Logs] = GetDefaultPath(AppDirectory::Logs);
        mPaths[AppDirectory::Temp] = GetDefaultPath(AppDirectory::Temp);
        mPaths[AppDirectory::Documents] = GetDefaultPath(AppDirectory::Documents);
        mPaths[AppDirectory::Downloads] = GetDefaultPath(AppDirectory::Downloads);
        mPaths[AppDirectory::Executable] = GetDefaultPath(AppDirectory::Executable);
    }

    fs::path PathManager::GetDefaultPath(AppDirectory dir) const
    {
#ifdef GL_DEBUG
        // In debug, use local directories for easier testing
        const fs::path devRoot = ".stride-dev";
        switch (dir)
        {
            case AppDirectory::Cache:     return devRoot / "cache";
            case AppDirectory::Config:    return devRoot / "config";
            case AppDirectory::Data:      return devRoot / "data";
            case AppDirectory::Logs:      return devRoot / "logs";
            case AppDirectory::Temp:      return devRoot / "temp";
            case AppDirectory::Documents: return GetDocumentsDirectory() / mAppName;
            case AppDirectory::Downloads: return GetDocumentsDirectory() / mAppName / "Downloads";
            case AppDirectory::Executable: return GetExecutableDirectory();
        }
#else
        switch (dir)
        {
            case AppDirectory::Cache:
                return GetLocalAppDataDirectory() / mAppName / "Cache";

            case AppDirectory::Config:
                return GetAppDataDirectory() / mAppName;

            case AppDirectory::Data:
                return GetLocalAppDataDirectory() / mAppName / "Data";

            case AppDirectory::Logs:
                return GetLocalAppDataDirectory() / mAppName / "Logs";

            case AppDirectory::Temp:
                return fs::temp_directory_path() / mAppName;

            case AppDirectory::Documents:
                return GetDocumentsDirectory() / mAppName;

            case AppDirectory::Downloads:
                return GetDocumentsDirectory() / mAppName / "Downloads";

            case AppDirectory::Executable:
                return GetExecutableDirectory();
        }
#endif
        return fs::path();
    }

    fs::path PathManager::GetDirectory(AppDirectory dir) const
    {
        // Check for custom override first
        auto customIt = mCustomPaths.find(dir);
        if (customIt != mCustomPaths.end())
        {
            return customIt->second;
        }

        auto it = mPaths.find(dir);
        return (it != mPaths.end()) ? it->second : fs::path();
    }

    fs::path PathManager::GetLogFile() const
    {
        // Generate log filename with timestamp
        auto now = std::time(nullptr);
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif

        std::ostringstream oss;
        oss << "stride_" << std::put_time(&tm, "%Y%m%d") << ".log";

        return GetLogsDir() / oss.str();
    }

    void PathManager::SetCustomPath(AppDirectory dir, const fs::path& path)
    {
        mCustomPaths[dir] = path;
    }

    void PathManager::ResetToDefaults()
    {
        mCustomPaths.clear();
        SetupDefaultPaths();
    }

    bool PathManager::EnsureDirectoryExists(AppDirectory dir) const
    {
        fs::path path = GetDirectory(dir);
        if (path.empty()) return false;

        std::error_code ec;
        if (!fs::exists(path))
        {
            if (!fs::create_directories(path, ec))
            {
                GL_ERROR("Failed to create directory: {} - {}", path.string(), ec.message());
                return false;
            }
        }
        return true;
    }

    bool PathManager::EnsureAllDirectoriesExist() const
    {
        bool success = true;
        success &= EnsureDirectoryExists(AppDirectory::Cache);
        success &= EnsureDirectoryExists(AppDirectory::Config);
        success &= EnsureDirectoryExists(AppDirectory::Data);
        success &= EnsureDirectoryExists(AppDirectory::Logs);
        success &= EnsureDirectoryExists(AppDirectory::Temp);
        return success;
    }

    bool PathManager::IsDirectoryWritable(AppDirectory dir) const
    {
        fs::path path = GetDirectory(dir);
        if (!fs::exists(path)) return false;

        // Try to create a test file
        fs::path testFile = path / ".write_test";
        std::ofstream ofs(testFile);
        bool writable = ofs.good();
        ofs.close();

        if (writable)
        {
            std::error_code ec;
            fs::remove(testFile, ec);
        }

        return writable;
    }

    // Platform-specific implementations

#ifdef _WIN32

    fs::path PathManager::GetUserHomeDirectory()
    {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &path)))
        {
            fs::path result(path);
            CoTaskMemFree(path);
            return result;
        }
        return fs::path();
    }

    fs::path PathManager::GetAppDataDirectory()
    {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path)))
        {
            fs::path result(path);
            CoTaskMemFree(path);
            return result;
        }
        return fs::path();
    }

    fs::path PathManager::GetLocalAppDataDirectory()
    {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path)))
        {
            fs::path result(path);
            CoTaskMemFree(path);
            return result;
        }
        return fs::path();
    }

    fs::path PathManager::GetDocumentsDirectory()
    {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path)))
        {
            fs::path result(path);
            CoTaskMemFree(path);
            return result;
        }
        return fs::path();
    }

    fs::path PathManager::GetExecutableDirectory()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return fs::path(buffer).parent_path();
    }

#elif __APPLE__

    fs::path PathManager::GetUserHomeDirectory()
    {
        const char* home = getenv("HOME");
        if (!home)
        {
            struct passwd* pw = getpwuid(getuid());
            home = pw ? pw->pw_dir : nullptr;
        }
        return home ? fs::path(home) : fs::path();
    }

    fs::path PathManager::GetAppDataDirectory()
    {
        return GetUserHomeDirectory() / "Library" / "Application Support";
    }

    fs::path PathManager::GetLocalAppDataDirectory()
    {
        return GetAppDataDirectory();  // Same on macOS
    }

    fs::path PathManager::GetDocumentsDirectory()
    {
        return GetUserHomeDirectory() / "Documents";
    }

    fs::path PathManager::GetExecutableDirectory()
    {
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) == 0)
        {
            return fs::path(buffer).parent_path();
        }
        return fs::path();
    }

#else // Linux

    fs::path PathManager::GetUserHomeDirectory()
    {
        const char* home = getenv("HOME");
        if (!home)
        {
            struct passwd* pw = getpwuid(getuid());
            home = pw ? pw->pw_dir : nullptr;
        }
        return home ? fs::path(home) : fs::path();
    }

    fs::path PathManager::GetAppDataDirectory()
    {
        const char* xdgConfig = getenv("XDG_CONFIG_HOME");
        if (xdgConfig)
        {
            return fs::path(xdgConfig);
        }
        return GetUserHomeDirectory() / ".config";
    }

    fs::path PathManager::GetLocalAppDataDirectory()
    {
        const char* xdgData = getenv("XDG_DATA_HOME");
        if (xdgData)
        {
            return fs::path(xdgData);
        }
        return GetUserHomeDirectory() / ".local" / "share";
    }

    fs::path PathManager::GetDocumentsDirectory()
    {
        return GetUserHomeDirectory() / "Documents";
    }

    fs::path PathManager::GetExecutableDirectory()
    {
        char buffer[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1)
        {
            buffer[len] = '\0';
            return fs::path(buffer).parent_path();
        }
        return fs::path();
    }

#endif

}

#pragma once
#include <filesystem>
#include <string>
#include <map>

namespace Stride
{
    namespace fs = std::filesystem;

    /// Standard application directories
    enum class AppDirectory
    {
        Cache,          // Temporary/cached data
        Config,         // User configuration files
        Data,           // Persistent application data
        Logs,           // Log files
        Temp,           // Temporary files (cleared on startup)
        Documents,      // User documents
        Downloads,      // Download location
        Executable,     // Where the exe is located
    };

    /// Manages all application paths
    class PathManager
    {
    public:
        static PathManager& Get();

        /// Initialize with app name (affects directory names)
        void Initialize(const std::string& appName = "Stride");

        /// Get standard directory
        fs::path GetDirectory(AppDirectory dir) const;

        /// Convenience accessors
        fs::path GetCacheDir() const { return GetDirectory(AppDirectory::Cache); }
        fs::path GetConfigDir() const { return GetDirectory(AppDirectory::Config); }
        fs::path GetDataDir() const { return GetDirectory(AppDirectory::Data); }
        fs::path GetLogsDir() const { return GetDirectory(AppDirectory::Logs); }
        fs::path GetTempDir() const { return GetDirectory(AppDirectory::Temp); }

        /// Standard files
        fs::path GetSettingsFile() const { return GetConfigDir() / "settings.json"; }
        fs::path GetFontDataFile() const { return GetConfigDir() / "fontData.json"; }
        fs::path GetDatabaseFile() const { return GetDataDir() / "stride.db"; }
        fs::path GetLogFile() const;

        /// Custom paths
        void SetCustomPath(AppDirectory dir, const fs::path& path);
        void ResetToDefaults();

        /// Utilities
        bool EnsureDirectoryExists(AppDirectory dir) const;
        bool EnsureAllDirectoriesExist() const;
        bool IsDirectoryWritable(AppDirectory dir) const;

        /// Get platform-specific paths
        static fs::path GetUserHomeDirectory();
        static fs::path GetAppDataDirectory();
        static fs::path GetLocalAppDataDirectory();
        static fs::path GetDocumentsDirectory();
        static fs::path GetExecutableDirectory();

    private:
        PathManager() = default;

        PathManager(const PathManager&) = delete;
        PathManager& operator=(const PathManager&) = delete;
        PathManager(PathManager&&) = delete;
        PathManager& operator=(PathManager&&) = delete;

        std::string mAppName = "Stride";
        std::map<AppDirectory, fs::path> mPaths;
        std::map<AppDirectory, fs::path> mCustomPaths;

        void SetupDefaultPaths();
        fs::path GetDefaultPath(AppDirectory dir) const;
    };
}

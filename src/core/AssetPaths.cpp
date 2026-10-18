#include "core/AssetPaths.h"
#include "BCT_BuildConfig.h"

#include <filesystem>
#include <vector>
#include <cstdlib>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace BachChien {
namespace {

std::string executableDir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return {};
    }
    return fs::path(std::string(buffer, length)).parent_path().string();
#else
    return {};
#endif
}

// Một thư mục được coi là gốc tài nguyên nếu nó tồn tại và có thư mục con "fonts".
bool looksLikeAssetRoot(const fs::path& candidate) {
    std::error_code ec;
    return fs::is_directory(candidate, ec) && fs::is_directory(candidate / "fonts", ec);
}

std::string detectAssetRoot() {
    std::vector<fs::path> candidates;

    if (const char* fromEnv = std::getenv("BCT_ASSETS")) {
        candidates.emplace_back(fromEnv);
    }

    candidates.emplace_back("assets");

    const std::string exeDir = executableDir();
    if (!exeDir.empty()) {
        candidates.emplace_back(fs::path(exeDir) / "assets");
    }

    candidates.emplace_back("../assets");
    candidates.emplace_back("../../assets");
    candidates.emplace_back("../../../assets");

#ifdef BCT_SOURCE_ASSETS_DIR
    candidates.emplace_back(BCT_SOURCE_ASSETS_DIR);
#endif

    for (const auto& candidate : candidates) {
        std::error_code ec;
        if (looksLikeAssetRoot(candidate)) {
            fs::path absolute = fs::absolute(candidate, ec);
            std::string resolved = ec ? candidate.string() : absolute.string();
            // Chuẩn hoá dấu phân cách để phần còn lại của chương trình dùng "/".
            for (char& ch : resolved) {
                if (ch == '\\') ch = '/';
            }
            while (resolved.size() > 1 && resolved.back() == '/') {
                resolved.pop_back();
            }
            return resolved;
        }
    }

    return "assets";
}

bool isAbsolutePath(const std::string& path) {
    if (path.empty()) return false;
    if (path.front() == '/' || path.front() == '\\') return true;
    return path.size() > 1 && path[1] == ':';
}

} // namespace

const std::string& assetRoot() {
    static const std::string root = detectAssetRoot();
    return root;
}

std::string assetPath(const std::string& relative) {
    if (relative.empty()) {
        return assetRoot();
    }
    if (isAbsolutePath(relative)) {
        return relative;
    }
    return assetRoot() + "/" + relative;
}

} // namespace BachChien

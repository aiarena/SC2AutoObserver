#include "Tools.h"

#include <filesystem>

namespace fs = std::filesystem;


bool loadReplayPaths(const std::string& path, std::vector<std::string>& dst)
{
    if (fs::path(path).extension() == ".SC2Replay") {
        dst.push_back(path);
        return false;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.path().extension() != ".SC2Replay")
            continue;

        dst.push_back(entry.path().string());
    }

    return true;
}

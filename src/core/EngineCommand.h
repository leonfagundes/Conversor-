#pragma once

#include "FileCategory.h"

#include <filesystem>
#include <string>
#include <vector>

namespace conversor {

struct EngineCommand {
    std::string executableKey;
    std::vector<std::string> arguments;
};

class EngineCommandBuilder {
public:
    static EngineCommand build(
        FileCategory category,
        const std::filesystem::path& inputPath,
        const std::filesystem::path& outputPath,
        const std::string& targetExtension);
};

} // namespace conversor

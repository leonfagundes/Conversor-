#pragma once

#include "FormatRegistry.h"
#include "MediaFile.h"

#include <filesystem>
#include <string>
#include <vector>

namespace conversor {

struct ConversionRequest {
    std::vector<MediaFile> inputs;
    FileCategory category = FileCategory::Unknown;
    std::string targetExtension;
    std::filesystem::path outputDirectory;
};

struct PlannedConversion {
    MediaFile input;
    std::filesystem::path outputPath;
    FormatOption target;
    bool lossy = false;
    std::vector<std::string> warnings;
};

class ConversionPlan {
public:
    static std::vector<PlannedConversion> build(const ConversionRequest& request);
};

} // namespace conversor

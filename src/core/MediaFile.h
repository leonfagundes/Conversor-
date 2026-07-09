#pragma once

#include "FileCategory.h"

#include <cstdint>
#include <filesystem>
#include <string>

namespace conversor {

struct MediaFile {
    std::filesystem::path path;
    std::string extension;
    FileCategory category = FileCategory::Unknown;
    std::uintmax_t sizeBytes = 0;
};

} // namespace conversor

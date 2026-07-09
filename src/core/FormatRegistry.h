#pragma once

#include "FileCategory.h"
#include "MediaFile.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace conversor {

struct FormatOption {
    std::string extension;
    std::string label;
    bool losslessCapable = false;
    bool lossyByNature = false;
    std::string warning;
};

class FormatRegistry {
public:
    static std::string normalizeExtension(std::string_view extension);
    static bool areEquivalentExtensions(std::string_view left, std::string_view right);
    static FileCategory categoryForExtension(std::string_view extension);
    static std::vector<FormatOption> targetFormats(FileCategory category);
    static std::optional<FormatOption> findTarget(FileCategory category, std::string_view extension);
    static MediaFile analyzePath(const std::filesystem::path& path, std::uintmax_t sizeBytes = 0);
};

} // namespace conversor

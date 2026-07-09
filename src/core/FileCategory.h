#pragma once

#include <string>

namespace conversor {

enum class FileCategory {
    Image,
    Audio,
    Video,
    Document,
    Unknown,
};

std::string categoryName(FileCategory category);

} // namespace conversor

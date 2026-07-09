#include "FormatRegistry.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <unordered_map>

namespace conversor {
namespace {

std::string trim(std::string value)
{
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();

    if (first >= last) {
        return {};
    }

    return std::string(first, last);
}

std::string canonicalExtension(std::string_view extension)
{
    auto value = trim(std::string(extension));

    if (!value.empty() && value.front() == '.') {
        value.erase(value.begin());
    }

    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });

    if (value == "jpeg") {
        return "jpg";
    }
    if (value == "tif") {
        return "tiff";
    }
    if (value == "m4v") {
        return "mp4";
    }
    if (value == "htm") {
        return "html";
    }

    return value;
}

bool contains(const std::vector<std::string>& values, const std::string& needle)
{
    return std::find(values.begin(), values.end(), needle) != values.end();
}

const std::vector<std::string>& imageExtensions()
{
    static const std::vector<std::string> values = {
        "jpg", "png", "webp", "gif", "bmp", "tiff", "heic", "avif", "svg"
    };
    return values;
}

const std::vector<std::string>& audioExtensions()
{
    static const std::vector<std::string> values = {
        "mp3", "wav", "flac", "aac", "m4a", "ogg", "opus", "aiff", "alac"
    };
    return values;
}

const std::vector<std::string>& videoExtensions()
{
    static const std::vector<std::string> values = {
        "mp4", "mkv", "mov", "avi", "webm", "wmv", "flv", "mpeg", "mpg"
    };
    return values;
}

const std::vector<std::string>& documentExtensions()
{
    static const std::vector<std::string> values = {
        "pdf", "docx", "doc", "odt", "rtf", "txt", "md", "html", "epub",
        "pptx", "ppt", "odp", "xlsx", "xls", "ods", "csv"
    };
    return values;
}

std::vector<FormatOption> imageTargets()
{
    return {
        {"png", "PNG", true, false, {}},
        {"jpg", "JPEG", false, true, "JPEG uses lossy compression; visual detail can be discarded."},
        {"webp", "WebP", true, false, "WebP can be lossless or lossy depending on selected options."},
        {"tiff", "TIFF", true, false, {}},
        {"bmp", "Bitmap", true, false, {}},
    };
}

std::vector<FormatOption> audioTargets()
{
    return {
        {"flac", "FLAC", true, false, {}},
        {"wav", "WAV", true, false, {}},
        {"alac", "ALAC", true, false, {}},
        {"mp3", "MP3", false, true, "MP3 is a lossy audio format; choose FLAC or WAV to preserve audio."},
        {"aac", "AAC", false, true, "AAC is a lossy audio format; choose FLAC or WAV to preserve audio."},
        {"opus", "Opus", false, true, "Opus is generally used as a lossy audio format."},
    };
}

std::vector<FormatOption> videoTargets()
{
    return {
        {"mp4", "MP4", false, false, "MP4 can preserve streams when codecs are compatible; otherwise recoding may be required."},
        {"mkv", "Matroska", true, false, "MKV can often remux compatible streams without quality loss."},
        {"webm", "WebM", false, true, "WebM output usually requires video recoding."},
        {"mov", "QuickTime MOV", false, false, "MOV can preserve streams when codecs are compatible; otherwise recoding may be required."},
    };
}

std::vector<FormatOption> documentTargets()
{
    return {
        {"pdf", "PDF", true, false, "Office documents usually export cleanly to PDF; reverse conversion can alter structure."},
        {"docx", "Word Document", false, false, "Document conversion can alter layout, fonts, or structure."},
        {"odt", "OpenDocument Text", false, false, "Document conversion can alter layout, fonts, or structure."},
        {"html", "HTML", false, false, "Document conversion can change styling and structure."},
        {"md", "Markdown", false, false, "Markdown preserves text structure but may drop visual layout."},
        {"epub", "EPUB", false, false, "EPUB conversion can change pagination and layout."},
    };
}

} // namespace

std::string categoryName(FileCategory category)
{
    switch (category) {
    case FileCategory::Image:
        return "Image";
    case FileCategory::Audio:
        return "Audio";
    case FileCategory::Video:
        return "Video";
    case FileCategory::Document:
        return "Document";
    case FileCategory::Unknown:
        return "Unknown";
    }

    return "Unknown";
}

std::string FormatRegistry::normalizeExtension(std::string_view extension)
{
    return canonicalExtension(extension);
}

bool FormatRegistry::areEquivalentExtensions(std::string_view left, std::string_view right)
{
    return normalizeExtension(left) == normalizeExtension(right);
}

FileCategory FormatRegistry::categoryForExtension(std::string_view extension)
{
    const auto normalized = normalizeExtension(extension);

    if (contains(imageExtensions(), normalized)) {
        return FileCategory::Image;
    }
    if (contains(audioExtensions(), normalized)) {
        return FileCategory::Audio;
    }
    if (contains(videoExtensions(), normalized)) {
        return FileCategory::Video;
    }
    if (contains(documentExtensions(), normalized)) {
        return FileCategory::Document;
    }

    return FileCategory::Unknown;
}

std::vector<FormatOption> FormatRegistry::targetFormats(FileCategory category)
{
    switch (category) {
    case FileCategory::Image:
        return imageTargets();
    case FileCategory::Audio:
        return audioTargets();
    case FileCategory::Video:
        return videoTargets();
    case FileCategory::Document:
        return documentTargets();
    case FileCategory::Unknown:
        return {};
    }

    return {};
}

std::optional<FormatOption> FormatRegistry::findTarget(FileCategory category, std::string_view extension)
{
    const auto normalized = normalizeExtension(extension);
    const auto targets = targetFormats(category);
    const auto found = std::find_if(targets.begin(), targets.end(), [&](const FormatOption& option) {
        return option.extension == normalized;
    });

    if (found == targets.end()) {
        return std::nullopt;
    }

    return *found;
}

MediaFile FormatRegistry::analyzePath(const std::filesystem::path& path, std::uintmax_t sizeBytes)
{
    MediaFile file;
    file.path = path;
    file.extension = normalizeExtension(path.extension().string());
    file.category = categoryForExtension(file.extension);
    file.sizeBytes = sizeBytes;
    return file;
}

} // namespace conversor

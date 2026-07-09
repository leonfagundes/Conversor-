#include "EngineCommand.h"

#include "FormatRegistry.h"

#include <stdexcept>

namespace conversor {
namespace {

std::string pathString(const std::filesystem::path& path)
{
    return path.generic_string();
}

bool isPandocDocumentTarget(const std::string& targetExtension)
{
    const auto normalized = FormatRegistry::normalizeExtension(targetExtension);
    return normalized == "md" || normalized == "html" || normalized == "epub";
}

EngineCommand buildFfmpegCommand(
    const std::filesystem::path& inputPath,
    const std::filesystem::path& outputPath,
    const std::string& targetExtension)
{
    const auto normalized = FormatRegistry::normalizeExtension(targetExtension);
    EngineCommand command;
    command.executableKey = "ffmpeg";
    command.arguments = {"-y", "-i", pathString(inputPath)};

    if (normalized == "mp3") {
        command.arguments.insert(command.arguments.end(), {"-vn", "-q:a", "0"});
    } else if (normalized == "aac") {
        command.arguments.insert(command.arguments.end(), {"-vn", "-c:a", "aac", "-b:a", "320k"});
    } else if (normalized == "flac") {
        command.arguments.insert(command.arguments.end(), {"-vn", "-c:a", "flac"});
    } else if (normalized == "wav") {
        command.arguments.insert(command.arguments.end(), {"-vn", "-c:a", "pcm_s16le"});
    } else if (normalized == "mp4" || normalized == "mkv" || normalized == "mov") {
        command.arguments.insert(command.arguments.end(), {"-c", "copy"});
    }

    command.arguments.push_back(pathString(outputPath));
    return command;
}

EngineCommand buildImageMagickCommand(
    const std::filesystem::path& inputPath,
    const std::filesystem::path& outputPath)
{
    return {
        "magick",
        {pathString(inputPath), pathString(outputPath)}
    };
}

EngineCommand buildLibreOfficeCommand(
    const std::filesystem::path& inputPath,
    const std::filesystem::path& outputPath,
    const std::string& targetExtension)
{
    return {
        "soffice",
        {
            "--headless",
            "--convert-to",
            FormatRegistry::normalizeExtension(targetExtension),
            "--outdir",
            pathString(outputPath.parent_path()),
            pathString(inputPath),
        }
    };
}

EngineCommand buildPandocCommand(
    const std::filesystem::path& inputPath,
    const std::filesystem::path& outputPath)
{
    return {
        "pandoc",
        {
            pathString(inputPath),
            "-o",
            pathString(outputPath),
        }
    };
}

} // namespace

EngineCommand EngineCommandBuilder::build(
    FileCategory category,
    const std::filesystem::path& inputPath,
    const std::filesystem::path& outputPath,
    const std::string& targetExtension)
{
    switch (category) {
    case FileCategory::Audio:
    case FileCategory::Video:
        return buildFfmpegCommand(inputPath, outputPath, targetExtension);
    case FileCategory::Image:
        return buildImageMagickCommand(inputPath, outputPath);
    case FileCategory::Document:
        if (isPandocDocumentTarget(targetExtension)) {
            return buildPandocCommand(inputPath, outputPath);
        }
        return buildLibreOfficeCommand(inputPath, outputPath, targetExtension);
    case FileCategory::Unknown:
        break;
    }

    throw std::invalid_argument("Cannot build an engine command for an unknown category.");
}

} // namespace conversor

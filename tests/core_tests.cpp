#include "core/ConversionPlan.h"
#include "core/EngineCommand.h"
#include "core/FormatRegistry.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message)
{
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

void test_normalizes_extensions()
{
    expect(conversor::FormatRegistry::normalizeExtension(".JPEG") == "jpg",
           "JPEG should normalize to jpg");
    expect(conversor::FormatRegistry::normalizeExtension("  .Mp4  ") == "mp4",
           "extensions should trim whitespace and lowercase");
    expect(conversor::FormatRegistry::normalizeExtension("tiff") == "tiff",
           "extensions without a leading dot should be accepted");
}

void test_detects_categories()
{
    using conversor::FileCategory;

    expect(conversor::FormatRegistry::categoryForExtension(".jpeg") == FileCategory::Image,
           "jpeg should be an image");
    expect(conversor::FormatRegistry::categoryForExtension("flac") == FileCategory::Audio,
           "flac should be audio");
    expect(conversor::FormatRegistry::categoryForExtension("mkv") == FileCategory::Video,
           "mkv should be video");
    expect(conversor::FormatRegistry::categoryForExtension("docx") == FileCategory::Document,
           "docx should be a document");
    expect(conversor::FormatRegistry::categoryForExtension("unknown") == FileCategory::Unknown,
           "unknown extensions should stay unknown");
}

void test_groups_equivalent_extensions()
{
    expect(conversor::FormatRegistry::areEquivalentExtensions(".jpg", ".jpeg"),
           "jpg and jpeg should be equivalent");
    expect(conversor::FormatRegistry::areEquivalentExtensions(".tif", ".tiff"),
           "tif and tiff should be equivalent");
    expect(!conversor::FormatRegistry::areEquivalentExtensions(".jpg", ".png"),
           "jpg and png should not be equivalent");
}

void test_returns_targets_by_category()
{
    using conversor::FileCategory;

    const auto imageTargets = conversor::FormatRegistry::targetFormats(FileCategory::Image);
    const auto audioTargets = conversor::FormatRegistry::targetFormats(FileCategory::Audio);
    const auto videoTargets = conversor::FormatRegistry::targetFormats(FileCategory::Video);
    const auto documentTargets = conversor::FormatRegistry::targetFormats(FileCategory::Document);

    expect(!imageTargets.empty(), "image targets should be available");
    expect(!audioTargets.empty(), "audio targets should be available");
    expect(!videoTargets.empty(), "video targets should be available");
    expect(!documentTargets.empty(), "document targets should be available");

    expect(conversor::FormatRegistry::findTarget(FileCategory::Image, "png").has_value(),
           "png should be an image target");
    expect(conversor::FormatRegistry::findTarget(FileCategory::Audio, "flac").has_value(),
           "flac should be an audio target");
    expect(conversor::FormatRegistry::findTarget(FileCategory::Video, "mp4").has_value(),
           "mp4 should be a video target");
    expect(conversor::FormatRegistry::findTarget(FileCategory::Document, "pdf").has_value(),
           "pdf should be a document target");
}

void test_exposes_quality_warnings()
{
    using conversor::FileCategory;

    const auto mp3 = conversor::FormatRegistry::findTarget(FileCategory::Audio, "mp3");
    const auto flac = conversor::FormatRegistry::findTarget(FileCategory::Audio, "flac");
    const auto jpeg = conversor::FormatRegistry::findTarget(FileCategory::Image, "jpeg");

    expect(mp3.has_value(), "mp3 target should exist");
    expect(mp3->lossyByNature, "mp3 should be marked lossy");
    expect(!mp3->warning.empty(), "mp3 should include a warning");
    expect(flac.has_value(), "flac target should exist");
    expect(flac->losslessCapable, "flac should be marked lossless-capable");
    expect(!flac->lossyByNature, "flac should not be marked lossy");
    expect(jpeg.has_value(), "jpeg target should resolve to jpg");
    expect(jpeg->lossyByNature, "jpeg should be marked lossy");
}

void test_analyzes_paths_without_touching_disk()
{
    const auto file = conversor::FormatRegistry::analyzePath("C:/media/photo.JPEG", 1234);

    expect(file.category == conversor::FileCategory::Image, "photo.JPEG should analyze as image");
    expect(file.extension == "jpg", "photo.JPEG should normalize extension");
    expect(file.sizeBytes == 1234, "analyzePath should keep provided size");
}

void test_builds_conversion_plan()
{
    using conversor::FileCategory;

    conversor::ConversionRequest request;
    request.category = FileCategory::Audio;
    request.targetExtension = "mp3";
    request.outputDirectory = "C:/out";
    request.inputs = {
        conversor::FormatRegistry::analyzePath("C:/in/song.flac", 100),
        conversor::FormatRegistry::analyzePath("C:/in/voice.wav", 200),
    };

    const auto plan = conversor::ConversionPlan::build(request);

    expect(plan.size() == 2, "plan should contain both audio files");
    expect(plan[0].outputPath.filename().string() == "song.mp3",
           "first output should use target extension");
    expect(plan[1].outputPath.filename().string() == "voice.mp3",
           "second output should use target extension");
    expect(plan[0].lossy, "mp3 plan item should be marked lossy");
    expect(!plan[0].warnings.empty(), "lossy target should produce warning");
}

void test_rejects_category_mismatch()
{
    conversor::ConversionRequest request;
    request.category = conversor::FileCategory::Audio;
    request.targetExtension = "flac";
    request.outputDirectory = "C:/out";
    request.inputs = {
        conversor::FormatRegistry::analyzePath("C:/in/photo.png", 100),
    };

    bool threw = false;
    try {
        static_cast<void>(conversor::ConversionPlan::build(request));
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    expect(threw, "planner should reject files outside the selected category");
}

void test_builds_ffmpeg_audio_command()
{
    const auto command = conversor::EngineCommandBuilder::build(
        conversor::FileCategory::Audio,
        "C:/in/song.flac",
        "C:/out/song.mp3",
        "mp3");

    expect(command.executableKey == "ffmpeg", "audio conversion should use ffmpeg");
    expect(command.arguments.size() >= 6, "ffmpeg command should include arguments");
    expect(command.arguments[0] == "-y", "ffmpeg command should overwrite output");
    expect(command.arguments[1] == "-i", "ffmpeg command should include input flag");
    expect(command.arguments[2] == "C:/in/song.flac", "ffmpeg command should include input path");
    expect(command.arguments.back() == "C:/out/song.mp3", "ffmpeg command should include output path");
}

void test_builds_imagemagick_command()
{
    const auto command = conversor::EngineCommandBuilder::build(
        conversor::FileCategory::Image,
        "C:/in/photo.png",
        "C:/out/photo.jpg",
        "jpg");

    expect(command.executableKey == "magick", "image conversion should use ImageMagick");
    expect(command.arguments.size() == 2, "ImageMagick command should pass input and output");
    expect(command.arguments[0] == "C:/in/photo.png", "ImageMagick command should include input path");
    expect(command.arguments[1] == "C:/out/photo.jpg", "ImageMagick command should include output path");
}

void test_builds_document_commands()
{
    const auto pdfCommand = conversor::EngineCommandBuilder::build(
        conversor::FileCategory::Document,
        "C:/in/report.docx",
        "C:/out/report.pdf",
        "pdf");

    const auto markdownCommand = conversor::EngineCommandBuilder::build(
        conversor::FileCategory::Document,
        "C:/in/page.html",
        "C:/out/page.md",
        "md");

    expect(pdfCommand.executableKey == "soffice", "office document conversion should use LibreOffice");
    expect(markdownCommand.executableKey == "pandoc", "markup conversion should use Pandoc");
}

} // namespace

int main()
{
    test_normalizes_extensions();
    test_detects_categories();
    test_groups_equivalent_extensions();
    test_returns_targets_by_category();
    test_exposes_quality_warnings();
    test_analyzes_paths_without_touching_disk();
    test_builds_conversion_plan();
    test_rejects_category_mismatch();
    test_builds_ffmpeg_audio_command();
    test_builds_imagemagick_command();
    test_builds_document_commands();

    if (failures != 0) {
        std::cerr << failures << " test assertion(s) failed\n";
        return EXIT_FAILURE;
    }

    std::cout << "All core tests passed\n";
    return EXIT_SUCCESS;
}

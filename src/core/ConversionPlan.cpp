#include "ConversionPlan.h"

#include <stdexcept>

namespace conversor {

std::vector<PlannedConversion> ConversionPlan::build(const ConversionRequest& request)
{
    if (request.category == FileCategory::Unknown) {
        throw std::invalid_argument("Cannot build a conversion plan for an unknown category.");
    }

    const auto target = FormatRegistry::findTarget(request.category, request.targetExtension);
    if (!target.has_value()) {
        throw std::invalid_argument("Target format is not available for the selected category.");
    }

    std::vector<PlannedConversion> plan;
    plan.reserve(request.inputs.size());

    for (const auto& input : request.inputs) {
        if (input.category != request.category) {
            throw std::invalid_argument("Input file category does not match the selected conversion category.");
        }

        PlannedConversion item;
        item.input = input;
        item.target = *target;
        item.lossy = target->lossyByNature;
        item.outputPath = request.outputDirectory / input.path.stem();
        item.outputPath.replace_extension(target->extension);

        if (!target->warning.empty()) {
            item.warnings.push_back(target->warning);
        }

        if (target->lossyByNature) {
            item.warnings.push_back("The selected target can permanently discard quality.");
        }

        plan.push_back(std::move(item));
    }

    return plan;
}

} // namespace conversor

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace conversor {

enum class ThemePreference {
    System,
    Light,
    Dark,
};

enum class EffectiveTheme {
    Light,
    Dark,
};

EffectiveTheme resolveEffectiveTheme(ThemePreference preference,
                                     std::optional<EffectiveTheme> systemTheme);

std::string themePreferenceKey(ThemePreference preference);
std::optional<ThemePreference> themePreferenceFromKey(std::string_view key);

} // namespace conversor

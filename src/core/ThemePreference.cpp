#include "ThemePreference.h"

namespace conversor {

EffectiveTheme resolveEffectiveTheme(ThemePreference preference,
                                     std::optional<EffectiveTheme> systemTheme)
{
    switch (preference) {
    case ThemePreference::Dark:
        return EffectiveTheme::Dark;
    case ThemePreference::Light:
        return EffectiveTheme::Light;
    case ThemePreference::System:
        return systemTheme.value_or(EffectiveTheme::Light);
    }

    return EffectiveTheme::Light;
}

std::string themePreferenceKey(ThemePreference preference)
{
    switch (preference) {
    case ThemePreference::Dark:
        return "dark";
    case ThemePreference::Light:
        return "light";
    case ThemePreference::System:
        return "system";
    }

    return "system";
}

std::optional<ThemePreference> themePreferenceFromKey(std::string_view key)
{
    if (key == "system") {
        return ThemePreference::System;
    }
    if (key == "light") {
        return ThemePreference::Light;
    }
    if (key == "dark") {
        return ThemePreference::Dark;
    }

    return std::nullopt;
}

} // namespace conversor

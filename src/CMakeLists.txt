add_library(conversor_core
    core/ConversionPlan.cpp
    core/EngineCommand.cpp
    core/FormatRegistry.cpp
    core/ThemePreference.cpp
)

target_include_directories(conversor_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Qt6 QUIET COMPONENTS Widgets)

if(CONVERSOR_BUILD_APP AND Qt6_FOUND)
    qt_standard_project_setup()

    qt_add_executable(conversor_app
        app/main.cpp
        app/MainWindow.cpp
        app/MainWindow.h
    )

    target_link_libraries(conversor_app
        PRIVATE
            Qt6::Widgets
            conversor_core
    )

    set_target_properties(conversor_app PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )
else()
    message(STATUS "Qt 6 Widgets not found; skipping conversor_app.")
endif()

# I used AI to write this build configuration file. CMake is a tool that tells
# the compiler how to combine all the .cpp files into a single executable program.

cmake_minimum_required(VERSION 3.10)
project(IntegrationCalculator CXX)

# Use C++23 standard (a version of the C++ language)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable compiler warnings (catch potential bugs)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# List all the source files that contain the math/integration logic
set(ENGINE_SOURCES
    engine/integration/integrator.cpp
    engine/integration/stats.cpp
    engine/integration/method/right-riemann.cpp
    engine/integration/method/left-riemann.cpp
    engine/integration/method/midpoint-riemann.cpp
    engine/integration/method/trapezoidal.cpp
    engine/integration/method/simpsons.cpp
    engine/expression/parser.cpp
    engine/expression/tree.cpp
    engine/csv/processor.cpp
)

# List all the source files for the menu/user interface
set(CLI_MODULE_SOURCES
    cli/logic/navigation.cpp
    cli/logic/execution.cpp
    cli/input/input-provider.cpp
    cli/content/menu-strings.cpp
    cli/content/integration-strings.cpp
    cli/content/messages.cpp
    cli/utils/console.cpp
)

# Create the executable program from all the .cpp files listed above
add_executable(IntegrationCalculator
    cli-entry.cpp
    ${ENGINE_SOURCES}
    ${CLI_MODULE_SOURCES}
)

# On Linux, link the math library (sin, cos, sqrt, etc.).
# On Windows and macOS, these functions are already available.
if(UNIX AND NOT APPLE)
    target_link_libraries(IntegrationCalculator m)
endif()

# ── wxWidgets GUI target (optional) ──────────────────────────
# Off by default so the CLI builds without wxWidgets. Enable with:
#   cmake -S . -B build -DBUILD_GUI=ON     (or just run ./run-gui)
option(BUILD_GUI "Build the wxWidgets GUI application" OFF)

if(BUILD_GUI)
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    include(${wxWidgets_USE_FILE})

    # GUI sub-module sources (the engine is shared with the CLI target)
    set(GUI_MODULE_SOURCES
        gui/core/application.cpp
        gui/main/window.cpp
        gui/main/window-ui.cpp
        gui/main/menu-bar.cpp
        gui/plotting/view.cpp
        gui/plotting/axes.cpp
        gui/plotting/bars.cpp
        gui/plotting/curve.cpp
        gui/plotting/bounds.cpp
        gui/dialog/history.cpp
        gui/dialog/help.cpp
        gui/content/window-strings.cpp
        gui/ui/widgets.cpp
        gui/auth/user-store.cpp
        gui/dialog/auth.cpp
        gui/dialog/login.cpp
        gui/dialog/register.cpp
        gui/auth/history.cpp
    )

    if(APPLE)
        set(ICON_FILE assets/icon.icns)
        set(ASSET_FILES
            assets/icons/Integral-iOS-Default-1024x1024@1x.png
            assets/icons/Integral-iOS-Dark-1024x1024@1x.png
            assets/icons/Integral-iOS-ClearLight-1024x1024@1x.png
            assets/icons/Integral-iOS-ClearDark-1024x1024@1x.png
            assets/icons/Integral-iOS-TintedLight-1024x1024@1x.png
            assets/icons/Integral-iOS-TintedDark-1024x1024@1x.png
        )
        set_source_files_properties(${ICON_FILE} ${ASSET_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        add_executable(IntegrationCalculatorGUI MACOSX_BUNDLE
            gui-entry.cpp
            ${ENGINE_SOURCES}
            ${GUI_MODULE_SOURCES}
            ${ICON_FILE}
            ${ASSET_FILES}
        )
        set_target_properties(IntegrationCalculatorGUI PROPERTIES
            OUTPUT_NAME "Integration Calculator"
            MACOSX_BUNDLE_ICON_FILE "icon.icns"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.integrationcalculator.gui"
            MACOSX_BUNDLE_BUNDLE_NAME "Integration Calculator"
        )
    else()
        add_executable(IntegrationCalculatorGUI
            gui-entry.cpp
            ${ENGINE_SOURCES}
            ${GUI_MODULE_SOURCES}
        )
    endif()

    target_link_libraries(IntegrationCalculatorGUI ${wxWidgets_LIBRARIES})

    if(UNIX AND NOT APPLE)
        target_link_libraries(IntegrationCalculatorGUI m)
    endif()
endif()

# ── Unit tests (optional) ────────────────────────────────────
# Off by default, so the normal CLI/GUI build is unchanged. The VS Code dev
# environment turns it on via cmake.configureSettings (BUILD_TESTS=ON), so the
# tests appear in the Testing panel (CTest). From the command line:
#   cmake -S . -B build -DBUILD_TESTS=ON && cmake --build build && ctest --test-dir build
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()

    function(add_engine_test target source)
        add_executable(${target} tests/${source} ${ENGINE_SOURCES})
        target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        if(UNIX AND NOT APPLE)
            target_link_libraries(${target} m)
        endif()
    endfunction()

    add_engine_test(parser_tests      parser.cpp)
    add_engine_test(integration_tests integration.cpp)
    add_engine_test(stats_tests       stats.cpp)
    add_engine_test(csv_tests         csv.cpp)

    # Parser
    add_test(NAME parser.parse_constant      COMMAND parser_tests parse_constant)
    add_test(NAME parser.parse_constant_pi   COMMAND parser_tests parse_constant_pi)
    add_test(NAME parser.parse_constant_e    COMMAND parser_tests parse_constant_e)
    add_test(NAME parser.parse_invalid       COMMAND parser_tests parse_invalid)
    add_test(NAME parser.parse_empty         COMMAND parser_tests parse_empty)
    add_test(NAME parser.evaluate_expression COMMAND parser_tests evaluate_expression)
    add_test(NAME parser.evaluate_sin        COMMAND parser_tests evaluate_sin)
    add_test(NAME parser.evaluate_negative   COMMAND parser_tests evaluate_negative)
    add_test(NAME parser.evaluate_unknown_fn COMMAND parser_tests evaluate_unknown_fn)

    # Integration
    add_test(NAME integration.right_riemann COMMAND integration_tests right_riemann)
    add_test(NAME integration.left_riemann  COMMAND integration_tests left_riemann)
    add_test(NAME integration.midpoint      COMMAND integration_tests midpoint)
    add_test(NAME integration.trapezoidal   COMMAND integration_tests trapezoidal)
    add_test(NAME integration.simpsons      COMMAND integration_tests simpsons)
    add_test(NAME integration.linear        COMMAND integration_tests linear)
    add_test(NAME integration.constant      COMMAND integration_tests constant)
    add_test(NAME integration.sin           COMMAND integration_tests sin)
    add_test(NAME integration.degenerate    COMMAND integration_tests degenerate)

    # Stats
    add_test(NAME stats.constant_fn COMMAND stats_tests constant_fn)
    add_test(NAME stats.is_finite   COMMAND stats_tests is_finite)

    # CSV
    add_test(NAME csv.valid_rows     COMMAND csv_tests valid_rows)
    add_test(NAME csv.malformed_row  COMMAND csv_tests malformed_row)
    add_test(NAME csv.empty_input    COMMAND csv_tests empty_input)
    add_test(NAME csv.invalid_bounds COMMAND csv_tests invalid_bounds)
endif()

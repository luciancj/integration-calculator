#include "menu-strings.h"

namespace cli::content {

using namespace cli::utils;

// Box-drawing characters reference:
// URL: https://en.wikipedia.org/wiki/Box-drawing_characters
// clang-format off
const TextBlock welcomeMenu = {
    {
        "╔══════════════════════════════════════╗",
        "║                Welcome               ║",
        "╠══════════════════════════════════════╣",
        "║           Choose an option:          ║",
        "║                                      ║",
        "║   "
        + ANSI_GREEN + "/i - Interactive mode" +
        ANSI_RESET +"              ║",
        "║   "
        + ANSI_GREEN + "/p - Parameter mode" +
        ANSI_RESET + "                ║",
        "║   "
        + ANSI_GREEN + "/e - Exit" +
        ANSI_RESET +
         "                          ║",
        "║                                      ║",
        "╚══════════════════════════════════════╝"}
};

const TextBlock parameterModeBanner = {
    {
        "╔══════════════════════════════════════╗",
        "║        Parameter mode - Choose:      ║",
        "╠══════════════════════════════════════╣",
        "║                                      ║",
        "║   " + ANSI_GREEN + "/rr - The Right Riemann Sum" +
         ANSI_RESET + "        ║",
        "║   " + ANSI_GREEN + "/lr - The Left Riemann Sum" +
         ANSI_RESET + "         ║",
        "║   " + ANSI_GREEN + "/mr - The Midpoint Riemann Sum" +
         ANSI_RESET + "     ║",
        "║   " + ANSI_GREEN + "/tr - Trapezoidal Rule" +
         ANSI_RESET + "             ║",
        "║   " + ANSI_GREEN + "/sr - Simpson's Rule" +
         ANSI_RESET + "               ║",
        "║                                      ║",
        "╚══════════════════════════════════════╝"}
};

const TextBlock interactiveModeBanner = {
    {
        "╔══════════════════════════════════════╗",
        "║      Interactive mode - Choose:      ║",
        "╠══════════════════════════════════════╣",
        "║                                      ║",
        "║   " + ANSI_GREEN + "/rr - The Right Riemann Sum" +
        ANSI_RESET + "        ║",
        "║   " + ANSI_GREEN + "/lr - The Left Riemann Sum" +
        ANSI_RESET + "         ║",
        "║   " + ANSI_GREEN + "/mr - The Midpoint Riemann Sum" +
        ANSI_RESET + "     ║",
        "║   " + ANSI_GREEN + "/tr - Trapezoidal Rule" +
        ANSI_RESET + "             ║",
        "║   " + ANSI_GREEN + "/sr - Simpson's Rule" +
        ANSI_RESET + "               ║",
        "║                                      ║",
        "╚══════════════════════════════════════╝"}
};

const TextBlock askFileNameBanner = {
    {
        "╔══════════════════════════════════════",
        "║ Type in the filename:"
    }
};

// clang-format on

} // namespace cli::content
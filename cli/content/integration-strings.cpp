#include "integration-strings.h"

namespace cli::content {

using namespace cli::utils;

// ASCII art source: TAAG (Text to ASCII Art Generator), font "Tmplr" reference:
// URL: https://patorjk.com/software/taag/
// clang-format off
const TextBlock printIntegrationBanner = {
    {ANSI_GREEN +
        "┳            •      ┏┓  ┓   ┓",
    "┃┏┓╋┏┓┏┓┏┓┏┓╋┓┏┓┏┓  ┃ ┏┓┃┏┓┏┃┏┓╋┏┓┏┓",
    "┻┛┗┗┗ ┗┫┛ ┗┻┗┗┗┛┛┗  ┗┛┗┻┗┗┗┻┗┗┻┗┗┛┛",
    "       ┛"
    + ANSI_RESET}
};

const TextBlock bannerRightRiemannSum = {
    {ANSI_GREEN +
        "┏┳┓┓     ┳┓•  ┓    ┳┓•             ┏┓",
    " ┃ ┣┓┏┓  ┣┫┓┏┓┣┓╋  ┣┫┓┏┓┏┳┓┏┓┏┓┏┓  ┗┓┓┏┏┳┓",
    " ┻ ┛┗┗   ┛┗┗┗┫┛┗┗  ┛┗┗┗ ┛┗┗┗┻┛┗┛┗  ┗┛┗┻┛┗┗",
    "             ┛"
    + ANSI_RESET}
};

const TextBlock bannerLeftRiemannSum = {
    {ANSI_GREEN +
        "┏┳┓┓     ┓   ┏   ┳┓•             ┏┓",
    " ┃ ┣┓┏┓  ┃ ┏┓╋╋  ┣┫┓┏┓┏┳┓┏┓┏┓┏┓  ┗┓┓┏┏┳┓",
    " ┻ ┛┗┗   ┗┛┗ ┛┗  ┛┗┗┗ ┛┗┗┗┻┛┗┛┗  ┗┛┗┻┛┗┗"
    + ANSI_RESET}
};

const TextBlock bannerMidpointRiemannSum = {
    {ANSI_GREEN +
        "┏┳┓┓     ┳┳┓• ┓    •     ┳┓•             ┏┓",
    " ┃ ┣┓┏┓  ┃┃┃┓┏┫┏┓┏┓┓┏┓╋  ┣┫┓┏┓┏┳┓┏┓┏┓┏┓  ┗┓┓┏┏┳┓",
    " ┻ ┛┗┗   ┛ ┗┗┗┻┣┛┗┛┗┛┗┗  ┛┗┗┗ ┛┗┗┗┻┛┗┛┗  ┗┛┗┻┛┗┗"
    + ANSI_RESET}
};

const TextBlock bannerTrapezoidalRule = {
    {ANSI_GREEN +
        "┏┳┓           • ┓  ┓  ┳┓  ┓",
    " ┃ ┏┓┏┓┏┓┏┓┓┏┓┓┏┫┏┓┃  ┣┫┓┏┃┏┓",
    " ┻ ┛ ┗┻┣┛┗ ┗┗┛┗┗┻┗┻┗  ┛┗┗┻┗┗",
    "       ┛"
    + ANSI_RESET}
};

const TextBlock bannerSimpsonsRule = {
    {ANSI_GREEN +
        "┏┓•          ╹   ┳┓  ┓  ",
    "┗┓┓┏┳┓┏┓┏┏┓┏┓ ┏  ┣┫┓┏┃┏┓",
    "┗┛┗┛┗┗┣┛┛┗┛┛┗ ┛  ┛┗┗┻┗┗ ",
    "      ┛                 "
    + ANSI_RESET}
};


const TextBlock printGoodbyeBanner = {
    {ANSI_RESET +
        "╔════════════════════════════════════╗",
    "║              Goodbye!              ║",
    "╚════════════════════════════════════╝"
    + ANSI_RESET}
};

// clang-format on

} // namespace cli::content
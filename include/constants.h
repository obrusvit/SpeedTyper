#ifndef GUARD_SPEED_TYPER_CONSTANTS
#define GUARD_SPEED_TYPER_CONSTANTS

namespace speedtyper {

namespace ASCII_values {

    constexpr int key_backspace = 8;
    constexpr int key_space = 32;
    constexpr int limit = 126;

} // namespace ASCII_values

namespace GUI_options {
    constexpr unsigned int win_sz_X = 1024;
    constexpr unsigned int win_sz_Y = 768;
    constexpr unsigned int FONT_SZ = 30;
    constexpr int FPS_LIMIT = 30;

} // namespace GUI_options

namespace gameopt {
    constexpr unsigned int seconds_limit = 20;

} // namespace gameopt

} // namespace speedtyper

#endif

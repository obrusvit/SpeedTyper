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
    constexpr unsigned int gui_font_sz = 30;  // used for Buttons, InputFields etc.
    constexpr unsigned int ent_font_sz = 30;  // used for WordEntity etc.
    constexpr int FPS_LIMIT = 30;

} // namespace GUI_options

namespace gameopt {
    constexpr int seconds_limit_min = 10;
    constexpr int seconds_limit_max = 1200;
    constexpr int seconds_limit_default = 60;

} // namespace gameopt

} // namespace speedtyper

#endif

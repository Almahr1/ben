#include "color_config.h"

void init_editor_colors(void) {
    // Initialize color pairs using the defined colors
    init_pair(COLOR_PAIR_TEXT, TEXT_FG_COLOR, TEXT_BG_COLOR);
    init_pair(COLOR_PAIR_LINE_NUMBERS, LINE_NUMBER_FG_COLOR, LINE_NUMBER_BG_COLOR);
    init_pair(COLOR_PAIR_STATUS_BAR, STATUS_BAR_FG_COLOR, STATUS_BAR_BG_COLOR);
    init_pair(COLOR_PAIR_COMMAND, COMMAND_INPUT_FG_COLOR, COMMAND_INPUT_BG_COLOR);
    init_pair(COLOR_PAIR_CURSOR_LINE, CURSOR_LINE_FG_COLOR, CURSOR_LINE_BG_COLOR);
}

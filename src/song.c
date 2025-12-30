#include <rp6502.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
#include "screen.h"
#include "song.h"

uint8_t cur_order_idx = 0; // Where we are in the playlist
uint16_t song_length = 1;   // Total number of patterns in the song
bool is_song_mode = false;   // Default to Pattern Mode

void write_order_xram(uint8_t index, uint8_t pattern_id) {
    // 1. Point the RIA to the Order List + the specific slot
    RIA.addr0 = ORDER_LIST_XRAM + index;
    RIA.step0 = 1;
    
    // 2. Write the Pattern ID into that slot
    RIA.rw0 = pattern_id;
}

uint8_t read_order_xram(uint8_t index) {
    // 1. Point the RIA to the Order List + the specific slot
    RIA.addr0 = ORDER_LIST_XRAM + index;
    RIA.step0 = 1;
    
    // 2. Read the Pattern ID from that slot and return it
    return RIA.rw0;
}

void update_order_display() {
    const uint8_t start_x = 23; // Sequence IDs start at column 21
    const uint8_t row_y = 4;    // Sequence line is row 4

    // Show 10 slots of the playlist
    for (uint8_t i = 0; i < 10; i++) {
        uint8_t x = start_x + (i * 3); // Each slot is 2 chars + 1 space
        uint16_t vga_ptr = text_message_addr + (row_y * 80 + x) * 3;
        
        if (i >= song_length) {
            // Draw empty slot dots at the correct X coordinate
            draw_string(x, row_y, ".. ", HUD_COL_DARKGREY, HUD_COL_BG);
        } else {
            uint8_t p_id = read_order_xram(i);
            
            // Highlight logic: 
            // Current editing slot gets Yellow text on Blue/Red background
            uint8_t fg = (i == cur_order_idx) ? HUD_COL_YELLOW : HUD_COL_WHITE;
            
            // Determine background color based on mode
            uint8_t bg;
            if (i == cur_order_idx) {
                bg = edit_mode ? HUD_COL_EDIT_CELL : HUD_COL_PLAY_CELL;
            } else {
                bg = HUD_COL_BG;
            }
            
            // 1. Draw the actual Hex ID
            draw_hex_byte(vga_ptr, p_id);
            
            // 2. Force the colors (including the background highlight)
            set_text_color(x, row_y, 2, fg, bg);
            
            // 3. Draw a separator space after the hex ID
            draw_string(x + 2, row_y, " ", HUD_COL_WHITE, HUD_COL_BG);
        }
    }
}
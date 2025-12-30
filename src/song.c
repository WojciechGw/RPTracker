#include <rp6502.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
#include "screen.h"
#include "song.h"

uint8_t cur_order_idx = 0; // Where we are in the playlist
uint8_t song_length = 1;   // Total number of patterns in the song

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
    // Draw the order list at a specific spot on the dashboard
    uint16_t vga_ptr = text_message_addr + (3 * 80 + 2) * 3; // Row 3
    draw_string(2, 3, "SONG ORDER:", HUD_COL_CYAN, HUD_COL_BG);
    
    // Draw the next 10 pattern IDs from the order list
    for (uint8_t i = 0; i < 10; i++) {
        uint8_t pat_id = read_order_xram(i);
        uint8_t color = (i == cur_order_idx) ? HUD_COL_YELLOW : HUD_COL_WHITE;
        // Logic to draw hex byte at (3, 14 + i*3)
    }
}
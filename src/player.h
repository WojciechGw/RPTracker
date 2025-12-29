#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#define is_shift_down() (key(KEY_LEFTSHIFT) || key(KEY_RIGHTSHIFT))

typedef struct {
    bool is_playing;
    uint8_t ticks_per_row; // Standard tracker speed (default 6)
    uint8_t tick_counter;  // Counter to track sub-row timing
    uint8_t bpm;           // Current BPM (logic uses 60Hz / ticks)
} SequencerState;

extern SequencerState seq;


// Initialize player state
void player_init(void);

// Process keyboard-to-OPL logic (call this once per frame in main loop)
void player_tick(void);

// Global settings
extern uint8_t current_octave;
extern uint8_t current_instrument;
extern uint8_t player_channel;
extern uint8_t current_volume;

extern void handle_navigation(void);
extern void handle_transport_controls(void);
extern void sequencer_step(void);
extern void handle_editing(void);
extern void modify_volume(int8_t delta);
extern void modify_instrument(int8_t delta);
extern void modify_note(int8_t delta);

#endif
# Export Feature Implementation Checklist

## Overview
Add binary export functionality to write OPL2 register data to disk for playback in games.

## Export Format
Each entry: `[reg:u8, val:u8, delay_lo:u8, delay_hi:u8]`
- Total: 4 bytes per register write
- End marker: `0xFF, 0xFF, 0x00, 0x00`
- File size: Must be multiple of 512 bytes

## Configuration Constants
- [x] Define `EXPORT_BUF_XRAM` at `0xF850`
- [x] Define `EXPORT_BUF_MAX` at `0xFE00` 
- [x] Define `EXPORT_CHUNK` as `512` (changed from 1456 for alignment)
- [x] Exposed in constants.h

## Core Implementation

### 1. Global State Variables
- [x] Add `bool is_exporting` flag (in opl.c)
- [x] Add `uint16_t export_idx` (tracks XRAM write position, in opl.c)
- [x] Add `uint16_t accumulated_delay` (for delay calculation, in opl.c)
- [x] Exposed in opl.h
- [x] Add `uint32_t export_total_bytes` (tracks total exported, in player.c)
- [x] Add `char export_filename[16]` (derived from tracker filename, in player.c)
- [x] Add file handle for export (export_fd in player.c)

### 2. OPL_Write() Buffering
- [x] Modify `OPL_Write()` in `opl.c` to check `is_exporting`
- [x] When exporting: buffer to XRAM instead of writing to hardware
  - [x] Write register byte
  - [x] Write value byte
  - [x] Calculate delay since last write (accumulated_delay)
  - [x] Write delay_lo
  - [x] Write delay_hi
  - [x] Increment `export_idx` by 4
  - [x] Reset accumulated_delay to 0 after write
  - [x] Check if buffer would overflow (safety check added)
  - [x] Increment accumulated_delay each frame in export_loop()

### 3. Buffer Management
- [x] Create `flush_export_buffer()` function
  - [x] Call `write_xram()` to write chunk to disk
  - [x] Append to file (using existing file handle)
  - [x] Reset `export_idx` to 0 (not back to XRAM base)
  - [x] Update `export_total_bytes`

### 4. Export Control Function
- [x] Create `start_export()` function
  - [x] Set `is_exporting = true`
  - [x] Derive filename from current tracker name
    - [x] Replace `.RPT` extension with `.BIN`
    - [x] Default to `UNTITLED.BIN` if no name set
  - [x] Initialize export state variables
  - [x] Force `is_song_mode = true`
  - [x] Set `cur_order_idx = 0`
  - [x] Set `play_row = 0`
  - [x] Reset `seq.tick_counter`
  - [x] Clear all effect states
  - [x] Open export file

- [x] Create `export_loop()` function
  - [x] Run `sequencer_step()` repeatedly (bypass vsync)
  - [x] Continue until end of song
  - [x] Detect end: `cur_order_idx >= song_length` after wrap
  - [x] Increment accumulated_delay each frame
  - [x] Check buffer and flush when needed

- [x] Create `finish_export()` function
  - [x] Write end marker: `0xFF, 0xFF, 0x00, 0x00` to buffer
  - [x] Flush remaining buffer data
  - [x] Pad file to multiple of 512 bytes with zeros
  - [x] Set `is_exporting = false`
  - [x] Restore normal playback state
  - [x] Show success message to user

### 5. User Interface Integration
- [x] Add `Ctrl+E` handler in `player_tick()`
  - [x] Check `is_ctrl_down() && key_pressed(KEY_E)`
  - [x] Call `start_export()`
  - [x] Call `export_loop()`
  - [x] Return early (don't process other input)

- [ ] Add export status to dashboard display
  - [ ] Show "EXPORTING..." message
  - [ ] Show progress (bytes written / estimated total)

### 6. Song Mode Integration
- [x] Ensure export starts at song beginning (done in start_export)
- [x] Detect song end properly
  - [x] Track when patterns complete and loop detected
  - [x] End when song loops back to start

### 7. Timing & Delay Calculation
- [x] Track frame/tick count between OPL writes (accumulated_delay)
- [x] Convert to 16-bit delay value
- [x] Increment accumulated_delay each frame in export_loop
- [x] Reset to 0 after each OPL_Write during export

### 8. File Padding
- [x] Calculate bytes needed to reach next 512-byte boundary
- [x] Write zeros to fill remainder
- [x] Verify final file size is `(n * 512)` bytes

## Testing Checklist
- [ ] Test export with simple pattern (single channel, few notes)
- [ ] Test export with full song (multiple patterns)
- [ ] Test export with all 9 channels active
- [ ] Test export with effects active (arp, vibrato, etc.)
- [ ] Verify file size is multiple of 512 bytes
- [ ] Test import in game player
- [ ] Verify delays produce correct timing
- [ ] Test with UNTITLED.RPT default name
- [ ] Test with custom tracker filename
- [ ] Test buffer overflow handling
- [ ] Test very long songs (multiple flushes)

## Edge Cases & Error Handling
- [ ] Handle disk full error during write
- [ ] Handle buffer overflow if many writes per tick
- [ ] Handle filename too long
- [ ] Prevent export during playback
- [ ] Prevent normal operations during export
- [ ] Handle user cancellation (Ctrl+C? ESC?)

## Code Files to Modify
- [x] `/src/player.c` - Add Ctrl+E handler, export functions
- [x] `/src/opl.c` - Modify OPL_Write() for buffering
- [x] `/src/opl.h` - Expose is_exporting flag and export vars
- [x] `/src/constants.h` - Add EXPORT_* defines
- [ ] `/src/screen.c` - Add export status display (optional)

## Implementation Summary

### Completed Features:
1. **Export State Variables** - Added to opl.c and player.c, exposed in opl.h
2. **OPL_Write Buffering** - Modified to write to XRAM buffer when exporting
3. **Buffer Management** - flush_export_buffer() writes 512-byte chunks
4. **Filename Handling** - derive_export_filename() converts .RPT to .BIN
5. **Export Control** - start_export(), export_loop(), finish_export()
6. **User Interface** - Ctrl+E triggers export
7. **Timing** - accumulated_delay tracks ticks between writes
8. **End Marker** - 0xFF 0xFF 00 00 written at end
9. **File Padding** - Pads to 512-byte boundary with zeros

### How It Works:
1. User presses Ctrl+E
2. start_export() initializes export, opens file, resets song to beginning
3. export_loop() runs sequencer without vsync, incrementing accumulated_delay each frame
4. OPL_Write() intercepts register writes and buffers them to XRAM with delay
5. Buffer flushes to disk when reaching 512 bytes
6. Song end detected when loop occurs
7. finish_export() writes end marker, pads to 512, closes file

### Export Format:
```
Repeated entries:
  [register:u8, value:u8, delay_lo:u8, delay_hi:u8]

End marker:
  [0xFF, 0xFF, 0x00, 0x00]

Padding:
  Zero bytes to reach multiple of 512
```

## Documentation
- [ ] Update README.md with export feature usage
- [ ] Document export file format
- [ ] Provide example game player code

## Future Enhancements (Optional)
- [ ] Export single pattern instead of full song
- [ ] Export with loop points
- [ ] Compress export data (RLE for delays?)
- [ ] Export as C array for embedding in source
- [ ] Show time estimate during export
- [ ] Progress bar during export

---

## Notes
- Buffer size `512` allows direct 512-byte sector writes
- XRAM buffer at `0xF850` provides ~430 bytes safe space before OPL area
- Each write = 4 bytes, so ~107 writes per buffer before flush needed
- Song mode ensures proper pattern sequencing
- No vsync = export runs at CPU speed (very fast on RP6502)

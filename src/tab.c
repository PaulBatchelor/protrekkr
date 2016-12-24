#include <stdlib.h>
#include <runt.h>

#include "ptk_data.h"
#include "variables.h"
#include "ptk.h"
#include "tab.h"

extern unsigned char *RawPatterns;

void ptk_tab_write(ptk_data *ptk)
{
    int off;
    int byte;
    int row;
    int track;

    for(track = 0; track < MAX_TRACKS; track++) {
        for(row = 0; row < MAX_ROWS * MAX_PATTERNS; row++) {
            for(byte = 0; byte < PATTERN_INSTR16; byte += 2) {
                off = PATTERN_ROW_LEN * row + PATTERN_BYTES * track; 
                switch(RawPatterns[byte + off]) {
                    case NO_NOTE: break;
                    case NOTE_OFF: 
                        printf("%d %d noteoff\n", track, row);
                        break;
                    default:
                        printf("%d %d %d %d note\n", 
                            track,
                            row, 
                            RawPatterns[byte + off], 
                            RawPatterns[byte + 1 + off] 
                            );
                    break;
                }
            }
        }
    }
}

#include <stdlib.h>

#include "ptk_data.h"
#include "variables.h"
#include "ptk.h"
#include "tab.h"

void ptk_tab_write(ptk_data *ptk)
{
    int off;
    int voice;
    int row;
    int track;
    ptk_tab *tab = &ptk->tab;
    FILE *fp = tab->fp;

    for(track = 0; track < MAX_TRACKS; track++) {
        for(row = 0; row < MAX_ROWS * MAX_PATTERNS; row++) {
            for(voice = 0; voice < PATTERN_INSTR16; voice += 2) {
                off = PATTERN_ROW_LEN * row + PATTERN_BYTES * track; 
                switch(ptk->RawPatterns[voice + off]) {
                    case NO_NOTE: break;
                    case NOTE_OFF: 
                        fprintf(fp, "%d %d %d noteoff\n", track, row, voice);
                        break;
                    default:
                        fprintf(fp, "%d %d %d %d %d note\n", 
                            track,
                            row, 
                            voice,
                            ptk->RawPatterns[voice + off], 
                            ptk->RawPatterns[voice + 1 + off] 
                            );
                    break;
                }
            }
        }
    }
}

static void ptk_define(ptk_data *data, 
        runt_vm *vm, 
        const char *word,
        runt_uint size,
        runt_proc proc,
        runt_ptr p) 
{
    runt_uint word_id;
    word_id = runt_word_define(vm, word, size, proc);
    runt_word_bind_ptr(vm, word_id, p);
}

static int rproc_note(runt_vm *vm, runt_ptr p)
{
    unsigned char args[5];
    runt_int i, rc;
    runt_stacklet *s;
    ptk_data *ptk = runt_to_cptr(p);
    
    for(i = 0; i < 5; i++) {
        rc = runt_ppop(vm, &s);
        RUNT_ERROR_CHECK(rc);
        args[i] = s->f;
    }

    ptk_tab_note(ptk, args[4], args[3], args[2], args[1], args[0]);    

    return RUNT_OK;
}

static int rproc_noteoff(runt_vm *vm, runt_ptr p)
{
    unsigned char args[3];
    runt_int i, rc;
    runt_stacklet *s;
    ptk_data *ptk = runt_to_cptr(p);
    
    for(i = 0; i < 3; i++) {
        rc = runt_ppop(vm, &s);
        RUNT_ERROR_CHECK(rc);
        args[i] = s->f;
    }

    return RUNT_OK;
}


void ptk_tab_init(ptk_data *ptk, ptk_tab *tab)
{
    runt_ptr p = runt_mk_cptr(&tab->vm, ptk);
    tab->mem = malloc(RUNT_MEGABYTE);
    tab->cells = malloc(1024 * sizeof(runt_cell));
    runt_init(&tab->vm);
    runt_cell_pool_set(&tab->vm, tab->cells, 1024);
    runt_cell_pool_init(&tab->vm);
    runt_memory_pool_set(&tab->vm, tab->mem, RUNT_MEGABYTE);
    runt_load_stdlib(&tab->vm);

    ptk_define(ptk, &tab->vm, "note", 4, rproc_note, p);
    ptk_define(ptk, &tab->vm, "noteoff", 7, rproc_noteoff, p);

    runt_set_state(&tab->vm, RUNT_MODE_INTERACTIVE, RUNT_ON);
}

void ptk_tab_clean(ptk_tab *tab)
{
    free(tab->mem);
    free(tab->cells);
}

int ptk_tab_open(ptk_tab *tab, char *filename)
{
    tab->fp = fopen(filename, "w");
    ptk_tab_please_dump(tab);
    return RUNT_OK;
}

int ptk_tab_close(ptk_tab *tab)
{
    if(ptk_tab_dump(tab)) fclose(tab->fp);
    return RUNT_OK;
}

int ptk_tab_please_dump(ptk_tab *tab)
{
    tab->write_data = 1;
}

int ptk_tab_dont_dump(ptk_tab *tab)
{
    tab->write_data = 0;
}

int ptk_tab_dump(ptk_tab *tab)
{
    return tab->write_data;
}

void ptk_tab_note(ptk_data *ptk,
    unsigned char track,
    unsigned char row,
    unsigned char voice,
    unsigned char note,
    unsigned char instr)
{
    int off;
 
    off = PATTERN_ROW_LEN * row + PATTERN_BYTES * track + 2 * voice; 
    ptk->RawPatterns[off] = note;
    ptk->RawPatterns[off + 1] = instr;
    voice += 1;
    if(voice > Channels_MultiNotes[track]) {
        Channels_MultiNotes[track] = voice;
    }
}


void ptk_tab_noteoff(ptk_data *ptk,
    unsigned char track,
    unsigned char row,
    unsigned char voice)
{

}

static void parse(runt_vm *vm, char *str, size_t read)
{
    const char *code = str;
    runt_pmark_set(vm);
    runt_compile(vm, code);
    runt_pmark_free(vm);
}

int ptk_tab_load(ptk_tab *tab, const char *filename)
{
    FILE *fp; 
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    runt_vm *vm = &tab->vm;

    fp = fopen(filename, "r");

    if(fp == NULL) {
        runt_print(vm, "Could not open file %s\n", filename);
        return RUNT_NOT_OK;
    }

    while((read = getline(&line, &len, fp)) != -1) {
        parse(vm, line, read);
    }

    free(line);
    return RUNT_OK;
}

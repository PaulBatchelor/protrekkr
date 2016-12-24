#ifndef _PTK_TAB
#define _PTK_TAB
#ifdef __cplusplus 
extern "C" {
#endif
void ptk_tab_write(ptk_data *ptk);
void ptk_tab_init(ptk_data *ptk, ptk_tab *tab);
void ptk_tab_clean(ptk_tab *tab);
int ptk_tab_open(ptk_tab *tab, char *filename);
int ptk_tab_close(ptk_tab *tab);
int ptk_tab_please_dump(ptk_tab *tab);
int ptk_tab_dont_dump(ptk_tab *tab);
int ptk_tab_dump(ptk_tab *tab);
void ptk_tab_note(ptk_data *ptk,
    unsigned char track,
    unsigned char row,
    unsigned char voice,
    unsigned char note,
    unsigned char instr);

void ptk_tab_noteoff(ptk_data *ptk,
    unsigned char track,
    unsigned char row,
    unsigned char voice);
    
int ptk_tab_load(ptk_tab *tab, const char *filename);
#ifdef __cplusplus 
}
#endif
#endif

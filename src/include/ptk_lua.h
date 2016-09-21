#ifdef __cplusplus 
extern "C" {
#endif
void ptk_lua_init(ptk_data *ptk);
void ptk_lua_call(ptk_data *ptk, int dat, int row);
void ptk_lua_note_call(ptk_data *ptk, int note, int sample, int vol);
#ifdef __cplusplus 
}
#endif

#ifdef __cplusplus 
extern "C" {
#endif
int ptk_sporth_init(ptk_data *ptk, char *str);
void ptk_sporth_destroy(ptk_data *ptk);
int ptk_sporth_set_vars(plumber_data *pd, void *ud);
#ifdef __cplusplus 
}
#endif

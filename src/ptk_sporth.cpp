#include "ptk.h"
#include "ptk_sporth.h"

int ptk_sporth_set_vars(plumber_data *pd, void *ud)
{
	ptk_data *ptk = (ptk_data *) ud;	
    plumber_ftmap_delete(pd, 0);
    plumber_ftmap_add_userdata(pd, "L", &ptk->left_float);
    plumber_ftmap_add_userdata(pd, "R", &ptk->right_float);
    plumber_ftmap_add_userdata(pd, "tick", &ptk->tick);
    plumber_ftmap_add_userdata(pd, "pos", &ptk->linepos);
    plumber_ftmap_delete(pd, 1);
}

int ptk_sporth_init(ptk_data *ptk, char *str)
{
    ptk_sporth *sporth = &ptk->sporth;
    plumber_register(&sporth->pd);
    plumber_init(&sporth->pd);
    sp_create(&sporth->sp);
	sporth_listener *sl = &sporth->sl;

    plumber_data *pd = &sporth->pd;

    plumber_register(&sporth->pd);
    plumber_init(&sporth->pd);
    sporth->pd.sp = sporth->sp;
	ptk_sporth_set_vars(pd, ptk);

	ptk->tick = 0;
	ptk->linepos = 0;

    if(plumber_parse_string(&sporth->pd, str) == PLUMBER_OK) {
        sporth->use_sporth = TRUE;
        plumber_compute(&sporth->pd, PLUMBER_INIT);
		sl->portno = 6449;
		sl->pd = pd;
		sl->start = 1;
		sporth_start_listener(sl); 
        return PLUMBER_OK;
    }

    return PLUMBER_NOTOK;
}

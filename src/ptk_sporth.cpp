#include "replay.h"
#include "ptk.h"
#include "ptk_sporth.h"

int ptk_sporth_set_vars(plumber_data *pd, void *ud)
{
	ptk_data *ptk = (ptk_data *) ud;	
    ptk_sporth *sporth = &ptk->sporth;
    plumber_ftmap_delete(pd, 0);
    plumber_ftmap_add_userdata(pd, "L", &ptk->left_float);
    plumber_ftmap_add_userdata(pd, "R", &ptk->right_float);
    plumber_ftmap_add_userdata(pd, "tick", &sporth->tick);
    plumber_ftmap_add_userdata(pd, "pos", &sporth->linepos);
    plumber_ftmap_add(pd, "notes", sporth->notes);
    plumber_ftmap_add(pd, "gates", sporth->gates);
    plumber_ftmap_delete(pd, 1);
}

int ptk_sporth_init(ptk_data *ptk, char *str)
{
    ptk_sporth *sporth = &ptk->sporth;
    plumber_register(&sporth->pd);
    plumber_init(&sporth->pd);
    sp_create(&sporth->sp);

    if(ptk->render_mode == TRUE) {
        sporth->sp->len = Calc_Length(ptk);
        sp_progress_create(&sporth->prog); 
        sp_progress_init(sporth->sp, sporth->prog); 
        printf("the length is now %d\n", sporth->prog->len);
    }
	sporth_listener *sl = &sporth->sl;

    plumber_data *pd = &sporth->pd;

    plumber_register(&sporth->pd);
    plumber_init(&sporth->pd);
    sporth->pd.sp = sporth->sp;

	sp_ftbl_create(pd->sp, &sporth->notes, 16);
	sp_ftbl_create(pd->sp, &sporth->gates, 16);

	ptk_sporth_set_vars(pd, ptk);

	sporth->tick = 0;
	sporth->linepos = 0;

    if(plumber_parse_string(&sporth->pd, str) == PLUMBER_OK) {
        sporth->use_sporth = TRUE;
        plumber_compute(&sporth->pd, PLUMBER_INIT);
		sl->portno = 6449;
		sl->pd = pd;
		sl->start = 1;
		if(ptk->start_gui == TRUE) {
			sporth_start_listener(sl); 
		}
        return PLUMBER_OK;
    }

    return PLUMBER_NOTOK;
}

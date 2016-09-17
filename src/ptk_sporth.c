#include "ptk_data.h"
#include "variables.h"
#include "ptk.h"
#include "ptk_sporth.h"
#include "files.h"

int ptk_sporth_set_vars(plumber_data *pd, void *ud)
{
	ptk_data *ptk = (ptk_data *) ud;	
    ptk_sporth *sporth = &ptk->sporth;
    plumber_ftmap_delete(pd, 0);
    plumber_ftmap_add_userdata(pd, "L", &ptk->left_float);
    plumber_ftmap_add_userdata(pd, "R", &ptk->right_float);
    plumber_ftmap_add_userdata(pd, "tick", &sporth->tick);
    plumber_ftmap_add_userdata(pd, "pos", &sporth->linepos);
    plumber_ftmap_add_userdata(pd, "play", &sporth->play);
    plumber_ftmap_add(pd, "notes", sporth->notes);
    plumber_ftmap_add(pd, "gates", sporth->gates);
    plumber_ftmap_add(pd, "tracks", sporth->tracks);
    plumber_ftmap_delete(pd, 1);
}

int ptk_sporth_init(ptk_data *ptk, char *str)
{
    ptk_sporth *sporth = &ptk->sporth;
    sp_create(&sporth->sp);
    plumber_register(&sporth->pd);
    plumber_init(&sporth->pd);

    if(ptk->render_mode == TRUE) {
        sporth->sp->len = Calc_Length(ptk);
        sp_progress_create(&sporth->prog); 
        sp_progress_init(sporth->sp, sporth->prog); 
        printf("the length is now %d\n", sporth->prog->len);
    }
	sporth_listener *sl = &sporth->sl;

    plumber_data *pd = &sporth->pd;

    sporth->pd.sp = sporth->sp;

	sp_ftbl_create(pd->sp, &sporth->notes, 16);
	sp_ftbl_create(pd->sp, &sporth->gates, 16);
	sp_ftbl_create(pd->sp, &sporth->tracks, MAX_TRACKS);

	ptk_sporth_set_vars(pd, ptk);

	sporth->tick = 0;
	sporth->linepos = 0;
	sporth->play = 0;

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

void ptk_sporth_destroy(ptk_data *ptk)
{
    if(ptk->start_gui == TRUE) {
        ptk->sporth.sl.start = 0;
        sleep(1);
    }
    if(ptk->render_mode == TRUE) {
        sp_progress_destroy(&ptk->sporth.prog);
    }
    ptk_sporth *sporth = &ptk->sporth;
    plumber_clean(&sporth->pd);
    sp_ftbl_destroy(&sporth->notes);
    sp_ftbl_destroy(&sporth->gates);
    sp_ftbl_destroy(&sporth->tracks);
    sp_destroy(&sporth->sp);
}

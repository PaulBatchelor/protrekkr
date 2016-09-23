#include "ptk_data.h"
#include "variables.h"
#include "ptk.h"
#include "ptk_lua.h"
#include "ptk_sporth.h"

static ptk_data * get_ptk_data(lua_State *L)
{
    lua_getglobal(L, "ptk");
    ptk_data *ptk = (ptk_data *)lua_touserdata(L, -1);
    return ptk;
}

static int foo(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    fprintf(stderr, "this is something... %d\n", ptk->Current_Instrument);
    return 0;
}

static int compile_sporth(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    const char *str = lua_tostring(L, 1);
    ptk_sporth_init(ptk, (char *)str);
    return 0;
}

static int set_callback(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    lua_settop(L, 1);
    ptk->lref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

static int set_recompile_callback(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    lua_settop(L, 1);
    ptk->recompile_cb = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

static int set_play_callback(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    lua_settop(L, 1);
    ptk->play_cb = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

static int set_note_callback(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    lua_settop(L, 1);
    ptk->note_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

static int get_sporth_var(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    const char *varstr = lua_tostring(L, 1);
    plumber_data *pd = &ptk->sporth.pd;
    void *ud;

    if(plumber_ftmap_search_userdata(pd, 
                (char *)varstr, &ud) != PLUMBER_OK) {
        printf("Could not find variable %s\n", varstr);
        lua_pushnil(L);
    } else {
        lua_pushlightuserdata(L, ud);
    }
    return 1;
}

static int l_get_pointer(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    const char *str = lua_tostring(L, 1);
    plumber_data *pd = &ptk->sporth.pd;
    plumber_ptr *ptr = lua_newuserdata(L, sizeof(plumber_ptr));
    
    if(plumber_get_userdata(pd, str, &ptr) == PLUMBER_NOTOK) {
        fprintf(stderr, "There was a problem getting table %s\n", str);
        lua_pushnil(L);
    } else {
        lua_pushlightuserdata(L, ptr);
    }

    return 1;
}

static int l_ps_eval(lua_State *L)
{
    plumber_ptr *ptr = lua_touserdata(L, 1);
    const char *str = lua_tostring(L, 2);
    polysporth_eval(ptr, str);
    return 0;
}

static int toggle_note_callback(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    int sample = lua_tointeger(L, 1);
    ptk->note_cb ^= 1 << sample;
    return 1;
}

static int set_sporth_var(lua_State *L)
{
    ptk_data *ptk = get_ptk_data(L);
    SPFLOAT *x = (SPFLOAT *)lua_touserdata(L, 1);
    SPFLOAT val = lua_tonumber(L, 2);
    *x = val;
    return 0;
}


void ptk_lua_init(ptk_data *ptk)
{
    lua_State *L = ptk->L;
    luaL_openlibs(L);
    lua_pushlightuserdata(L, ptk);
    lua_setglobal(L, "ptk");
    lua_register(L, "foo", foo);
    lua_register(L, "ptk_compile_sporth", compile_sporth);
    lua_register(L, "ptk_set_effect_callback", set_callback);
    lua_register(L, "ptk_set_note_callback", set_note_callback);
    lua_register(L, "ptk_set_recompile_callback", set_recompile_callback);
    lua_register(L, "ptk_set_play_callback", set_play_callback);
    lua_register(L, "ptk_toggle_note_callback", toggle_note_callback);
    lua_register(L, "ptk_var_get", get_sporth_var);
    lua_register(L, "ptk_var_set", set_sporth_var);
    lua_register(L, "ptk_get_pointer", l_get_pointer);
    lua_register(L, "ptk_ps_eval", l_ps_eval);
    if(luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
        fprintf(stderr, "cannot run file %s\n", lua_tostring(L, -1));
    /* lua lock needed for thread-safety */
    ptk->lua_lock = 0;

}

void ptk_lua_call(ptk_data *ptk, int dat, int row)
{
    if(ptk->lref != -1) {
        lua_rawgeti(ptk->L, LUA_REGISTRYINDEX, ptk->lref);
        lua_pushinteger(ptk->L, dat);
        lua_pushinteger(ptk->L, row);
        lua_pcall(ptk->L, 2, 0, 0);
    } else {
        printf("row callback not set!\n");
    }
}

void ptk_lua_call_noargs(ptk_data *ptk, int ref)
{
    int lock = ptk->lua_lock;
    if(lock) {
        printf("Couldn't call function at reference %d: thread is locked!\n",
                ref);
        return;
    }
    if(ref != -1) {
        ptk->lua_lock = 1;
        lua_rawgeti(ptk->L, LUA_REGISTRYINDEX, ref);
        lua_pcall(ptk->L, 0, 0, 0);
        ptk->lua_lock = 0;
    } 
}

void ptk_lua_note_call(ptk_data *ptk, int note, int sample, int vol)
{
    if(ptk->note_cb_ref != -1) {
        lua_rawgeti(ptk->L, LUA_REGISTRYINDEX, ptk->note_cb_ref);
        lua_pushinteger(ptk->L, note);
        lua_pushinteger(ptk->L, sample);
        lua_pushinteger(ptk->L, vol);
        lua_pcall(ptk->L, 3, 0, 0);
    }
}


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

void ptk_lua_init(ptk_data *ptk)
{
    lua_State *L = ptk->L;
    luaL_openlibs(L);
    lua_pushlightuserdata(L, ptk);
    lua_setglobal(L, "ptk");
    lua_register(L, "foo", foo);
    lua_register(L, "ptk_compile_sporth", compile_sporth);
    lua_register(L, "ptk_set_effect_callback", set_callback);
    if(luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
        fprintf(stderr, "cannot run file %s\n", lua_tostring(L, -1));
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

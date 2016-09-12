#include "ptk.h"
#include "ptk_lua.h"

static int foo(lua_State *L)
{
    fprintf(stderr, "this is something...\n");
    return 0;
}

void ptk_lua_init(ptk_data *ptk)
{
    lua_State *L = ptk->L;
    luaL_openlibs(L);
    lua_pushlightuserdata(L, ptk);
    lua_setglobal(L, "ptk");
    lua_register(L, "foo", foo);
    if(luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
        fprintf(stderr, "cannot run file %s\n", lua_tostring(L, -1));
}

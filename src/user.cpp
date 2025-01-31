#include "user.hpp"

// int GetPlayerSteamLevel();
EXTERN int luasteam_getPlayerSteamLevel(lua_State *L) {
    lua_pushnumber(L, SteamUser()->GetPlayerSteamLevel());
    return 1;
}

// CSteamID GetSteamID();
EXTERN int luasteam_getSteamID(lua_State *L) {
    luasteam::pushuint64(L, SteamUser()->GetSteamID().ConvertToUint64());
    return 1;
}


EXTERN int luasteam_loggedOn(lua_State *L){
    lua_pushboolean(L, SteamUser()->BLoggedOn());
    return 1;
}


namespace luasteam {

void add_user(lua_State *L) {
    lua_createtable(L, 0, 3);
    add_func(L, "getPlayerSteamLevel", luasteam_getPlayerSteamLevel);
    add_func(L, "getSteamID", luasteam_getSteamID);
    add_func(L, "loggedOn", luasteam_loggedOn);
    lua_setfield(L, -2, "user");
}

void init_user(lua_State *L) {}

void shutdown_user(lua_State *L) {}

} // namespace luasteam

#include "friends.hpp"

// ============================
// ======= SteamFriends =======
// ============================

namespace {

class CallbackListener;
CallbackListener *friends_listener = nullptr;
int friends_ref = LUA_NOREF;

const char *dialog_types[] = {"friends", "community", "players", "settings", "officialgamegroup", "stats", "achievements", NULL};

class CallbackListener {
  private:
    STEAM_CALLBACK(CallbackListener, OnGameOverlayActivated, GameOverlayActivated_t);
};

void CallbackListener::OnGameOverlayActivated(GameOverlayActivated_t *data) {
    if (data == nullptr) {
        return;
    }
    lua_State *L = luasteam::global_lua_state;
    if (!lua_checkstack(L, 4)) {
        return;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, friends_ref);
    lua_getfield(L, -1, "onGameOverlayActivated");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
    } else {
        lua_createtable(L, 0, 1);
        lua_pushboolean(L, data->m_bActive);
        lua_setfield(L, -2, "active");
        //lua_call(L, 1, 0);
        if(lua_pcall(L, 1, 0, 0)) 
            lua_pop(L, 1);
            
        lua_pop(L, 1);
    }
}

} // namespace

// void ActivateGameOverlay( const char *pchDialog );
EXTERN int luasteam_activateGameOverlay(lua_State *L) {
    const char *dialog = luaL_checkstring(L, 1);
    SteamFriends()->ActivateGameOverlay(dialog);
    return 0;
}

// void ActivateGameOverlayToWebPage( const char *pchURL );
EXTERN int luasteam_activateGameOverlayToWebPage(lua_State *L) {
    const char *url = luaL_checkstring(L, 1);
    SteamFriends()->ActivateGameOverlayToWebPage(url);
    return 0;
}

// const char * GetFriendPersonaName( CSteamID steamIDFriend );
EXTERN int luasteam_getFriendPersonaName(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    const char *name = SteamFriends()->GetFriendPersonaName(id);
    lua_pushstring(L, name);
    return 1;
}

EXTERN int luasteam_getPersonaName(lua_State *L) {
    lua_pushstring(L, SteamFriends()->GetPersonaName());
    return 1;
}

EXTERN int luasteam_getPersonaState(lua_State *L) {
    lua_pushnumber(L, SteamFriends()->GetPersonaState());
    return 1;
}

EXTERN int luasteam_getSmallFriendAvatar(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    int handle = SteamFriends()->GetSmallFriendAvatar(id);
    if (handle == 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, handle);
    }
    return 1;
}

EXTERN int luasteam_getMediumFriendAvatar(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    int size = luaL_checkint(L, 2);
    int handle = SteamFriends()->GetMediumFriendAvatar(id);
    if (handle == 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, handle);
    }
    return 1;
}

EXTERN int luasteam_getLargeFriendAvatar(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    int handle = SteamFriends()->GetLargeFriendAvatar(id);
    if (handle == 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, handle);
    }
    return 1;
}

EXTERN int luasteam_requestUserInformation(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    bool name_only = lua_toboolean(L, 2);
    bool success = SteamFriends()->RequestUserInformation(id, name_only);
    lua_pushboolean(L, success);
    return 1;
}

// bool SetRichPresence( const char *pchKey, const char *pchValue );
EXTERN int luasteam_setRichPresence(lua_State *L) {
    const char *key = luaL_checkstring(L, 1);
    const char *value = luaL_checkstring(L, 2);
    bool success = SteamFriends()->SetRichPresence(key, value);
    lua_pushboolean(L, success);
    return 1;
}


EXTERN int luasteam_getFriendCount(lua_State *L) {
    const int flags = luaL_checkint(L, 1);
    const int count = SteamFriends()->GetFriendCount(flags);
    lua_pushnumber(L, count);
    return 1;
}

EXTERN int luasteam_getFriendByIndex(lua_State *L) {
    const int index = luaL_checkint(L, 1);
    const int flags = luaL_checkint(L, 2);

    //luasteam::pushuint64(L, SteamFriends()->GetFriendByIndex(index, flags).ConvertToUint64());
    // check if k_steamIDNil
    CSteamID id = SteamFriends()->GetFriendByIndex(index, flags);
    if (id == k_steamIDNil) {
        lua_pushnil(L);
    } else {
        luasteam::pushuint64(L, id.ConvertToUint64());
    }
    return 1;
}

EXTERN int luasteam_getFriendPersonaState(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    EPersonaState state = SteamFriends()->GetFriendPersonaState(id);
    lua_pushnumber(L, state);
    return 1;
}

EXTERN int luasteam_getFriendGamePlayed(lua_State *L) {
    FriendGameInfo_t info;
    bool success = SteamFriends()->GetFriendGamePlayed(luasteam::checkuint64(L, 1), &info);
    if (success) {
        lua_createtable(L, 0, 2);
        lua_pushnumber(L, info.m_gameID.AppID());
        lua_setfield(L, -2, "gameID");
        luasteam::pushuint64(L, info.m_steamIDLobby.ConvertToUint64());
        lua_setfield(L, -2, "lobbyID");
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

EXTERN int luasteam_activateGameOverlayInviteDialog(lua_State *L) {
    CSteamID id(luasteam::checkuint64(L, 1));
    SteamFriends()->ActivateGameOverlayInviteDialog(id);
    return 0;
}


namespace luasteam {

void add_friends(lua_State *L) {
    lua_createtable(L, 0, 15);
    add_func(L, "activateGameOverlay", luasteam_activateGameOverlay);
    add_func(L, "activateGameOverlayToWebPage", luasteam_activateGameOverlayToWebPage);
    add_func(L, "getFriendPersonaName", luasteam_getFriendPersonaName);
    add_func(L, "getPersonaName", luasteam_getPersonaName);
    add_func(L, "getPersonaState", luasteam_getPersonaState);
    add_func(L, "setRichPresence", luasteam_setRichPresence);
    add_func(L, "getFriendCount", luasteam_getFriendCount);
    add_func(L, "getFriendByIndex", luasteam_getFriendByIndex);
    add_func(L, "getFriendPersonaState", luasteam_getFriendPersonaState);
    add_func(L, "getFriendGamePlayed", luasteam_getFriendGamePlayed);
    add_func(L, "getMediumFriendAvatar", luasteam_getMediumFriendAvatar);
    add_func(L, "getLargeFriendAvatar", luasteam_getLargeFriendAvatar);
    add_func(L, "requestUserInformation", luasteam_requestUserInformation);
    add_func(L, "getSmallFriendAvatar", luasteam_getSmallFriendAvatar);
    add_func(L, "activateGameOverlayInviteDialog", luasteam_activateGameOverlayInviteDialog);
    lua_pushvalue(L, -1);
    friends_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_setfield(L, -2, "friends");
}

void init_friends(lua_State *L) { friends_listener = new CallbackListener(); }

void shutdown_friends(lua_State *L) {
    luaL_unref(L, LUA_REGISTRYINDEX, friends_ref);
    friends_ref = LUA_NOREF;
    delete friends_listener;
    friends_listener = nullptr;
}

} // namespace luasteam

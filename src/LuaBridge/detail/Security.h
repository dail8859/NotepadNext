#pragma once

namespace luabridge {

//------------------------------------------------------------------------------
/**
security options.
*/
class Security
{
public:
  static bool hideMetatables()
  {
    return getSettings().hideMetatables;
  }

  static void setHideMetatables(bool shouldHide)
  {
    getSettings().hideMetatables = shouldHide;
  }

private:
  struct Settings
  {
    Settings() : hideMetatables(true)
    {
    }

    bool hideMetatables;
  };

  static Settings& getSettings()
  {
    static Settings settings;
    return settings;
  }
};

//------------------------------------------------------------------------------
/**
Set a global value in the lua_State.

@note This works on any type specialized by `Stack`, including `LuaRef` and
its table proxies.
*/
template <class T>
inline void setGlobal(lua_State* L, T t, char const* name)
{
  push(L, t);
  lua_setglobal(L, name);
}

//------------------------------------------------------------------------------
/**
Change whether or not metatables are hidden (on by default).
*/
inline void setHideMetatables(bool shouldHide)
{
  Security::setHideMetatables(shouldHide);
}

} // namespace luabridge

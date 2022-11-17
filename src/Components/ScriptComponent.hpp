#pragma once

#include <utility>
#include "sol/sol.hpp"

struct ScriptComponent {
    sol::function func;

    explicit ScriptComponent(sol::function func = sol::lua_nil) :
            func(std::move(func)) {}
};

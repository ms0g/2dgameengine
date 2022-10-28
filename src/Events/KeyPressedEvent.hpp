#pragma once

#include <SDL2/SDL.h>
#include "../EventBus/Event.hpp"

class KeyPressedEvent : public Event {
public:
    explicit KeyPressedEvent(SDL_Keycode symbol):symbol(symbol) {}
    SDL_Keycode symbol;
};

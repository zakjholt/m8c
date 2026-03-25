// Copyright 2021 Jonne Kokkonen
// Released under the MIT licence, https://opensource.org/licenses/MIT

#ifndef EVENTS_H_
#define EVENTS_H_

#include "m8c_sdl.h"

m8c_app_result m8c_handle_event(void *appstate, SDL_Event *event);

#endif

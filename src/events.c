#include "events.h"
#include "backends/m8.h"
#include "common.h"
#include "gamepads.h"
#include "input.h"
#include "m8c_sdl.h"
#include "render.h"
#include "settings.h"

m8c_app_result m8c_handle_event(void *appstate, SDL_Event *event) {
  struct app_context *ctx = appstate;
  m8c_app_result ret_val = M8C_APP_CONTINUE;

  switch (event->type) {

  // --- System events ---
  case SDL_EVENT_QUIT:
#ifndef M8C_USE_SDL2
  case SDL_EVENT_TERMINATING:
#endif
    ret_val = M8C_APP_SUCCESS;
    break;

#ifdef M8C_USE_SDL2
  case SDL_WINDOWEVENT:
    if (event->window.event == SDL_WINDOWEVENT_RESIZED ||
        event->window.event == SDL_WINDOWEVENT_MOVED) {
      renderer_fix_texture_scaling_after_window_resize(&ctx->conf);
    }
    break;
#else
  case SDL_EVENT_WINDOW_RESIZED:
  case SDL_EVENT_WINDOW_MOVED:
    // If the window size is changed, some systems might need a little nudge to fix scaling
    renderer_fix_texture_scaling_after_window_resize(&ctx->conf);
    break;
#endif

#ifndef M8C_USE_SDL2
  // --- iOS specific events ---
  case SDL_EVENT_DID_ENTER_BACKGROUND:
    // iOS: Application entered into the background on iOS. About 5 seconds to stop things.
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "Received SDL_EVENT_DID_ENTER_BACKGROUND");
    ctx->app_suspended = 1;
    if (ctx->device_connected)
      m8_pause_processing();
    break;
  case SDL_EVENT_WILL_ENTER_BACKGROUND:
    // iOS: App about to enter into the background
    break;
  case SDL_EVENT_WILL_ENTER_FOREGROUND:
    // iOS: App returning to the foreground
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "Received SDL_EVENT_WILL_ENTER_FOREGROUND");
    break;
  case SDL_EVENT_DID_ENTER_FOREGROUND:
    // iOS: App becomes interactive again
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "Received SDL_EVENT_DID_ENTER_FOREGROUND");
    ctx->app_suspended = 0;
    if (ctx->device_connected) {
      renderer_clear_screen();
      m8_resume_processing();
    }
    break;
#endif

  // --- Input events ---
  case SDL_EVENT_GAMEPAD_ADDED:
  case SDL_EVENT_GAMEPAD_REMOVED:
    // Reinitialize game controllers on controller add/remove/remap
    gamepads_initialize();
    break;

  case SDL_EVENT_KEY_DOWN:
    // Settings view toggles handled here to avoid being able to get stuck in the config view
    // Toggle settings with Command/Win+comma (for keyboards without function keys)
    if (M8C_EVENT_KEY_SYM(event) == SDLK_COMMA && M8C_EVENT_KEY_REPEAT(event) == 0 &&
        (M8C_EVENT_KEY_MOD(event) & SDL_KMOD_GUI)) {
      settings_toggle_open();
      return ret_val;
    }
    // Toggle settings with config defined key
    if (M8C_EVENT_KEY_SCANCODE(event) == ctx->conf.key_toggle_settings &&
        M8C_EVENT_KEY_REPEAT(event) == 0) {
      settings_toggle_open();
      return ret_val;
    }
    // Route to settings if open
    if (settings_is_open()) {
      settings_handle_event(ctx, event);
      return ret_val;
    }
    input_handle_key_down_event(ctx, event);
    break;

  case SDL_EVENT_KEY_UP:
    if (settings_is_open()) {
      settings_handle_event(ctx, event);
      return ret_val;
    }
    input_handle_key_up_event(ctx, event);
    break;

  case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    if (settings_is_open()) {
      settings_handle_event(ctx, event);
      return ret_val;
    }

    // Allow toggling the settings view using a gamepad only when the device is disconnected to
    // avoid accidentally opening the screen while using the device
    if (M8C_EVENT_GP_BTN(event) == SDL_GAMEPAD_BUTTON_BACK) {
      if (ctx->app_state == WAIT_FOR_DEVICE && !settings_is_open()) {
        settings_toggle_open();
      }
    }

    input_handle_gamepad_button(ctx, M8C_EVENT_GP_BTN(event), true);
    break;

  case SDL_EVENT_GAMEPAD_BUTTON_UP:
    if (settings_is_open()) {
      settings_handle_event(ctx, event);
      return ret_val;
    }
    input_handle_gamepad_button(ctx, M8C_EVENT_GP_BTN(event), false);
    break;

  case SDL_EVENT_GAMEPAD_AXIS_MOTION:
    if (settings_is_open()) {
      settings_handle_event(ctx, event);
      return ret_val;
    }
    input_handle_gamepad_axis(ctx, M8C_EVENT_GP_AXIS(event), M8C_EVENT_GP_AXIS_VAL(event));
    break;

  default:
    break;
  }
  return ret_val;
}

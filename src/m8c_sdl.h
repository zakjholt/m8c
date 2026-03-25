/* Unified SDL2/SDL3 for m8c. TrimUI Brick / MinUI: build with -DM8C_USE_SDL2. */
#ifndef M8C_SDL_H
#define M8C_SDL_H

#ifdef M8C_USE_SDL2

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_mutex.h>

typedef SDL_mutex SDL_Mutex;
typedef SDL_cond SDL_Condition;
#define SDL_CreateCondition() SDL_CreateCond()
#define SDL_DestroyCondition SDL_DestroyCond
#define SDL_SignalCondition SDL_CondSignal

typedef int m8c_app_result;
#define M8C_APP_CONTINUE 0
#define M8C_APP_SUCCESS 1
#define M8C_APP_FAILURE -1

typedef SDL_GameController SDL_Gamepad;
typedef SDL_GameControllerButton SDL_GamepadButton;
typedef SDL_GameControllerAxis SDL_GamepadAxis;

#ifndef SDL_GAMEPAD_AXIS_COUNT
#define SDL_GAMEPAD_AXIS_COUNT (SDL_CONTROLLER_AXIS_MAX + 1)
#endif

#define SDL_GAMEPAD_AXIS_INVALID SDL_CONTROLLER_AXIS_INVALID
#define SDL_GAMEPAD_AXIS_LEFTX SDL_CONTROLLER_AXIS_LEFTX
#define SDL_GAMEPAD_AXIS_LEFTY SDL_CONTROLLER_AXIS_LEFTY
#define SDL_GAMEPAD_AXIS_RIGHT_TRIGGER SDL_CONTROLLER_AXIS_TRIGGERRIGHT
#define SDL_GAMEPAD_AXIS_LEFT_TRIGGER SDL_CONTROLLER_AXIS_TRIGGERLEFT

#define SDL_GAMEPAD_BUTTON_SOUTH SDL_CONTROLLER_BUTTON_A
#define SDL_GAMEPAD_BUTTON_EAST SDL_CONTROLLER_BUTTON_B
#define SDL_GAMEPAD_BUTTON_WEST SDL_CONTROLLER_BUTTON_X
#define SDL_GAMEPAD_BUTTON_NORTH SDL_CONTROLLER_BUTTON_Y
#define SDL_GAMEPAD_BUTTON_BACK SDL_CONTROLLER_BUTTON_BACK
#define SDL_GAMEPAD_BUTTON_GUIDE SDL_CONTROLLER_BUTTON_GUIDE
#define SDL_GAMEPAD_BUTTON_START SDL_CONTROLLER_BUTTON_START
#define SDL_GAMEPAD_BUTTON_LEFT_STICK SDL_CONTROLLER_BUTTON_LEFTSTICK
#define SDL_GAMEPAD_BUTTON_RIGHT_STICK SDL_CONTROLLER_BUTTON_RIGHTSTICK
#define SDL_GAMEPAD_BUTTON_LEFT_SHOULDER SDL_CONTROLLER_BUTTON_LEFTSHOULDER
#define SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
#define SDL_GAMEPAD_BUTTON_DPAD_UP SDL_CONTROLLER_BUTTON_DPAD_UP
#define SDL_GAMEPAD_BUTTON_DPAD_DOWN SDL_CONTROLLER_BUTTON_DPAD_DOWN
#define SDL_GAMEPAD_BUTTON_DPAD_LEFT SDL_CONTROLLER_BUTTON_DPAD_LEFT
#define SDL_GAMEPAD_BUTTON_DPAD_RIGHT SDL_CONTROLLER_BUTTON_DPAD_RIGHT

#define SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN SDL_CONTROLLER_BUTTON_LABEL_UNKNOWN

#define SDL_KMOD_GUI KMOD_GUI

#define M8C_EVENT_KEY_TYPE(ev) ((ev)->type)

#define M8C_ShowCursor() SDL_ShowCursor(SDL_ENABLE)
#define M8C_HideCursor() SDL_ShowCursor(SDL_DISABLE)

#define SDL_HINT_IOS_HIDE_HOME_INDICATOR "SDL_IOS_HIDE_HOME_INDICATOR"

#define SDL_EVENT_QUIT SDL_QUIT
#define SDL_EVENT_TERMINATING SDL_APP_TERMINATING
#define SDL_EVENT_WINDOWEVENT SDL_WINDOWEVENT
#define SDL_EVENT_DID_ENTER_BACKGROUND SDL_APP_DIDENTERBACKGROUND
#define SDL_EVENT_WILL_ENTER_BACKGROUND SDL_APP_WILLENTERBACKGROUND
#define SDL_EVENT_WILL_ENTER_FOREGROUND SDL_APP_WILLENTERFOREGROUND
#define SDL_EVENT_DID_ENTER_FOREGROUND SDL_APP_DIDENTERFOREGROUND
#define SDL_EVENT_GAMEPAD_ADDED SDL_CONTROLLERDEVICEADDED
#define SDL_EVENT_GAMEPAD_REMOVED SDL_CONTROLLERDEVICEREMOVED
#define SDL_EVENT_KEY_DOWN SDL_KEYDOWN
#define SDL_EVENT_KEY_UP SDL_KEYUP
#define SDL_EVENT_GAMEPAD_BUTTON_DOWN SDL_CONTROLLERBUTTONDOWN
#define SDL_EVENT_GAMEPAD_BUTTON_UP SDL_CONTROLLERBUTTONUP
#define SDL_EVENT_GAMEPAD_AXIS_MOTION SDL_CONTROLLERAXISMOTION

#define SDL_EVENT_KEY_UP SDL_KEYUP

#define SDL_LOGICAL_PRESENTATION_INTEGER_SCALE 1
#define SDL_LOGICAL_PRESENTATION_DISABLED 0

#define SDL_SCALEMODE_NEAREST SDL_ScaleModeNearest
#define SDL_SCALEMODE_LINEAR SDL_ScaleModeLinear

#define SDL_WINDOW_HIGH_PIXEL_DENSITY SDL_WINDOW_ALLOW_HIGHDPI

#define SDL_ALPHA_OPAQUE 255

#define M8C_EVENT_KEY_SYM(ev) ((ev)->key.keysym.sym)
#define M8C_EVENT_KEY_SCANCODE(ev) ((ev)->key.keysym.scancode)
#define M8C_EVENT_KEY_MOD(ev) ((ev)->key.keysym.mod)
#define M8C_EVENT_KEY_REPEAT(ev) ((ev)->key.repeat)
#define M8C_EVENT_GP_BTN(ev) ((ev)->cbutton.button)
#define M8C_EVENT_GP_AXIS(ev) ((ev)->caxis.axis)
#define M8C_EVENT_GP_AXIS_VAL(ev) ((ev)->caxis.value)

#define M8C_RenderClear(r) (SDL_RenderClear(r) == 0)
#define M8C_SetRenderDrawColor(r, a, b, c, d) (SDL_SetRenderDrawColor((r), (Uint8)(a), (Uint8)(b), (Uint8)(c), (Uint8)(d)) == 0)
#define M8C_RenderTexture(rend, tex, src, dst) (m8c_render_texture_f((rend), (tex), (src), (dst)))
#define M8C_RenderFillRect(rend, r) (m8c_fill_rect_f((rend), (r)))
#define M8C_RenderPoints(rend, pts, n) (m8c_render_points_f((rend), (pts), (n)))
#define M8C_RenderLines(rend, pts, n) (m8c_render_lines_f((rend), (pts), (n)))
#define M8C_SetRenderTarget(rend, tex) (SDL_SetRenderTarget((rend), (tex)) == 0)
#define M8C_SetTextureScaleMode(t, m) (SDL_SetTextureScaleMode((t), (m)) == 0)
#define M8C_SetTextureBlendMode(t, m) (SDL_SetTextureBlendMode((t), (m)) == 0)
#define M8C_SetRenderVSync(rend, on) (m8c_set_render_vsync((rend), (on)))

static inline int m8c_sdl_init_ok(int rc) { return rc == 0; }

#else /* SDL3 */

#include <SDL3/SDL.h>

typedef SDL_AppResult m8c_app_result;
#define M8C_APP_CONTINUE SDL_APP_CONTINUE
#define M8C_APP_SUCCESS SDL_APP_SUCCESS
#define M8C_APP_FAILURE SDL_APP_FAILURE

#define M8C_EVENT_KEY_TYPE(ev) ((ev)->key.type)

#define M8C_ShowCursor() SDL_ShowCursor()
#define M8C_HideCursor() SDL_HideCursor()

#define M8C_EVENT_KEY_SYM(ev) ((ev)->key.key)
#define M8C_EVENT_KEY_SCANCODE(ev) ((ev)->key.scancode)
#define M8C_EVENT_KEY_MOD(ev) ((ev)->key.mod)
#define M8C_EVENT_KEY_REPEAT(ev) ((ev)->key.repeat)
#define M8C_EVENT_GP_BTN(ev) ((ev)->gbutton.button)
#define M8C_EVENT_GP_AXIS(ev) ((ev)->gaxis.axis)
#define M8C_EVENT_GP_AXIS_VAL(ev) ((ev)->gaxis.value)

#define M8C_RenderClear SDL_RenderClear
#define M8C_SetRenderDrawColor SDL_SetRenderDrawColor
#define M8C_RenderTexture SDL_RenderTexture
#define M8C_RenderFillRect SDL_RenderFillRect
#define M8C_RenderPoints SDL_RenderPoints
#define M8C_RenderLines SDL_RenderLines
#define M8C_SetRenderTarget SDL_SetRenderTarget
#define M8C_SetTextureScaleMode SDL_SetTextureScaleMode
#define M8C_SetTextureBlendMode SDL_SetTextureBlendMode
#define M8C_SetRenderVSync SDL_SetRenderVSync

static inline int m8c_sdl_init_ok(SDL_bool ok) { return ok ? 1 : 0; }

#endif

#endif /* M8C_SDL_H */

#ifndef M8C_SDL_COMPAT_H
#define M8C_SDL_COMPAT_H

#include "m8c_sdl.h"

#ifdef M8C_USE_SDL2

int m8c_create_window_and_renderer(const char *title, int w, int h, Uint32 flags, SDL_Window **win,
                                   SDL_Renderer **rend);
int m8c_set_render_logical_presentation(SDL_Renderer *rend, int w, int h, int mode);
int m8c_get_window_size_in_pixels(SDL_Window *win, int *w, int *h);
void m8c_sync_window(SDL_Window *win);

int m8c_get_connected_gamepad_count(void);

const char *m8c_get_gamepad_string_for_button(SDL_GamepadButton b);
const char *m8c_get_gamepad_string_for_axis(SDL_GamepadAxis a);
SDL_GamepadButtonLabel m8c_get_gamepad_button_label(SDL_Gamepad *pad, SDL_GamepadButton btn);

void m8c_query_texture_size(SDL_Texture *t, int *w, int *h);

int m8c_render_texture_f(SDL_Renderer *rend, SDL_Texture *tex, const SDL_FRect *src,
                         const SDL_FRect *dst);

int m8c_fill_rect_f(SDL_Renderer *rend, const SDL_FRect *rect);
int m8c_render_points_f(SDL_Renderer *rend, const SDL_FPoint *pts, int n);
int m8c_render_lines_f(SDL_Renderer *rend, const SDL_FPoint *pts, int n);

int m8c_set_render_vsync(SDL_Renderer *rend, int on);

#else

static inline int m8c_create_window_and_renderer(const char *title, int w, int h, Uint32 flags,
                                                 SDL_Window **win, SDL_Renderer **rend) {
  return SDL_CreateWindowAndRenderer(title, w, h, flags, win, rend) ? 0 : -1;
}

static inline int m8c_set_render_logical_presentation(SDL_Renderer *rend, int w, int h, int mode) {
  SDL_RendererLogicalPresentation m =
      (mode == SDL_LOGICAL_PRESENTATION_INTEGER_SCALE) ? SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
                                                        : SDL_LOGICAL_PRESENTATION_DISABLED;
  return SDL_SetRenderLogicalPresentation(rend, w, h, m) ? 0 : -1;
}

static inline int m8c_get_window_size_in_pixels(SDL_Window *win, int *w, int *h) {
  return SDL_GetWindowSizeInPixels(win, w, h) ? 0 : -1;
}

static inline void m8c_sync_window(SDL_Window *win) { SDL_SyncWindow(win); }

static inline int m8c_get_connected_gamepad_count(void) {
  int n = 0;
  SDL_JoystickID *ids = SDL_GetGamepads(&n);
  if (ids) {
    SDL_free(ids);
  }
  return n;
}

static inline const char *m8c_get_gamepad_string_for_button(SDL_GamepadButton b) {
  return SDL_GetGamepadStringForButton(b);
}

static inline const char *m8c_get_gamepad_string_for_axis(SDL_GamepadAxis a) {
  return SDL_GetGamepadStringForAxis(a);
}

static inline SDL_GamepadButtonLabel m8c_get_gamepad_button_label(SDL_Gamepad *pad,
                                                                  SDL_GamepadButton btn) {
  return SDL_GetGamepadButtonLabel(pad, btn);
}

static inline void m8c_query_texture_size(SDL_Texture *t, int *w, int *h) {
  *w = (int)SDL_GetNumberProperty(SDL_GetTextureProperties(t), SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
  *h = (int)SDL_GetNumberProperty(SDL_GetTextureProperties(t), SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
}

static inline int m8c_render_texture_f(SDL_Renderer *rend, SDL_Texture *tex, const SDL_FRect *src,
                                       const SDL_FRect *dst) {
  return SDL_RenderTexture(rend, tex, src, dst) ? 1 : 0;
}

static inline int m8c_fill_rect_f(SDL_Renderer *rend, const SDL_FRect *rect) {
  return SDL_RenderFillRect(rend, rect) ? 1 : 0;
}

static inline int m8c_render_points_f(SDL_Renderer *rend, const SDL_FPoint *pts, int n) {
  return SDL_RenderPoints(rend, pts, n) ? 1 : 0;
}

static inline int m8c_render_lines_f(SDL_Renderer *rend, const SDL_FPoint *pts, int n) {
  return SDL_RenderLines(rend, pts, n) ? 1 : 0;
}

static inline int m8c_set_render_vsync(SDL_Renderer *rend, int on) {
  return SDL_SetRenderVSync(rend, on) ? 1 : 0;
}

#endif

#endif

/* SDL2-only implementations; not compiled for SDL3 builds. */
#ifdef M8C_USE_SDL2

#include "m8c_sdl_compat.h"
#include <stdlib.h>

int m8c_create_window_and_renderer(const char *title, int w, int h, Uint32 flags, SDL_Window **win,
                                   SDL_Renderer **rend) {
  *win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags);
  if (*win == NULL) {
    return -1;
  }
  *rend = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED);
  if (*rend == NULL) {
    SDL_DestroyWindow(*win);
    *win = NULL;
    return -1;
  }
  return 0;
}

int m8c_set_render_logical_presentation(SDL_Renderer *rend, int w, int h, int mode) {
  if (mode == SDL_LOGICAL_PRESENTATION_DISABLED || (w == 0 && h == 0)) {
    SDL_RenderSetIntegerScale(rend, SDL_FALSE);
    SDL_RenderSetLogicalSize(rend, 0, 0);
    return 0;
  }
  SDL_RenderSetLogicalSize(rend, w, h);
  SDL_RenderSetIntegerScale(rend, mode == SDL_LOGICAL_PRESENTATION_INTEGER_SCALE ? SDL_TRUE : SDL_FALSE);
  return 0;
}

int m8c_get_window_size_in_pixels(SDL_Window *win, int *w, int *h) {
#if SDL_VERSION_ATLEAST(2, 26, 0)
  SDL_GetWindowSizeInPixels(win, w, h);
#else
  SDL_GetWindowSize(win, w, h);
#endif
  return 0;
}

void m8c_sync_window(SDL_Window *win) {
  (void)win;
}

int m8c_get_connected_gamepad_count(void) {
  int n = 0;
  const int num = SDL_NumJoysticks();
  for (int i = 0; i < num; i++) {
    if (SDL_IsGameController(i)) {
      n++;
    }
  }
  return n;
}

const char *m8c_get_gamepad_string_for_button(SDL_GamepadButton b) {
  return SDL_GameControllerGetStringForButton((SDL_GameControllerButton)b);
}

const char *m8c_get_gamepad_string_for_axis(SDL_GamepadAxis a) {
  return SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)a);
}

SDL_GamepadButtonLabel m8c_get_gamepad_button_label(SDL_Gamepad *pad, SDL_GamepadButton btn) {
  return SDL_GameControllerGetButtonLabel((SDL_GameController *)pad, (SDL_GameControllerButton)btn);
}

void m8c_query_texture_size(SDL_Texture *t, int *w, int *h) { SDL_QueryTexture(t, NULL, NULL, w, h); }

static void frect_to_rect(const SDL_FRect *f, SDL_Rect *o) {
  if (!f || !o) {
    return;
  }
  o->x = (int)SDL_floorf(f->x);
  o->y = (int)SDL_floorf(f->y);
  o->w = (int)SDL_ceilf(f->w + f->x) - o->x;
  o->h = (int)SDL_ceilf(f->h + f->y) - o->y;
}

int m8c_render_texture_f(SDL_Renderer *rend, SDL_Texture *tex, const SDL_FRect *src,
                         const SDL_FRect *dst) {
#if SDL_VERSION_ATLEAST(2, 0, 10)
  return SDL_RenderCopyF(rend, tex, src, dst) == 0 ? 1 : 0;
#else
  SDL_Rect s, d;
  const SDL_Rect *ps = NULL;
  const SDL_Rect *pd = NULL;
  if (src) {
    frect_to_rect(src, &s);
    ps = &s;
  }
  if (dst) {
    frect_to_rect(dst, &d);
    pd = &d;
  }
  return SDL_RenderCopy(rend, tex, ps, pd) == 0 ? 1 : 0;
#endif
}

int m8c_fill_rect_f(SDL_Renderer *rend, const SDL_FRect *rect) {
  if (!rect) {
    return 0;
  }
#if SDL_VERSION_ATLEAST(2, 0, 10)
  return SDL_RenderFillRectF(rend, rect) == 0 ? 1 : 0;
#else
  SDL_Rect r;
  frect_to_rect(rect, &r);
  return SDL_RenderFillRect(rend, &r) == 0 ? 1 : 0;
#endif
}

int m8c_render_points_f(SDL_Renderer *rend, const SDL_FPoint *pts, int n) {
  if (!pts || n <= 0) {
    return 0;
  }
#if SDL_VERSION_ATLEAST(2, 0, 10)
  return SDL_RenderDrawPointsF(rend, pts, n) == 0 ? 1 : 0;
#else
  SDL_Point *tmp = (SDL_Point *)SDL_malloc(sizeof(SDL_Point) * (size_t)n);
  if (!tmp) {
    return 0;
  }
  for (int i = 0; i < n; i++) {
    tmp[i].x = (int)SDL_lroundf(pts[i].x);
    tmp[i].y = (int)SDL_lroundf(pts[i].y);
  }
  const int ok = SDL_RenderDrawPoints(rend, tmp, n) == 0 ? 1 : 0;
  SDL_free(tmp);
  return ok;
#endif
}

int m8c_render_lines_f(SDL_Renderer *rend, const SDL_FPoint *pts, int n) {
  if (!pts || n <= 0) {
    return 0;
  }
#if SDL_VERSION_ATLEAST(2, 0, 10)
  return SDL_RenderDrawLinesF(rend, pts, n) == 0 ? 1 : 0;
#else
  SDL_Point *tmp = (SDL_Point *)SDL_malloc(sizeof(SDL_Point) * (size_t)n);
  if (!tmp) {
    return 0;
  }
  for (int i = 0; i < n; i++) {
    tmp[i].x = (int)SDL_lroundf(pts[i].x);
    tmp[i].y = (int)SDL_lroundf(pts[i].y);
  }
  const int ok = SDL_RenderDrawLines(rend, tmp, n) == 0 ? 1 : 0;
  SDL_free(tmp);
  return ok;
#endif
}

int m8c_set_render_vsync(SDL_Renderer *rend, int on) {
#if SDL_VERSION_ATLEAST(2, 0, 17)
  return SDL_RenderSetVSync(rend, on) == 0 ? 1 : 0;
#else
  (void)rend;
  (void)on;
  return 1;
#endif
}

#endif /* M8C_USE_SDL2 */

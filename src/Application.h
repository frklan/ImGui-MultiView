// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0
#pragma once

#include <algorithm>
#include <fmt/core.h>
#include <memory>
#include <numeric>
#include <span>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <implot.h>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

#include "FontList.h"
#include "Window.h"

namespace mv {
  class Application final {
      using shared_renderer_t = std::shared_ptr<SDL_Renderer>;
      using shared_texture_t = std::shared_ptr<SDL_Texture>;
      using shared_window_t = std::shared_ptr<SDL_Window>;

    public:
      Application();
      ~Application();

      int run();

    private:
      std::vector<std::unique_ptr<Window>> m_windows;
      shared_window_t m_window;
      shared_renderer_t m_renderer;

      FontList m_small_font;
      FontList m_big_font;
      FontList m_default_font;

      bool m_window_is_hidden{false};
      bool m_show_about{false};


      void setup_sdl();
      void setup_imgui();

      void set_theme();
      void load_fonts();

      void begin_render();
      void render();
      void end_render();

      void render_main_menu();
      void render_about_box();

      bool process_events();
  };
}// namespace mv

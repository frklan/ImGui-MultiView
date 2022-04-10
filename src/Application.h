// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0
#pragma once

#include <memory>
#include <span>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <implot.h>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

#include "Window.h"


namespace ImGui {
  bool Shortcut(ImGuiKeyModFlags mod, ImGuiKey key, bool repeat);
}// namespace ImGui


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
      shared_window_t m_window;
      shared_renderer_t m_renderer;
      float m_ui_scale{1.5F};

      std::vector<std::unique_ptr<Window>> m_windows;

      virtual void set_theme();
      virtual void render();
      void process();
      bool process_events();

      shared_renderer_t get_renderer() { return m_renderer; }

      void begin_render();
      void end_render();

      void setup_sdl();
      void setup_imgui();
  };
}// namespace mv

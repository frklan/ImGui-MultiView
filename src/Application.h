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

namespace mv {
  class DebugWindow {
    public:
      DebugWindow() {
        uuids::uuid id = uuids::uuid_system_generator{}();
        m_window_title = "DebugView###" + uuids::to_string(id);
      }
      void render() {
        auto io = ImGui::GetIO();
        m_frametime_history.push_back(1000.0 / io.Framerate);
        if(m_frametime_history.size() > 500) {
          m_frametime_history.erase(m_frametime_history.begin(), m_frametime_history.begin() + 1);
        }


        /*******/

        ImGui::SetNextWindowPos(ImVec2(650, 120), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        if(ImGui::Begin(m_window_title.c_str(), &m_is_open, ImGuiWindowFlags_NoSavedSettings)) {
          ImGui::Text("Hello world!");
          ImGui::Text("%s", m_window_title.c_str());

          ImGui::BeginChild("left pane", ImVec2(150, 0), true);
          ImGui::Text("DUT ID");
          ImGui::SameLine();
          if(ImGui::Button("Find")) {
          }
          ImGui::Separator();
          if(ImGui::BeginTable("Table", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Text");
            ImGui::TableNextColumn();
            ImGui::Text("Text 2");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Text 3");
            ImGui::TableNextColumn();
            ImGui::Text("Text 4");
            ImGui::EndTable();
          }
          ImGui::EndChild();
          ImGui::SameLine();

          ImGui::BeginChild("right pane");
          if(ImPlot::BeginPlot("Frame time", nullptr, "mS", ImVec2(-1, 0), ImPlotFlags_None, ImPlotAxisFlags_AutoFit)) {
            ImPlot::PlotLine("##time", m_frametime_history.data(), static_cast<int>(m_frametime_history.size()));
            ImPlot::EndPlot();
          }

          ImGui::EndChild();
        }
        ImGui::End();
        /************/
      }


      [[nodiscard]] bool close() const {
        return !m_is_open;
      }

    private:
      std::string m_window_title;
      bool m_is_open{true};

      std::vector<float> m_frametime_history;
  };

  class Application {
      using shared_renderer_t = std::shared_ptr<SDL_Renderer>;
      using shared_texture_t = std::shared_ptr<SDL_Texture>;
      using shared_window_t = std::shared_ptr<SDL_Window>;

    public:
      Application();
      virtual ~Application();

      int run();

    private:
      shared_window_t m_window;
      shared_renderer_t m_renderer;
      float m_ui_scale{1.5F};

      std::vector<DebugWindow> m_debug_windows;

      virtual void set_theme(){};
      virtual void render() = 0;
      bool process_events();
      void render_main_debug();

      shared_renderer_t get_renderer() { return m_renderer; }

      void begin_render();
      void end_render();

      void setup_sdl();
      void setup_imgui();
  };
}// namespace mv

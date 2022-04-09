// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

#include "Window.h"

namespace mv {
  class DebugWindow : public Window {
    public:
      DebugWindow() {
        uuids::uuid id = uuids::uuid_system_generator{}();
        m_window_title = "DebugView###" + uuids::to_string(id);
      }

      void render() override {
        auto io = ImGui::GetIO();
        m_frametime_history.push_back(io.DeltaTime * 1000.0F);// NOLINT
        if(m_frametime_history.size() > MAX_HISTORY_LENGHT) {
          m_frametime_history.erase(m_frametime_history.begin(), m_frametime_history.begin() + 1);
        }

        ImGui::SetNextWindowPos(ImVec2(DEFAULT_WINDOW_POS_X, DEFAULT_WINDOW_POS_Y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), ImGuiCond_FirstUseEver);
        if(ImGui::Begin(m_window_title.c_str(), &m_is_open, ImGuiWindowFlags_NoSavedSettings)) {
          if(ImPlot::BeginPlot("Frame time", nullptr, "mS", ImGui::GetContentRegionAvail(), ImPlotFlags_None, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoDecorations)) {// NOLINT:hicpp-signed-bitwise
            ImPlot::PlotLine("##time", m_frametime_history.data(), static_cast<int>(m_frametime_history.size()));
            ImPlot::EndPlot();
          }
        }
        ImGui::End();
      }

    private:
      std::string m_window_title;
      std::vector<float> m_frametime_history;

      static constexpr size_t MAX_HISTORY_LENGHT = 500;
      static constexpr float DEFAULT_WINDOW_POS_X = 650.0F;
      static constexpr float DEFAULT_WINDOW_POS_Y = 120.0F;
      static constexpr float DEFAULT_WINDOW_WIDTH = 640.0F;
      static constexpr float DEFAULT_WINDOW_HEIGHT = 480.0F;
  };
}// namespace mv
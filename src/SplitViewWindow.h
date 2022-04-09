// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <misc/cpp/imgui_stdlib.h>
#include <spdlog/spdlog.h>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

#include "Window.h"

namespace mv {
  class SplitViewWindow : public Window {
    public:
      SplitViewWindow() {
        uuids::uuid id = uuids::uuid_system_generator{}();
        m_window_id = uuids::to_string(id);
        m_window_title = "GH-4261 (Flipped)###" + m_window_id;
      }


      /****
        +-------------+-----------------+
        | Left       |  Right           |
        | +--------+ |                  |
        | | Top    | |                  |
        | | Left   | |                  |
        | |        | |                  |
        | +--------+ |                  |
        | | bottom | |                  |
        | | Left   | |                  |
        | |        | |                  |
        | +--------+ |                  |
        +------------+------------------+
        **/
      void render() override {
        ImGui::SetNextWindowSizeConstraints(ImVec2(MINIMUM_WINDOW_WIDTH, MINIMUM_WINDOW_HEIGHT), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowPos(ImVec2(DEFAULT_WINDOW_POS_X, DEFAULT_WINDOW_POS_Y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), ImGuiCond_FirstUseEver);

        if(ImGui::Begin(m_window_title.c_str(), &m_is_open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar)) {
          render_menu();

          ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

          if(!m_hide_search) {
            render_left_child();

            ImGui::SameLine();
            ImGui::InvisibleButton("vsplitter", ImVec2(SPLIT_GAP, ImGui::GetContentRegionAvail().y));
            if(ImGui::IsItemActive()) {
              m_vertical_split = std::max(MINIMUM_SPLIT_SIZE, m_vertical_split + ImGui::GetIO().MouseDelta.x);
            }
            m_vertical_split = std::min(m_vertical_split, ImGui::GetWindowWidth() - MINIMUM_SPLIT_SIZE);
            ImGui::SameLine();
          }

          render_right_child();

          ImGui::PopStyleVar();
        }
        ImGui::End();
      }

    private:
      std::string m_window_title{};
      std::string m_window_id{};
      std::string m_some_input{};
      std::vector<std::string> m_items;
      bool m_window_has_focus{false};
      bool m_hide_search{false};

      static constexpr float DEFAULT_HORIZONTAL_SPLIT = 150.0F;
      static constexpr float DEFAULT_VERTICAL_SPLIT = 250.0F;

      float m_horizontal_split{DEFAULT_HORIZONTAL_SPLIT};
      float m_vertical_split{DEFAULT_VERTICAL_SPLIT};

      static constexpr float MINIMUM_SPLIT_SIZE = 50.0F;
      static constexpr float SPLIT_GAP = 8.0F;
      static constexpr float DEFAULT_WINDOW_POS_X = 450.0F;
      static constexpr float DEFAULT_WINDOW_POS_Y = 200.0F;
      static constexpr float DEFAULT_WINDOW_WIDTH = 1024.0F;
      static constexpr float DEFAULT_WINDOW_HEIGHT = 768.0F;
      static constexpr float MINIMUM_WINDOW_HEIGHT = 300.0F;
      static constexpr float MINIMUM_WINDOW_WIDTH = 300.0F;

      void render_menu() {
        if(ImGui::BeginMenuBar()) {
          if(ImGui::BeginMenu("Window")) {
            if(ImGui::MenuItem("Close window")) {
              m_is_open = false;
            }

            ImGui::MenuItem("Hide search", nullptr, &m_hide_search);

            ImGui::EndMenu();
          }

          ImGui::EndMenuBar();
        }
      }

      void render_top_left_child() {
        ImGui::BeginChild("child2", ImVec2(0, m_horizontal_split), true);
        ImGui::Text("left upper");// NOLINT: hicpp-vararg
        ImGui::Text("h = %f", static_cast<double>(m_horizontal_split));// NOLINT: hicpp-vararg

        ImGui::NewLine();
        ImGui::NewLine();

        ImGui::InputText("###some_input", &m_some_input);
        ImGui::SameLine();
        if(ImGui::Button("Add")) {
          m_items.push_back(m_some_input);
          m_some_input.clear();
        }
        ImGui::EndChild();
      }
      void render_bottom_left_child() const {
        ImGui::BeginChild("child3", ImVec2(0, 0), true);
        ImGui::Text("left lower");// NOLINT: hicpp-vararg
        ImGui::Text("m_vertical_split = %f", static_cast<double>(m_vertical_split));// NOLINT: hicpp-vararg

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        if(m_window_has_focus) {
          ImGui::Text("This window has focus!");
        }

        ImGui::EndChild();
      }
      void render_left_child() {
        ImGui::BeginChild("Left", ImVec2(m_vertical_split, 0));
        render_top_left_child();

        ImGui::InvisibleButton("hsplitter", ImVec2(-1, SPLIT_GAP));
        if(ImGui::IsItemActive()) {
          m_horizontal_split = std::max(MINIMUM_SPLIT_SIZE, m_horizontal_split + ImGui::GetIO().MouseDelta.y);
        }
        m_horizontal_split = std::min(m_horizontal_split, ImGui::GetWindowHeight() - MINIMUM_SPLIT_SIZE);

        render_bottom_left_child();

        ImGui::EndChild();
      }
      void render_right_child() const {
        ImGui::BeginChild("right", ImVec2(0, 0), true);
        ImGui::Text("Right pane");// NOLINT: hicpp-vararg

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        if(ImGui::BeginTable(std::string("#itemslist" + m_window_id).c_str(), 1)) {
          for(auto const &i : m_items) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", i.c_str());// NOLINT: hicpp-vararg
          }
          ImGui::EndTable();
        }


        ImGui::EndChild();
      }
  };
}// namespace mv
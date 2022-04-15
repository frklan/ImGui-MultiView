// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#pragma once

#include <string_view>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <misc/cpp/imgui_stdlib.h>
#include <spdlog/spdlog.h>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

#include "Application.h"
#include "ImGuiUtil.h"
#include "Window.h"

namespace mv {
  class SplitViewWindow : public Window {
    public:
      SplitViewWindow() {
        uuids::uuid id = uuids::uuid_system_generator{}();
        m_window_id = uuids::to_string(id);
        m_window_title = fmt::format("MV-1337 ###{}", m_window_id);
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

        is_disabled = m_items.size() > 10;

        if(ImGui::Begin(m_window_title.c_str(), &m_is_open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar)) {
          ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
          ImGui::BeginDisabled(is_disabled);

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

          ImGui::EndDisabled();
          if(is_disabled) {
            y44::im_popup_modal("Please wait!", m_window_id);
          }
          ImGui::PopStyleVar();

          render_menu();
          process_shortcuts();
        }
        ImGui::End();
      }

    private:
      std::string m_window_title{};
      std::string m_window_id{};
      std::string m_some_input{};
      std::vector<std::string> m_items{};
      float m_horizontal_split{DEFAULT_HORIZONTAL_SPLIT};
      float m_vertical_split{DEFAULT_VERTICAL_SPLIT};
      bool m_hide_search{false};
      bool is_disabled{false};

      static constexpr float DEFAULT_HORIZONTAL_SPLIT = 150.0F;
      static constexpr float DEFAULT_VERTICAL_SPLIT = 250.0F;
      static constexpr float DEFAULT_WINDOW_POS_X = 450.0F;
      static constexpr float DEFAULT_WINDOW_POS_Y = 200.0F;
      static constexpr float DEFAULT_WINDOW_WIDTH = 1024.0F;
      static constexpr float DEFAULT_WINDOW_HEIGHT = 768.0F;
      static constexpr float MINIMUM_WINDOW_HEIGHT = 300.0F;
      static constexpr float MINIMUM_WINDOW_WIDTH = 300.0F;
      static constexpr float MINIMUM_SPLIT_SIZE = 50.0F;
      static constexpr float SPLIT_GAP = 8.0F;

      void render_menu() {
        if(ImGui::BeginMenuBar()) {
          if(ImGui::BeginMenu("Window")) {
            if(ImGui::MenuItem("Close window")) {
              m_is_open = false;
            }

            ImGui::MenuItem("Hide search", "⌘+B", &m_hide_search);

            if(is_disabled) {
              ImGui::Separator();
              if(ImGui::MenuItem("Clear list")) {
                m_items.clear();
              }
            }
            ImGui::EndMenu();
          }
          ImGui::SameLine(0, 100);
          ImGui::MenuItem(fmt::format("{}", m_items.size()).c_str(), nullptr, false, false);

          ImGui::EndMenuBar();
        }
      }

      void process_shortcuts() {
        if(!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
          return;
        }

        if(y44::im_shortcut(ImGuiModFlags_Super, ImGuiKey_B, false)) {
          m_hide_search = !m_hide_search;
        }
      }

      void render_top_left_child() {
        ImGui::BeginChild("child2", ImVec2(0, m_horizontal_split), true);
        y44::im_text("left upper");
        y44::im_text("m_vertical_split = {}", m_horizontal_split);

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
        y44::im_text("left lower");
        y44::im_text("m_vertical_split = {}", m_vertical_split);

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

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
      void render_right_child() {
        ImGui::BeginChild("right", ImVec2(0, 0), true);
        y44::im_text("Right pane");

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        if(ImGui::BeginTable(fmt::format("#itemslist{}", m_window_id).c_str(), 1)) {
          for(auto const &i : m_items) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            y44::im_text("{}", i);
          }
          ImGui::EndTable();
        }
        ImGui::EndChild();
      }
  };
}// namespace mv
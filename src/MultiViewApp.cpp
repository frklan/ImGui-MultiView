// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#include <algorithm>
#include <cassert>
#include <chrono>
#include <exception>
#include <filesystem>
#include <memory>
#include <thread>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <imgui_internal.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "MultiViewApp.h"


namespace mv {
  using namespace std::chrono_literals;
  void MultiViewApp::render() {
    // ImGui::ShowDemoWindow();
    //  ImPlot::ShowDemoWindow();
    //  ImGui::ShowMetricsWindow();

    render_split_windows();
  }


  void MultiViewApp::set_theme() {
    const ImVec4 bg = ImColor(0x28, 0x2a, 0x36);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    const ImVec4 bg_darker = ImColor(0x19, 0x1a, 0x21);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    const ImVec4 purple = ImColor(0xbd, 0x93, 0xf9, 0x55);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

    auto &style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0F;
    style.WindowBorderSize = 1.0F;
    style.WindowRounding = 8.0F;// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    style.WindowPadding = ImVec2(5.0F, 5.0F);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

    style.Colors[ImGuiCol_WindowBg] = bg;
    style.Colors[ImGuiCol_HeaderActive] = bg_darker;
    style.Colors[ImGuiCol_DockingEmptyBg] = bg_darker;
    style.Colors[ImGuiCol_Border] = purple;
    style.Colors[ImGuiCol_MenuBarBg] = bg_darker;
    style.Colors[ImGuiCol_TitleBgActive] = bg_darker;
  }

  void MultiViewApp::render_split_windows() {

    auto *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), viewport->Size);


    ImGui::SetNextWindowPos(ImVec2(450, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(1024, 768), ImGuiCond_FirstUseEver);
    ImGui::Begin("GH-4261 (Flipped)");
    static float w = 200.0f;
    static float h = 300.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));


    ImGui::BeginChild("Left", ImVec2(w, 0));
    {

      ImGui::BeginChild("child2", ImVec2(0, h), true);
      ImGui::Text("left upper");
      ImGui::Text("h = %f", h);
      ImGui::EndChild();


      ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
      if(ImGui::IsItemActive()) {
        h = std::max(50.0F, h + ImGui::GetIO().MouseDelta.y);
      }
      h = std::min(h, ImGui::GetWindowHeight() - 50.0F);

      ImGui::BeginChild("child3", ImVec2(0, 0), true);
      ImGui::Text("left lower");
      ImGui::Text("w = %f", w);
      ImGui::EndChild();

      ImGui::EndChild();// left part
    }

    ImGui::SameLine();
    ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, ImGui::GetContentRegionAvail().y));
    if(ImGui::IsItemActive()) {
      w = std::max(50.0F, w + ImGui::GetIO().MouseDelta.x);
    }
    w = std::min(w, ImGui::GetWindowWidth() - 50.0F);

    ImGui::SameLine();

    ImGui::BeginChild("right", ImVec2(0, 0), true);
    ImGui::Text("Right pane");
    ImGui::EndChild();

    ImGui::PopStyleVar();
    ImGui::End();
  }

}// namespace mv

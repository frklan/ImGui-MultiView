// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

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
#include <implot.h>
#include <spdlog/spdlog.h>

#include "MultiViewApp.h"


namespace mv {
  using namespace std::chrono_literals;
  void MultiViewApp::render() {
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
    ImGui::ShowMetricsWindow();
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

}// namespace mv

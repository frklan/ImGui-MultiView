// Copyright (C) 2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#pragma once

#include <string_view>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>

inline ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs) noexcept {
  return ImVec2{lhs.x + rhs.x, lhs.y + rhs.y};
}

inline ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs) noexcept {
  return ImVec2{lhs.x - rhs.x, lhs.y - rhs.y};
}

namespace y44 {
  inline bool im_shortcut(ImGuiKeyModFlags mod, ImGuiKey key, bool repeat) {
    return mod == ImGui::GetMergedModFlags() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(key), repeat);
  }

  template <typename... Args>
  inline void im_centered_text(const std::string_view fmt, Args &&...args) {
    const auto text = fmt::format(fmt::runtime(fmt), args...);
    const auto text_size = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_size) * 0.5F);// NOLINT:cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    ImGui::Text("%s", text.c_str());// NOLINT
  }

  template <typename... Args>
  inline void im_centered_text(ImFont *font, const std::string_view fmt, Args &&...args) {
    ImGui::PushFont(font);
    im_centered_text(fmt, args...);
    ImGui::PopFont();
  }

  enum Align {
    LEFT,
    CENTER,
    RIGHT
  };

  template <typename... Args>
  inline void im_text(std::string_view fmt, Args &&...args) {
    im_text(LEFT, fmt, args...);
  }

  template <typename... Args>
  inline void im_text(ImFont *font, Align align, std::string_view fmt, Args &&...args) {
    ImGui::PushFont(font);
    im_text(align, fmt, args...);
    ImGui::PopFont();
  }

  template <typename... Args>
  inline void im_text(Align align, std::string_view fmt, Args &&...args) {
    const auto text = fmt::format(fmt::runtime(fmt), args...);
    if(align == CENTER) {
      const auto text_size = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_size) * 0.5F);// NOLINT:cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    } else if(align == RIGHT) {
      const auto text_size = ImGui::CalcTextSize(text.c_str()).x;
      auto padding = ImGui::GetStyle().WindowPadding.x;
      ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_size) - padding);// NOLINT:cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    }
    ImGui::Text("%s", text.c_str());// NOLINT
  }

  inline void im_popup_modal(const std::string_view text, const std::string &window_id) {
    auto parent_pos = ImGui::GetWindowPos();
    auto parent_size = ImGui::GetWindowSize();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));// NOLINT:cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

    ImGui::Begin(fmt::format("###disabled{}", window_id).c_str(), nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_Modal);
    im_text("{}", text);

    auto popup_size = ImGui::GetWindowSize();
    auto popup_pos = ImVec2((parent_pos.x + parent_size.x / 2) - popup_size.x / 2, (parent_pos.y + parent_size.y / 2) - popup_size.y / 2);
    ImGui::SetWindowPos(popup_pos);
    ImGui::End();

    ImGui::PopStyleVar();
  }
}// namespace y44
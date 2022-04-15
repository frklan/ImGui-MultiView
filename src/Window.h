// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <spdlog/spdlog.h>


namespace mv {
  class Window {
    public:
      Window() = default;
      Window(Window &) = default;
      Window(Window &&) = default;

      virtual ~Window() = default;

      Window &operator=(const Window &) = default;
      Window &operator=(Window &&) = default;

      virtual void render() = 0;

      [[nodiscard]] bool should_close() const {
        return !m_is_open;
      }

    protected:
      bool m_is_open{true};// NOLINT: cppcoreguidelines-non-private-member-variables-in-classes
  };
}// namespace mv
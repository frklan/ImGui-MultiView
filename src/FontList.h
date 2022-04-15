// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0
#pragma once

#include <numeric>
#include <vector>

#include <imgui.h>

namespace mv {
  class FontList {
    public:
      void push_back(ImFont *font) {
        m_fonts.push_back(font);
        m_current_font = std::midpoint(static_cast<size_t>(0), size());
      }

      void reserve(size_t count) {
        m_fonts.reserve(count);
      }

      [[nodiscard]] size_t size() const {
        return m_fonts.size();
      }

      operator ImFont *() {// NOLINT: hicpp-explicit-conversions
        if(m_fonts.empty()) {
          return nullptr;
        }
        return m_fonts[m_current_font];
      }

      FontList operator++() {
        m_current_font = std::min(m_current_font + 1, m_fonts.size() - 1);
        return *this;
      }

      FontList operator--() {
        if(m_current_font > 0) {
          --m_current_font;
        }
        return *this;
      }

    private:
      size_t m_current_font{0};
      std::vector<ImFont *> m_fonts;
  };
}// namespace mv
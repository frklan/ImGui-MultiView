// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0
#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "Application.h"

namespace mv {
  class MultiViewApp : public Application {

    private:
      void set_theme() override;
      void render() override;
  };
}// namespace mv

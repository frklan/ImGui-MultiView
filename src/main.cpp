// Copyright (C) 2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#include <exception>

#include <spdlog/spdlog.h>

#include "Application.h"

/************************
 *
 *         gui
 *
 ************************/

int main(int /*argc*/, char ** /*argv*/) {
  spdlog::info("-=gui=-");
  try {
    mv::Application app;
    app.run();
  } catch(std::exception &e) {
    spdlog::error("{}", e.what());
    return -1;
  } catch(...) {
    return -1;
  }

  return 0;
}

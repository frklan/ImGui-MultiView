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
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#include "Application.h"

using namespace std::chrono_literals;

constexpr int WIDTH = 1920 /*2560*/ /*3840*/;
constexpr int HEIGHT = 1080 /*1440*/ /*2160 */;
constexpr float UI_SCALE_MAX = 15.0F;
constexpr float UI_SCALE_MIN = 1.0F;

namespace mv {
  using namespace std::chrono_literals;

  Application::Application() {
    setup_sdl();
    setup_imgui();

    m_debug_windows.emplace_back();
  }

  Application::~Application() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_Quit();
  }

  bool Application::process_events() {
    bool should_quit = false;
    SDL_Event event{};
    while(SDL_PollEvent(&event) != 0) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch(event.type) {
      case SDL_QUIT:
        should_quit = true;
        break;
      case SDL_KEYDOWN:
        if((event.key.keysym.mod & KMOD_CTRL) != 0) {
          switch(event.key.keysym.sym) {
          case SDLK_PLUS:
            m_ui_scale = std::clamp(m_ui_scale + 1.0F, UI_SCALE_MIN, UI_SCALE_MAX);
            break;
          case SDLK_MINUS:
            m_ui_scale = std::clamp(m_ui_scale - 1.0F, UI_SCALE_MIN, UI_SCALE_MAX);
            break;
          case SDLK_n:
            m_debug_windows.emplace_back();
            break;
          default:
            break;
          }
        }
        break;
      default:
        break;
      }
    }
    return should_quit;
  }

  int Application::run() {
    // Main loop
    bool should_quit{false};
    try {
      while(!should_quit) {
        should_quit = process_events();

        /* Draw stuff */
        set_theme();
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window.get());
        ImGui::NewFrame();

        begin_render();

        for(auto &win : m_debug_windows) {
          win.render();
        }
        m_debug_windows.erase(std::remove_if(m_debug_windows.begin(), m_debug_windows.end(), [](const DebugWindow &w) { return w.close(); }), m_debug_windows.end());

        render();
        // render_main_debug();
        end_render();
      }
    } catch(std::exception &e) {
      spdlog::error("{}", e.what());
    } catch(...) {
      spdlog::error("Error occured..");
    }
    return 0;
  }

  void Application::render_main_debug() {
    const auto *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);


    const auto window_title = fmt::format("Main Debug {} {}###AnimatedTitle", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], ImGui::GetFrameCount());
    if(ImGui::Begin(window_title.c_str(), nullptr)) {
      ImGui::Text("%f", m_ui_scale);
      ImGui::Text("%d debug views", m_debug_windows.size());

      if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("Menu")) {

          if(ImGui::MenuItem("Open new debug", nullptr)) {
            m_debug_windows.emplace_back();
          }
          ImGui::MenuItem("Console", nullptr);
          ImGui::MenuItem("Log", nullptr);

          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }
    }
    ImGui::End();
  }

  void Application::setup_sdl() {
    SDL_Init(SDL_INIT_EVERYTHING);

    m_window = shared_window_t{SDL_CreateWindow("MultiView v0.0.1", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE), &SDL_DestroyWindow};
    m_renderer = shared_renderer_t{SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), &SDL_DestroyRenderer};
    SDL_ShowWindow(m_window.get());
  }

  void Application::setup_imgui() {
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGui::StyleColorsDark();
    ImPlot::StyleColorsDark();

    ImGui::GetStyle().ScaleAllSizes(m_ui_scale);
    ImGui_ImplSDL2_InitForSDLRenderer(m_window.get());
    ImGui_ImplSDLRenderer_Init(m_renderer.get());

    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }

  void Application::begin_render() {
    const auto *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    const auto *dockspace_name = "DockSpace##mainwidow";
    const auto dockspace_id = ImGui::GetID(dockspace_name);

    ImGui::Begin(dockspace_name,
      nullptr,
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

    // const auto dockspace_id = ImGui::GetID(dockspace_name);
    ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), ImGuiDockNodeFlags_None);
  }

  void Application::end_render() {
    ImGui::End();
    ImGui::Render();
    SDL_RenderClear(m_renderer.get());
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer.get());
  }
}// namespace mv

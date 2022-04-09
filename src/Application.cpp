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
#include "DebugWindow.h"
#include "SplitViewWindow.h"

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


    m_windows.push_back(std::make_unique<DebugWindow>());
    m_windows.push_back(std::make_unique<SplitViewWindow>());
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
            m_windows.push_back(std::make_unique<DebugWindow>());
            break;
          default:
            break;
          }
        } else if((event.key.keysym.mod & KMOD_GUI) != 0) {
          switch(event.key.keysym.sym) {
          case SDLK_n:
            m_windows.push_back(std::make_unique<SplitViewWindow>());
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

        process();

        begin_render();
        render();
        end_render();
      }
    } catch(std::exception &e) {
      spdlog::error("{}", e.what());
    } catch(...) {
      spdlog::error("Error occured..");
    }
    return 0;
  }

  void Application::process() {
    for(auto &win : m_windows) {
      win->process();
    }
  }

  void Application::render() {
    // ImGui::ShowDemoWindow();
    //   ImPlot::ShowDemoWindow();
    //   ImGui::ShowMetricsWindow();

    for(auto &win : m_windows) {
      win->render();
    }


    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(), [](const std::unique_ptr<Window> &w) { return w->should_close(); }), m_windows.end());
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
    ImGui_ImplSDL2_InitForSDLRenderer(m_window.get(), m_renderer.get());
    ImGui_ImplSDLRenderer_Init(m_renderer.get());

    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;// NOLINT:hicpp-signed-bitwise
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
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);// NOLINT:hicpp-signed-bitwise

    ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), ImGuiDockNodeFlags_None);
  }

  void Application::end_render() {
    ImGui::End();
    ImGui::Render();
    SDL_RenderClear(m_renderer.get());
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer.get());
  }

  void Application::set_theme() {
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

    // Set plot axis bg to transparent.
    auto &plot_style = ImPlot::GetStyle();
    plot_style.Colors[ImPlotCol_AxisBgHovered] = ImVec4(0, 0, 0, 0);
    plot_style.Colors[ImPlotCol_AxisBgActive] = ImVec4(0, 0, 0, 0);
  }
}// namespace mv

// Copyright (C) 2021-2022, Fredrik Andersson
// SPDX-License-Identifier: CC-BY-NC-4.0

#include <SDL_video.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstring>
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
#include "ImGuiUtil.h"
#include "SplitViewWindow.h"

#include "CourierPrime.h"

constexpr int WIDTH = 1920 /*2560*/ /*3840*/;
constexpr int HEIGHT = 1080 /*1440*/ /*2160 */;

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

    ImGui_ImplSDL2_InitForSDLRenderer(m_window.get(), m_renderer.get());
    ImGui_ImplSDLRenderer_Init(m_renderer.get());

    auto &io = ImGui::GetIO();
    io.ConfigWindowsResizeFromEdges = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;// NOLINT:hicpp-signed-bitwise
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;// NOLINT:hicpp-signed-bitwise

    load_fonts();
  }

  int Application::run() {
    using namespace std::chrono_literals;

    // Main loop
    bool should_quit{false};
    try {
      while(!should_quit) {
        should_quit = process_events();

        begin_render();
        render();
        end_render();

        if(m_window_is_hidden) {
          std::this_thread::sleep_for(100ms);// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
        }
      }
    } catch(std::exception &e) {
      spdlog::error("{}", e.what());
    } catch(...) {
      spdlog::error("Error occured..");
    }
    return 0;
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
      case SDL_WINDOWEVENT:
        switch(event.window.event) {
        case SDL_WINDOWEVENT_FOCUS_GAINED:
        case SDL_WINDOWEVENT_SHOWN:
          m_window_is_hidden = false;
          break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
        case SDL_WINDOWEVENT_HIDDEN:
          m_window_is_hidden = true;
          break;
        }
        break;
      case SDL_KEYDOWN:
        if((event.key.keysym.mod & KMOD_CTRL) != 0) {
          switch(event.key.keysym.sym) {
          case SDLK_n:
            m_windows.push_back(std::make_unique<DebugWindow>());
            break;
          }
        } else if((event.key.keysym.mod & KMOD_GUI) != 0) {
          switch(event.key.keysym.sym) {
          case SDLK_n:
            m_windows.push_back(std::make_unique<SplitViewWindow>());
            break;
          case SDLK_PLUS:
            ++m_default_font;
            break;
          case SDLK_MINUS:
            --m_default_font;
            break;
          }
        }
        break;
      }
    }
    return should_quit;
  }

  void Application::begin_render() {
    set_theme();
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window.get());
    ImGui::NewFrame();

    const auto *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    const auto *dockspace_name = "DockSpace##mainwidow";

    ImGui::PushFont(m_default_font);
    ImGui::Begin(dockspace_name,
      nullptr,
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);// NOLINT:hicpp-signed-bitwise

    const auto dockspace_id = ImGui::GetID(dockspace_name);
    ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), ImGuiDockNodeFlags_AutoHideTabBar);
  }

  void Application::render() {
    // ImGui::ShowDemoWindow();
    // ImPlot::ShowDemoWindow();
    // ImGui::ShowMetricsWindow();

    render_main_menu();
    render_about_box();

    for(auto &win : m_windows) {
      win->render();
    }
    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(), [](const std::unique_ptr<Window> &w) { return w->should_close(); }), m_windows.end());
  }

  void Application::end_render() {
    ImGui::PopFont();
    ImGui::End();// Dockspace window
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

    style.DisabledAlpha = 0.33F;// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

    // Set plot axis bg to transparent.
    auto &plot_style = ImPlot::GetStyle();
    plot_style.Colors[ImPlotCol_AxisBgHovered] = ImVec4(0, 0, 0, 0);
    plot_style.Colors[ImPlotCol_AxisBgActive] = ImVec4(0, 0, 0, 0);
  }

  void Application::load_fonts() {
    auto &io = ImGui::GetIO();
    io.Fonts->ClearFonts();

    m_small_font.push_back(io.Fonts->AddFontFromMemoryCompressedTTF(static_cast<const void *>(courier_code_font_compressed_data), courier_code_font_compressed_size, 10.0F));// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    m_big_font.push_back(io.Fonts->AddFontFromMemoryCompressedTTF(static_cast<const void *>(courier_code_font_compressed_data), courier_code_font_compressed_size, 42.0F));// NOLINT: cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers

    constexpr size_t NUM_FONTS = 12;
    m_default_font.reserve(NUM_FONTS);
    for(size_t i = 0; i <= NUM_FONTS; i++) {
      m_default_font.push_back(io.Fonts->AddFontFromMemoryCompressedTTF(static_cast<const void *>(courier_code_font_compressed_data), courier_code_font_compressed_size, 8.0F + static_cast<float>(i)));
    }
  }

  void Application::render_main_menu() {
    if(ImGui::BeginMainMenuBar()) {
      if(ImGui::BeginMenu("Help")) {
        if(ImGui::MenuItem("About")) {
          m_show_about = true;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  }

  void Application::render_about_box() {
    // config..
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25, 25));// NOLINT:cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
    if(ImGui::BeginPopupModal("About...", &m_show_about, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {// NOLINT: hicpp-signed-bitwise
      y44::im_text(m_big_font, y44::Align::CENTER, "MultiView");
      y44::im_text(m_small_font, y44::Align::CENTER, "v0.0.1");
      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();
      y44::im_text("I started painting as a hobby when I was little.\nI didn't know I had any talent. I believe talent is\njust a pursued interest. Anybody can do what I do.\nJust go back and put one little more happy tree in there.\nEverybody's different. Trees are different. Let them\nall be individuals. We'll put some happy little leaves\nhere and there. These things happen automatically.\nAll you have to do is just let them happen. Everyone\nwants to enjoy the good parts - but you have to build the\nframework first. Let's do that again. I'm gonna start with a\nlittle Alizarin crimson and a touch of Prussian blue.\n");
      ImGui::NewLine();
      y44::im_text("         -- Created by Fredrik Andersson");
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::EndPopup();

      if(y44::im_shortcut(ImGuiKeyModFlags_None, ImGuiKey_Escape, false)) {
        m_show_about = false;
      }
    }
    ImGui::PopStyleVar();

    // ..show it
    if(m_show_about) {
      ImGui::OpenPopup("About...", ImGuiPopupFlags_NoOpenOverExistingPopup);
    }
  }
}// namespace mv

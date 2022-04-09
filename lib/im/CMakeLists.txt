include(FetchContent)
# ImGui docking branch
FetchContent_Declare(imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG "15b4a064f9244c430e65214f7249b615fb394321"
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(implot
  GIT_REPOSITORY "https://github.com/epezent/implot.git"
  GIT_TAG "dea3387cdcc1d6a7ee3607f8a37a9dce8a85224f"
)
FetchContent_MakeAvailable(implot)

find_package(SDL2 REQUIRED)

project(im)

add_library(${PROJECT_NAME}
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer.cpp
  
  ${implot_SOURCE_DIR}/implot_demo.cpp
  ${implot_SOURCE_DIR}/implot_items.cpp
  ${implot_SOURCE_DIR}/implot.cpp
)

target_link_libraries(
  ${PROJECT_NAME} 
  SYSTEM
    spdlog::spdlog
    SDL2::SDL2
    SDL2::SDL2-static SDL2::SDL2main
)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${imgui_SOURCE_DIR}/backends)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${implot_SOURCE_DIR})

include(FetchContent)
# ImGui docking branch
FetchContent_Declare(imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG "505f19e3b00d54623566dda354980c7f3410f2b5"
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(implot
  GIT_REPOSITORY "https://github.com/epezent/implot.git"
  GIT_TAG "116024321845529bd9aacaa00216d91f15b430e2"
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
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
  
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




add_executable(${PROJECT_NAME} 
  main.cpp
  Application.cpp
  MultiViewApp.cpp
)

# https://github.com/mariusbancila/stduuid
find_package(stduuid)

target_link_libraries(
  ${PROJECT_NAME} 
  PRIVATE 
    project_options
    project_warnings
    spdlog::spdlog
    fmt::fmt
    stduuid::stduuid
    im
    )

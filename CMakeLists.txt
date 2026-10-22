cmake_minimum_required(VERSION 3.20)
project(Faithful VERSION 0.1.0 LANGUAGES CXX)
if(NOT WIN32)
  message(FATAL_ERROR "Faithful v0.1 currently targets Windows")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.9b GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(imgui)
add_executable(Faithful WIN32 src/main.cpp src/ui/Dashboard.cpp
  ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
target_include_directories(Faithful PRIVATE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends src)
target_link_libraries(Faithful PRIVATE d3d11 dxgi)
target_compile_definitions(Faithful PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)

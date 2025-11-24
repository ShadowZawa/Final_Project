# 指定Cmake版本需求
cmake_minimum_required(VERSION 3.10)

# Project名稱
project(Final_Project)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows MinGW 特殊設定
if(MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,console -mconsole")
endif()

# 尋找 FTXUI 套件
find_package(ftxui REQUIRED)

# 建立執行檔
add_executable(main main.cpp)

# 連結 FTXUI 函式庫
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
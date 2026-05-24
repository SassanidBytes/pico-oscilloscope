cmake_minimum_required(VERSION 3.16)
project(pico_oscilloscope_gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort Charts)

qt_standard_project_setup()

qt_add_executable(oscilloscope_gui
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(oscilloscope_gui PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)
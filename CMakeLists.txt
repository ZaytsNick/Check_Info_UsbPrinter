cmake_minimum_required(VERSION 3.10)
project(Check_Info_UsbPrinter)

# Указываем стандарт C++ (например, C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Отключение линковки динамических библиотек
# set(BUILD_SHARED_LIBS OFF) 

add_executable(Check_Info_UsbPrinter main.cpp)
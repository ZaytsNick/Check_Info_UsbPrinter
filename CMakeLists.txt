cmake_minimum_required(VERSION 3.10)
project(Check_Info_UsbPrinter)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Указываем стандарт C++ (например, C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/usr/include/libusb-1.0)
link_directories(/usr/x86_64-w64-mingw32/lib)

add_executable(Check_Info_UsbPrinter main.cpp)

target_link_libraries(Check_Info_UsbPrinter  libusb-1.0)
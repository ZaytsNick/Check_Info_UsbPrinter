cmake_minimum_required(VERSION 3.10)
project(Check_Info_UsbPrinter)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Указываем стандарт C++ (например, C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIBUSB_LIB_DIR "C:/libusb/MinGW64/static")
set(LIBUSB_INCLUDE_DIR "C:/libusb/include/")

add_executable(Check_Info_UsbPrinter main.cpp)


# Добавьте директорию с заголовочными файлами
target_include_directories(Check_Info_UsbPrinter PRIVATE ${LIBUSB_INCLUDE_DIR})

# Подключите библиотеку
target_link_libraries(Check_Info_UsbPrinter ${LIBUSB_LIB_DIR}/libusb-1.0.a)

set_target_properties(Check_Info_UsbPrinter PROPERTIES
    LINK_FLAGS "-static"
)
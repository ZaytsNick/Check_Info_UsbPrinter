#include <iostream>
#include <vector>
#include "libusb.h"

// Константы для endpoint'ов
constexpr uint8_t BULK_OUT_ENDPOINT = 0x01; // Endpoint для отправки данных
constexpr uint8_t BULK_IN_ENDPOINT = 0x81;  // Endpoint для получения данных

// Функция для опроса состояния тонера
bool checkTonerStatus(libusb_device_handle* printerHandle) {
    // Буфер для данных от принтера
    unsigned char buffer[64];
    int actualLength;

    // Команда для запроса состояния тонера (зависит от модели принтера)
    unsigned char command[] = "\x1B%-12345X@PJL INFO STATUS\r\n\x1B%-12345X";

    // Отправка команды принтеру
    int result = libusb_bulk_transfer(
        printerHandle,
        BULK_OUT_ENDPOINT,
        command,
        sizeof(command) - 1, // Исключаем нулевой символ из длины
        &actualLength,
        1000 // Таймаут в миллисекундах
    );

    if (result != 0) {
        std::cerr << "Error sending command: " << libusb_error_name(result) << std::endl;
        return false;
    }

    // Получение ответа от принтера
    result = libusb_bulk_transfer(
        printerHandle,
        BULK_IN_ENDPOINT,
        buffer,
        sizeof(buffer),
        &actualLength,
        1000 // Таймаут в миллисекундах
    );

    if (result != 0) {
        std::cerr << "Error while retrieving data: " << libusb_error_name(result) << std::endl;
        return false;
    }

    // Анализ ответа (зависит от модели принтера)
    if (buffer[0] == 0x12) { // Пример: 0x12 означает низкий уровень тонера
        std::cout << "Toner is almost gone!" << std::endl;
        return false;
    }
    else if (buffer[0] == 0x00) { // Пример: 0x00 означает нормальный уровень тонера
        std::cout << "The toner is normal." << std::endl;
        return true;
    }
    else {
        std::cerr << "Unknown toner status: " << std::hex << static_cast<int>(buffer[0]) << std::endl;
        return false;
    }
}

void tonerStatus(libusb_context* context, const std::pair<uint16_t, uint16_t>& deviceInfo) {
    // Поиск принтера по VID и PID
    libusb_device_handle* printerHandle = libusb_open_device_with_vid_pid(context, deviceInfo.first, deviceInfo.second);
    if (!printerHandle) {
        std::cerr << "Printer not found!" << std::endl;
        return;
    }

    // Захват интерфейса принтера
    if (libusb_claim_interface(printerHandle, 0) != 0) {
        std::cerr << "Error opening interface!" << std::endl;
        libusb_close(printerHandle);
        return;
    }

    // Проверка состояния тонера
    bool status = checkTonerStatus(printerHandle);

    // Закрытие интерфейса и освобождение ресурсов
    libusb_release_interface(printerHandle, 0);
    libusb_close(printerHandle);
}

std::vector<std::pair<uint16_t, uint16_t>> deviceList(libusb_context* context) {
    // Получение списка устройств
    libusb_device** devices;
    ssize_t count = libusb_get_device_list(context, &devices);
    if (count < 0) {
        std::cerr << "Error getting list of devices: " << libusb_error_name(count) << std::endl;
        return {};
    }

    std::vector<std::pair<uint16_t, uint16_t>> deviceInfoList;

    // Перебор всех устройств
    for (ssize_t i = 0; i < count; i++) {
        libusb_device* device = devices[i];
        libusb_device_descriptor descriptor;

        // Получение дескриптора устройства
        int result = libusb_get_device_descriptor(device, &descriptor);
        if (result < 0) {
            std::cerr << "Error getting device descriptor: " << libusb_error_name(result) << std::endl;
            continue;
        }

        // Вывод VID и PID
        std::cout << "Device #" << i + 1 << ":\n";
        std::cout << "  VID: " << std::hex << descriptor.idVendor << "\n";
        std::cout << "  PID: " << std::hex << descriptor.idProduct << "\n";
        deviceInfoList.push_back(std::make_pair(descriptor.idVendor, descriptor.idProduct));
    }

    // Освобождение списка устройств
    libusb_free_device_list(devices, 1);
    return deviceInfoList;
}

int main() {
    // Инициализация libusb
    libusb_context* context = nullptr;
    if (libusb_init(&context) != 0) {
        std::cerr << "libusb initialization error" << std::endl;
        return 1;
    }

    // Получение списка устройств
    std::vector<std::pair<uint16_t, uint16_t>> deviceInfoList = deviceList(context);
    std::cout << "Found " << deviceInfoList.size() << " devices." << std::endl;

    // Проверка состояния тонера для каждого устройства
    for (const auto& deviceInfo : deviceInfoList) {
        tonerStatus(context, deviceInfo);
    }

    // Завершение работы libusb
    libusb_exit(context);
    std::cout << "Program finished. Press any key to exit..." << std::endl;
     std::cin.get(); 
    return 0;
}

















// #include <iostream>
// #include <vector>
// #include "libusb.h"

// // Функция для опроса состояния тонера
// bool checkTonerStatus(libusb_device_handle *printerHandle){
//     // Буфер для данных от принтера
//     unsigned char buffer[64];
//     int actualLength;

//     // Команда для запроса состояния тонера (зависит от модели принтера)
//     // unsigned char command[] = {0x1D, 0x49, 0x42}; // Пример команды ESC/POS

//     unsigned char command[] = "\x1B%-12345X@PJL INFO STATUS\r\n\x1B%-12345X";
//     // Отправка команды принтеру
//     int result = libusb_bulk_transfer(
//         printerHandle,
//         0x01, // Endpoint для отправки данных
//         command,
//         sizeof(command),
//         &actualLength,
//         1000 // Таймаут в миллисекундах
//     );

//     if (result != 0)
//     {
//         std::cerr << "Error sending command: " << libusb_error_name(result) << std::endl;
//         return false;
//     }

//     // Получение ответа от принтера
//     result = libusb_bulk_transfer(
//         printerHandle,
//         0x81, // Endpoint для получения данных
//         buffer,
//         sizeof(buffer),
//         &actualLength,
//         1000 // Таймаут в миллисекундах
//     );

//     if (result != 0)
//     {
//         std::cerr << "Error while retrieving data:" << libusb_error_name(result) << std::endl;
//         return false;
//     }

//     // Анализ ответа (зависит от модели принтера)
//     if (buffer[0] == 0x12)
//     { // Пример: 0x12 означает низкий уровень тонера
//         std::cout << "Toner is almost gone!" << std::endl;
//         return false;
//     }
//     else if (buffer[0] == 0x00)
//     { // Пример: 0x00 означает нормальный уровень тонера
//         std::cout << "The toner is normal." << std::endl;
//         return true;
//     }
//     else
//     {
//         std::cerr << "Unknown toner status: " << std::hex << (int)buffer[0] << std::endl;
//         return false;
//     }
// }

// void tonerStatus(std::pair<uint16_t, uint16_t> deviceInfo){
//     // Инициализация libusb
//     libusb_context* context = nullptr;
//     if (libusb_init(&context) != 0) {
//         // std::cerr << "Ошибка инициализации libusb!" << std::endl;//libusb initialization error
//         std::cerr << "libusb initialization error" << std::endl;//
//         // return 1;
//     }
//     // Поиск принтера по VID и PID
//     libusb_device_handle* printerHandle = libusb_open_device_with_vid_pid(context, deviceInfo.first, deviceInfo.second);
//     if (!printerHandle) {
//         // std::cerr << "Принтер не найден!" << std::endl;//Printer not found!
//         std::cerr << "Printer not found!" << std::endl;//Printer not found!
//         libusb_exit(context);
//         // return 1;
//     }else  if (libusb_claim_interface(printerHandle, 0) != 0) { // Открытие интерфейса принтера
//         // std::cerr << "Ошибка при открытии интерфейса!" << std::endl;//Error opening interface!
//         std::cerr << "Error opening interface!" << std::endl;//Error opening interface!
//         libusb_close(printerHandle);
//         libusb_exit(context);
//         // return 1;
//     }

//     // Проверка состояния тонера
//     bool tonerStatus = checkTonerStatus(printerHandle);

//     // Закрытие интерфейса и освобождение ресурсов
//     libusb_release_interface(printerHandle, 0);

//     libusb_close(printerHandle);

//     libusb_exit(context);

// }

// std::vector<std::pair<uint16_t, uint16_t>> deviceList()
// {
//     // Инициализация libusb
//     libusb_context *context = nullptr;
//     int result = libusb_init(&context);
//     if (result < 0)
//     {
//         std::cerr << "Error initializing libusb: " << libusb_error_name(result) << std::endl;
//         // return 1;
//     }

//     // Получение списка устройств
//     libusb_device **devices;
//     ssize_t count = libusb_get_device_list(context, &devices);
//     if (count < 0)
//     {
//         std::cerr << "Error getting list of devices: " << libusb_error_name(count) << std::endl;
//         libusb_exit(context);
//         // return 1;
//     }
//     std::cout<<"count: "<<count<<std::endl;
//     std::vector<std::pair<uint16_t, uint16_t>> deviceInfoList;
//     // Перебор всех устройств
//     for (ssize_t i = 0; i < count; i++)
//     {
//         libusb_device *device = devices[i];
//         libusb_device_descriptor descriptor;

//         // Получение дескриптора устройства
//         result = libusb_get_device_descriptor(device, &descriptor);
//         if (result < 0)
//         {
//             std::cerr << "Error getting device descriptor: " << libusb_error_name(result) << std::endl;
//             continue;
//         }
//         // Вывод VID и PID
//         std::cout << "Device #" << i + 1 << ":\n";
//         std::cout << "  VID: " << std::hex << descriptor.idVendor << "|" << typeid(descriptor.idVendor).name() << "\n";
//         std::cout << "  PID: " << std::hex << descriptor.idProduct << "|" << typeid(descriptor.idProduct).name() << "\n";
//         deviceInfoList.push_back(std::make_pair(descriptor.idVendor, descriptor.idProduct));
//         // std::cout << "  Manufacturer: " << std::hex<< descriptor.iManufacturer << "\n";
//         // std::cout << "  Product: " << std::hex<< descriptor.iProduct << "\n";
//         // std::cout << "  Serial Number: " << std::hex<< descriptor.iSerialNumber << "\n";
//         // std::cout << "  bcdUSB: " << std::hex<< descriptor.bcdUSB << "\n";
//         std::cout << std::endl;
//     }

//     // Освобождение списка устройств
//     libusb_free_device_list(devices, 1);

//     // Завершение работы libusb
//     libusb_exit(context);
//     // Возвращаем вектор с VID и PID
//     return deviceInfoList;
// }

// int main()
// {
//     // std::cout <<"start"<<std::endl;
    
// std::vector<std::pair<uint16_t, uint16_t>> deviceInfoList=deviceList();
// std::cout<<deviceInfoList.size()<<std::endl;
//     // for (auto& deviceInfo : deviceInfoList){
//     for(int i=0;i<deviceInfoList.size();++i)
//     {

//         // std::cout<< " VID: " <<deviceInfo.first<<"    PID: "<<deviceInfo.second<<std::endl;
        
//         tonerStatus(deviceInfoList[i]);
//     }

//     std::cout << "end" << std::endl;
//     // return tonerStatus ? 0 : 1;
//     return 0;
// }
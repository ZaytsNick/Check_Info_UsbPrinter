#include <iostream>
#include <libusb.h>
#include <vector>
#include <cstring>
 // // // // Device not found (VID: 3f0, PID: 2b17)
#define HP_VENDOR_ID  0x03F0  // Vendor ID HP
#define HP_LJ1020_PID 0x2B17  // Product ID HP LaserJet 1020

const char pjl_command[] =
    "\x1B%-12345X@PJL \r\n"
    "@PJL INFO STATUS \r\n"
    "@PJL INFO PAGECOUNT \r\n"
    "@PJL INFO CONSUMABLES \r\n"
    "\x1B%-12345X";

void printHexData(const unsigned char* data, int length) {
    std::cout << "Ответ принтера (HEX):\n";
    for (int i = 0; i < length; ++i) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    libusb_context* ctx = nullptr;
    libusb_device_handle* dev_handle = nullptr;
    int result = libusb_init(&ctx);

    if (result < 0) {
        std::cerr << "Ошибка инициализации libusb!1" << std::endl;
        return 1;
    }

    dev_handle = libusb_open_device_with_vid_pid(ctx, HP_VENDOR_ID, HP_LJ1020_PID);
    if (!dev_handle) {
        std::cerr << "Ошибка: принтер HP LaserJet 1020 не найден!2" << std::endl;
        libusb_exit(ctx);
        return 1;
    }

        

    std::cout << "Принтер HP LaserJet 1020 найден!3" << std::endl;

    if (libusb_claim_interface(dev_handle, 0) < 0) {
        std::cerr << "Ошибка: не удалось захватить интерфейс USB!4" << std::endl;
        libusb_close(dev_handle);
        libusb_exit(ctx);
        return 1;
    }




    // unsigned char data[64];  // Буфер для данных, которые отправляем
    // int transferred = 0;
    // result = libusb_control_transfer(
    //     dev_handle,                           // Дескриптор устройства
    //     LIBUSB_REQUEST_TYPE_CLASS |           // Тип запроса (классовый)
    //     LIBUSB_RECIPIENT_DEVICE,              // Кому: устройству
    //     0x09,                                 // Стандартный запрос (Set Configuration или подобный)
    //     0,                                    // Значение
    //     0,                                    // Индекс
    //     (unsigned char*)pjl_command,          // Данные, которые отправляем
    //     strlen(pjl_command),                  // Размер данных
    //     1000                                  // Таймаут (в миллисекундах)
    // );
    // if (result < 0) {
    //     std::cerr << "Ошибка отправки команды PJL!" << std::endl;
    //     libusb_release_interface(dev_handle, 0);
    //     libusb_close(dev_handle);
    //     libusb_exit(ctx);
    //     return 1;
    // }
    // std::cout << "Команда PJL отправлена, читаем ответ..." << std::endl;
    // // Чтение ответа от принтера
    // unsigned char response[1024];
    // result = libusb_control_transfer(
    //     dev_handle,
    //     LIBUSB_REQUEST_TYPE_CLASS |           // Тип запроса (классовый)
    //     LIBUSB_RECIPIENT_DEVICE,              // Кому: устройству
    //     0x01,                                 // Стандартный запрос (например, Get Status)
    //     0,                                    // Значение
    //     0,                                    // Индекс
    //     response,                             // Буфер для данных
    //     sizeof(response),                     // Размер буфера
    //     2000                                  // Таймаут
    // );
    // if (result >= 0) {
    //     response[result] = '\0'; // Добавляем символ конца строки
    //     std::cout << "Ответ принтера:\n" << response << std::endl;
    //     printHexData(response, result);
    // } else {
    //     std::cerr << "Ошибка: не удалось получить ответ от принтера! 10" << libusb_error_name(result) << std::endl;
    // }





 // Запрос на количество страниц
 unsigned char pcl_pagecount[] = { 0x1B, 0x25, 0x2D, 0x49, 0x4E, 0x46, 0x4F, 0x20, 0x50, 0x41, 0x47, 0x45, 0x43, 0x4F, 0x55, 0x4E, 0x54, 0x0D, 0x0A };
    
 // Запрос на информацию о расходных материалах
 unsigned char pcl_consumables[] = { 0x1B, 0x25, 0x2D, 0x49, 0x4E, 0x46, 0x4F, 0x20, 0x43, 0x4F, 0x4E, 0x53, 0x55, 0x4D, 0x41, 0x42, 0x4C, 0x45, 0x53, 0x0D, 0x0A };

    int transferred;
 // Отправляем команду для получения количества страниц
  result = libusb_bulk_transfer(dev_handle, 0x81, pcl_pagecount, sizeof(pcl_pagecount), &transferred, 1000);
 if (result != 0) {
     std::cerr << "Ошибка отправки команды PageCount!" << std::endl;
 } else {
    std::cout << "Команда PCL для получения количества страниц отправлена.11" << std::endl;
 }

 // Отправляем команду для получения информации о расходных материалах (тонере)
 result = libusb_bulk_transfer(dev_handle, 0x81, pcl_consumables, sizeof(pcl_consumables), &transferred, 1000);
 if (result != 0) {
    std::cerr << "Ошибка отправки команды Consumables!" << std::endl;
 } else {
    std::cout << "Команда PCL для получения состояния расходных материалов отправлена.11" << std::endl;
 }

 // Прочитаем ответ от принтера (предполагаем, что это данные о количестве страниц и уровне тонера)
 unsigned char response[256];  // Массив для хранения ответа принтера
 result = libusb_bulk_transfer(dev_handle, 0x01, response, sizeof(response), &transferred, 10000);

 if (result == 0) {
     // Выводим ответ принтера (простой вывод для примера)
     std::cout << "Ответ от принтера:" << std::endl;
     for (int i = 0; i < transferred; ++i) {
         printf("%02X ", response[i]);
     }
     std::cout << std::endl;
 } else {
    std::cerr << "Ошибка получения данных от принтера!22" << std::endl;
 }




    // int transferred;
    // result = libusb_bulk_transfer(dev_handle, 0x01, (unsigned char*)pjl_command, strlen(pjl_command), &transferred, 1000);
    // if (result != 0) {
    //     std::cerr << "Ошибка отправки команды PJL!5" << std::endl;
    //     libusb_release_interface(dev_handle, 0);
    //     libusb_close(dev_handle);
    //     libusb_exit(ctx);
    //     return 1;
    // }

    // std::cout << "Команда PJL отправлена, читаем ответ...6" << std::endl;

    // unsigned char response[1024];
    // result = libusb_bulk_transfer(dev_handle, 0x81, response, sizeof(response), &transferred, 2000);

    // if (result == 0 && transferred > 0) {
    //     response[transferred] = '\0';
    //     std::cout << "Ответ принтера:\n" << response << std::endl;
    //     printHexData(response, transferred);
    // } else {
    //     std::cerr << "Ошибка: не удалось получить ответ от принтера!7"<<libusb_error_name(result) << std::endl;
    // }

    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    std::cout << "Program finished. Press any key to exit...\n";
    std::cin.get();

    return 0;
}



















// #include <iostream>
// #include <vector>
// #include "libusb.h"

// // Константы для endpoint'ов
// constexpr uint8_t BULK_OUT_ENDPOINT = 0x01; // Endpoint для отправки данных
// constexpr uint8_t BULK_IN_ENDPOINT = 0x81;  // Endpoint для получения данных

// // Функция для взаимодействия с устройством
// void interactWithDevice(libusb_context* ctx, const std::pair<uint16_t, uint16_t>& dev) {
//     libusb_device_handle* handle = libusb_open_device_with_vid_pid(ctx, dev.first, dev.second);
//     if (!handle) {
//         std::cerr << "Device not found (VID: " << std::hex << dev.first << ", PID: " << dev.second << ")\n";
//         return;
//     }

//     // Захват интерфейса
//     if (libusb_claim_interface(handle, 0) != 0) {
//         std::cerr << "Error claiming interface\n";
//         libusb_close(handle);
//         return;
//     }

//     // Команда PJL
//     std::string pjlCommand = "\x1B%-12345X@PJL INFO STATUS\r\n\x1B%-12345X\r\n";
//     unsigned char buffer[1024];
//     int actualLength;

//     // Отправка команды
//     int result = libusb_bulk_transfer(
//         handle,
//         BULK_OUT_ENDPOINT,
//         reinterpret_cast<unsigned char*>(const_cast<char*>(pjlCommand.c_str())),
//         pjlCommand.size(),
//         &actualLength,
//         5000 // Увеличенный таймаут
//     );

//     if (result != 0) {
//         std::cerr << "Error sending command: " << libusb_error_name(result) << "\n";
//         libusb_release_interface(handle, 0);
//         libusb_close(handle);
//         return;
//     }

//     // Получение ответа
//     result = libusb_bulk_transfer(
//         handle,
//         BULK_IN_ENDPOINT,
//         buffer,
//         sizeof(buffer),
//         &actualLength,
//         5000 // Увеличенный таймаут
//     );

//     if (result != 0) {
//         std::cerr << "Error receiving data: " << libusb_error_name(result) << "\n";
//     } else {
//         std::cout << "Response from printer (" << actualLength << " bytes):\n";
//         std::cout << std::string(buffer, buffer + actualLength) << "\n";
//     }

//     // Освобождение интерфейса и закрытие устройства
//     libusb_release_interface(handle, 0);
//     libusb_close(handle);
// }

// // Функция для получения списка устройств
// std::vector<std::pair<uint16_t, uint16_t>> getDeviceList(libusb_context* ctx) {
//     libusb_device** devices;
//     ssize_t count = libusb_get_device_list(ctx, &devices);
//     if (count < 0) {
//         std::cerr << "Error getting device list: " << libusb_error_name(count) << "\n";
//         return {};
//     }
//     // // // // Device not found (VID: 3f0, PID: 2b17)
//     std::vector<std::pair<uint16_t, uint16_t>> deviceList;
//     for (ssize_t i = 0; i < count; i++) {
//         libusb_device_descriptor desc;
//         if (libusb_get_device_descriptor(devices[i], &desc) == 0) {
//             std::cout << "Device #" << i + 1 << ":\n";
//             std::cout << "  VID: " << std::hex << desc.idVendor << "\n";
//             std::cout << "  PID: " << std::hex << desc.idProduct << "\n";
//             std::cout << "  bcdDevice: " << std::hex << desc.bcdDevice << "\n";
//             std::cout << "  bcdUSB: " << std::hex << desc.bcdUSB << "\n";
//             std::cout << "  iManufacturer: " << std::hex << desc.iManufacturer << "\n";
//             std::cout << "  iProduct: " << std::hex << desc.iProduct<< "\n";
//             // // std::cout << "  bcdUSB: " << std::hex << desc.bcdUSB << "\n";
//             // std::cout << "  bcdUSB: " << std::hex << desc.bcdUSB << "\n";
//             // std::cout << "  bcdUSB: " << std::hex << desc.bcdUSB << "\n";
//             deviceList.push_back({desc.idVendor, desc.idProduct});
//         }
//     }

//     libusb_free_device_list(devices, 1);
//     return deviceList;
// }

// int main() {
//     libusb_context* ctx = nullptr;
//     if (libusb_init(&ctx) != 0) {
//         std::cerr << "Failed to initialize libusb\n";
//         return 1;
//     }

//     // Получение списка устройств
//     auto devices = getDeviceList(ctx);
//     std::cout << "Found " << devices.size() << " devices.\n";

//     // Взаимодействие с каждым устройством
//     for (const auto& dev : devices) {
//         interactWithDevice(ctx, dev);
//     }

//     libusb_exit(ctx);
//     std::cout << "Program finished. Press any key to exit...\n";
//     std::cin.get();
//     return 0;
// }






















// #include <iostream>
// #include <vector>
// #include "libusb.h"

// // Константы для endpoint'ов
// // constexpr uint8_t BULK_OUT_ENDPOINT = 0x01; // Endpoint для отправки данных
// // constexpr uint8_t BULK_IN_ENDPOINT = 0x81;  // Endpoint для получения данных

// // Функция для опроса состояния тонера
// bool checkTonerStatus(libusb_device_handle* printerHandle) {
//     // Буфер для данных от принтера
//     unsigned char buffer[64];
//     int actualLength;

//     // Команда для запроса состояния тонера (зависит от модели принтера)
//     unsigned char command[] = "\x1B%-12345X@PJL INFO STATUS\r\n\x1B%-12345X";

//     // Отправка команды принтеру
//     int result = libusb_bulk_transfer(
//         printerHandle,
//         BULK_OUT_ENDPOINT,
//         command,
//         sizeof(command) - 1, // Исключаем нулевой символ из длины
//         &actualLength,
//         1000 // Таймаут в миллисекундах
//     );

//     if (result != 0) {
//         std::cerr << "Error sending command: " << libusb_error_name(result) << std::endl;
//         return false;
//     }

//     // Получение ответа от принтера
//     result = libusb_bulk_transfer(
//         printerHandle,
//         BULK_IN_ENDPOINT,
//         buffer,
//         sizeof(buffer),
//         &actualLength,
//         1000 // Таймаут в миллисекундах
//     );

//     if (result != 0) {
//         std::cerr << "Error while retrieving data: " << libusb_error_name(result) << std::endl;
//         return false;
//     }

//     // Анализ ответа (зависит от модели принтера)
//     if (buffer[0] == 0x12) { // Пример: 0x12 означает низкий уровень тонера
//         std::cout << "Toner is almost gone!" << std::endl;
//         return false;
//     }
//     else if (buffer[0] == 0x00) { // Пример: 0x00 означает нормальный уровень тонера
//         std::cout << "The toner is normal." << std::endl;
//         return true;
//     }
//     else {
//         std::cerr << "Unknown toner status: " << std::hex << static_cast<int>(buffer[0]) << std::endl;
//         return false;
//     }
// }

// void tonerStatus(libusb_context* context, const std::pair<uint16_t, uint16_t>& deviceInfo) {
//     // Поиск принтера по VID и PID
//     libusb_device_handle* printerHandle = libusb_open_device_with_vid_pid(context, deviceInfo.first, deviceInfo.second);
//     if (!printerHandle) {
//         std::cerr << "Printer not found!" << std::endl;
//         return;
//     }

//     // Захват интерфейса принтера
//     if (libusb_claim_interface(printerHandle, 0) != 0) {
//         std::cerr << "Error opening interface!" << std::endl;
//         libusb_close(printerHandle);
//         return;
//     }

//     // Проверка состояния тонера
//     bool status = checkTonerStatus(printerHandle);

//     // Закрытие интерфейса и освобождение ресурсов
//     libusb_release_interface(printerHandle, 0);
//     libusb_close(printerHandle);
// }

// std::vector<std::pair<uint16_t, uint16_t>> deviceList(libusb_context* context) {
//     // Получение списка устройств
//     libusb_device** devices;
//     ssize_t count = libusb_get_device_list(context, &devices);
//     if (count < 0) {
//         std::cerr << "Error getting list of devices: " << libusb_error_name(count) << std::endl;
//         return {};
//     }

//     std::vector<std::pair<uint16_t, uint16_t>> deviceInfoList;

//     // Перебор всех устройств
//     for (ssize_t i = 0; i < count; i++) {
//         libusb_device* device = devices[i];
//         libusb_device_descriptor descriptor;

//         // Получение дескриптора устройства
//         int result = libusb_get_device_descriptor(device, &descriptor);
//         if (result < 0) {
//             std::cerr << "Error getting device descriptor: " << libusb_error_name(result) << std::endl;
//             continue;
//         }

//         // Вывод VID и PID
//         std::cout << "Device #" << i + 1 << ":\n";
//         std::cout << "  VID: " << std::hex << descriptor.idVendor << "\n";
//         std::cout << "  PID: " << std::hex << descriptor.idProduct << "\n";
//         deviceInfoList.push_back(std::make_pair(descriptor.idVendor, descriptor.idProduct));
//     }

//     // Освобождение списка устройств
//     libusb_free_device_list(devices, 1);
//     return deviceInfoList;
// }

// int main() {
//     // Инициализация libusb
//     libusb_context* context = nullptr;
//     if (libusb_init(&context) != 0) {
//         std::cerr << "libusb initialization error" << std::endl;
//         return 1;
//     }

//     // Получение списка устройств
//     std::vector<std::pair<uint16_t, uint16_t>> deviceInfoList = deviceList(context);
//     std::cout << "Found " << deviceInfoList.size() << " devices." << std::endl;

//     // Проверка состояния тонера для каждого устройства
//     for (const auto& deviceInfo : deviceInfoList) {
//         tonerStatus(context, deviceInfo);
//     }

//     // Завершение работы libusb
//     libusb_exit(context);
//     std::cout << "Program finished. Press \"Enter\" to exit..." << std::endl;
//      std::cin.get(); 
//     return 0;
// }
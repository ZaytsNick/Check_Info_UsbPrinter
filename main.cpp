#include <iostream>
// #include <usb.h>
#include <libusb-1.0/libusb.h>

// Функция для опроса состояния тонера
bool checkTonerStatus(libusb_device_handle* printerHandle) {
    // Буфер для данных от принтера
    unsigned char buffer[64];
    int actualLength;

    // Команда для запроса состояния тонера (зависит от модели принтера)
    unsigned char command[] = {0x1D, 0x49, 0x42}; // Пример команды ESC/POS

    // Отправка команды принтеру
    int result = libusb_bulk_transfer(
        printerHandle,
        0x01, // Endpoint для отправки данных
        command,
        sizeof(command),
        &actualLength,
        1000 // Таймаут в миллисекундах
    );

    if (result != 0) {
        std::cerr << "Ошибка при отправке команды: " << libusb_error_name(result) << std::endl;
        return false;
    }

    // Получение ответа от принтера
    result = libusb_bulk_transfer(
        printerHandle,
        0x81, // Endpoint для получения данных
        buffer,
        sizeof(buffer),
        &actualLength,
        1000 // Таймаут в миллисекундах
    );

    if (result != 0) {
        std::cerr << "Ошибка при получении данных: " << libusb_error_name(result) << std::endl;
        return false;
    }

    // Анализ ответа (зависит от модели принтера)
    if (buffer[0] == 0x12) { // Пример: 0x12 означает низкий уровень тонера
        std::cout << "Тонер почти закончился!" << std::endl;
        return false;
    } else if (buffer[0] == 0x00) { // Пример: 0x00 означает нормальный уровень тонера
        std::cout << "Тонер в норме." << std::endl;
        return true;
    } else {
        std::cerr << "Неизвестный статус тонера: " << std::hex << (int)buffer[0] << std::endl;
        return false;
    }
}

int main() {
    // Инициализация libusb
    libusb_context* context = nullptr;
    if (libusb_init(&context) != 0) {
        std::cerr << "Ошибка инициализации libusb!" << std::endl;
        return 1;
    }

    // Поиск принтера по VID и PID
    libusb_device_handle* printerHandle = libusb_open_device_with_vid_pid(context, 0x1234, 0x5678);
    if (!printerHandle) {
        std::cerr << "Принтер не найден!" << std::endl;
        libusb_exit(context);
        return 1;
    }

    // Открытие интерфейса принтера
    if (libusb_claim_interface(printerHandle, 0) != 0) {
        std::cerr << "Ошибка при открытии интерфейса!" << std::endl;
        libusb_close(printerHandle);
        libusb_exit(context);
        return 1;
    }

    // Проверка состояния тонера
    bool tonerStatus = checkTonerStatus(printerHandle);

    // Закрытие интерфейса и освобождение ресурсов
    libusb_release_interface(printerHandle, 0);
    libusb_close(printerHandle);
    libusb_exit(context);

    return tonerStatus ? 0 : 1;
}
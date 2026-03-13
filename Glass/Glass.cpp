#include <iostream>
#include <Windows.h>
#include <string>

int main(int argc, char* argv[])
{
    setlocale(0, "rus");

    int num = 1; // Значение по умолчанию на случай запуска без аргументов
    if (argc >= 2) {
        num = std::atoi(argv[1]);
        std::string title = "Naperstki_" + std::to_string(num);
        SetConsoleTitleA(title.c_str());
    }

    // Даем ОС миллисекунду на применение заголовка к окну консоли
    Sleep(50);

    // Открываем нужное событие и подаем сигнал главному окну
    std::string eventName = "GlassReadyEvent_" + std::to_string(num);
    HANDLE hEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, eventName.c_str());
    if (hEvent) {
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }

    Sleep(200);

    SetConsoleTitleA("Glass");

    HANDLE Mutex = OpenMutexA(SYNCHRONIZE, FALSE, "Naperstki");
    if (!Mutex)
    {
        std::cout << "не открылся" <<GetLastError()<< std::endl;
        Sleep(5000);
    }
    


    char map[20][20] = {
        "                   ",
        "     ________      ",
        "    /         \\    ",
        "   /           \\   ",
        "  /             \\  ",
        " /               \\ ",
        "/                 \\",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "|                 |",
        "-------------------"


    };
    for (int i = 0; i < 19; i++) {
        std::cout << map[i] << std::endl;
    
}

    //Sleep(2000);


    //SetConsoleTitleA("thimble");
    ////std::cin.get();

    WaitForSingleObject(Mutex, INFINITE);
    CloseHandle(Mutex);

    return 0;
}

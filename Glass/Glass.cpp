#include <iostream>
#include <Windows.h>
#include <string>

int main(int argc, char* argv[])
{

    setlocale(0, "rus");

    if (argc >= 2) {
        int num = std::atoi(argv[1]);
        std::string title = "Naperstki_" + std::to_string(num);
        SetConsoleTitleA(title.c_str());
    }
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

    Sleep(2000);


    SetConsoleTitleA("thimble");
    //std::cin.get();

    WaitForSingleObject(Mutex, INFINITE);
    CloseHandle(Mutex);

    return 0;
}

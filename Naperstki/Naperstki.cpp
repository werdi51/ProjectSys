#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>	


#include <io.h>
#include <fcntl.h>

//https://www.codeincodeblock.com/2011/03/move-console-windows-using-codeblock.html
//																										ANSI ↔ Unicode

using namespace std;
static int CountGlasses = 3;
HWND Ball;

void CreateGlasses()
{
	STARTUPINFOA Bsi = { sizeof(Bsi) };
	PROCESS_INFORMATION Bpi;

	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi[3];
	vector<HWND> hWnds(3);

	string appPath = "D:\\система прогр\\ProjectSys-master\\Naperstki\\x64\\Debug\\Glass.exe";
	string BallPath = "D:\\система прогр\\ProjectSys-master\\Naperstki\\x64\\Debug\\Ball.exe";

	if (appPath == "D:\\система прогр\\ProjectSys-master\\Naperstki\\x64\\Debug\\Glass.exze")
	{
		cout << "вставь свой путь к процессу стакана" << endl;
	}
	else
	{
		for (int i = 0; i < CountGlasses; i++) {

			std::string cmdLine = appPath + " " + std::to_string(i + 1);

			BOOL success = CreateProcessA(
				NULL,                   // Имя модуля (можно NULL, если указано в командной строке)
				(LPSTR)cmdLine.c_str(), // Полная командная строка
				NULL,                   // Атрибуты безопасности процесса
				NULL,                   // Атрибуты безопасности потока
				FALSE,                  // Не наследовать дескрипторы
				CREATE_NEW_CONSOLE,      // Создать новое окно консоли
				NULL,                   // Использовать окружение родителя
				NULL,                   // Текущая директория
				&si,                    // STARTUPINFO
				&pi[i]                  // PROCESS_INFORMATION
			);

			if (!success) {
				std::cerr << "Failed to create process " << i + 1 << std::endl;
			}
		}
		Sleep(1000);

		std::string cmdLine = BallPath + " " + std::to_string(1);

		BOOL success = CreateProcessA(
			NULL,                   // Имя модуля (можно NULL, если указано в командной строке)
			(LPSTR)cmdLine.c_str(), // Полная командная строка
			NULL,                   // Атрибуты безопасности процесса
			NULL,                   // Атрибуты безопасности потока
			FALSE,                  // Не наследовать дескрипторы
			CREATE_NEW_CONSOLE,      // Создать новое окно консоли
			NULL,                   // Использовать окружение родителя
			NULL,                   // Текущая директория
			&Bsi,                    // STARTUPINFO
			&Bpi                  // PROCESS_INFORMATION
		);
		if (!success) {
			std::cerr << "Failed to create process b" << 1 << std::endl;
		}
		Sleep(1000);

		for (int i = 0; i < CountGlasses; i++) {
			std::string title = "Naperstki_" + std::to_string(i + 1);
			hWnds[i] = FindWindowA(NULL, title.c_str());
			if (hWnds[i] == NULL) {
				std::cerr << "Window " << i + 1 << " not found" << std::endl;
			}
			else {
				std::cout << "Window " << i + 1 << " found" << std::endl;
			}
		}

		std::string title = "Ball";
		Ball = FindWindowA(NULL, title.c_str());
		if (Ball == NULL) {
			std::cerr << "Window Ball not found" << std::endl;
		}
		else {
			std::cout << "Window Ball found" << std::endl;
		}

		//ПАРАМЕТРЫ ДЛЯ SetWindowPos

		// РАЗМЕР ОКНА
		//ПЕРВОЕ-ПУТЬ
		//ВТОРОЕ-ГЛУБИНА ОКНА (ОСЬ Z)
		//ТРЕТЬЕ-пОЗИЦИЯ ПО X ОСИ
		//ЧЕТВЕРТОЕ- ПОЗИЦИЯ ПО Y ОСИ (ВЫСОТЕ)
		//ПЯТОЕ-ШИРИНА ОКНА
		//ШЕСТОЕ-ВЫСОТА ОКНА
		//СЕДЬМОЕ-КАКИЕ НИБУДЬ ЗАПРЕТЫ (СТОИТ ЗАПРЕТ НА ИЗМЕНЕНИЕ Z ОСИ ИЗ ЗА ЧЕГО 2 ПАРАМЕТР ПУСТОЕ Т.К. НЕ НУЖЕН)

		int glass2X = 0, glass2Y = 0;//хранение позиции второго сткана
		int glassHeight = 400;//высота окна стакана

		//позиционирование других стаканов 
		for (int i = 0; i < CountGlasses; i++) {
			if (hWnds[i]) {
				int x = 150 + i * 500;
				int y = 300;
				SetWindowPos(hWnds[i], HWND_TOP, x, y, 200, glassHeight, SWP_SHOWWINDOW);

				//сохранение позиции второго стакана
				if (i == 1) {
					glass2X = x;
					glass2Y = y;
				}
			}
		}

		//размер шарика
		int ballWidth = 200;
		int ballHeight = 200;

		//позиционирование шарика за вторым стаканом
		if (Ball != NULL && glass2X != 0) {
			SetWindowPos(Ball, HWND_BOTTOM,//помещаем окно шарика под второй стакан
				glass2X + 30,//делаем шарик по центру стакана
				glass2Y + 125,//опускаем шарик вниз, чтобы он был в нижней части стакана
				ballWidth, ballHeight,
				SWP_SHOWWINDOW);
		}

		//НАЧАЛЬНАЯ АНИМАЦИЯ 

		if (hWnds[1] != NULL && Ball != NULL) {
			Sleep(1000);

			//поднимаем второй стакан вверх
			for (int i = 0; i < 40; i++) {
				SetWindowPos(hWnds[1], HWND_TOP,
					glass2X,
					glass2Y - i * 5,//постепенно поднимаем вверх
					200, glassHeight,
					SWP_SHOWWINDOW);
				Sleep(15);
			}

			//поднимаем шарик чуть выше второго стакана
			for (int i = 0; i < 10; i++) {
				SetWindowPos(Ball, HWND_TOP,//временно поднимаем шарик поверх второго
					glass2X + 25,
					glass2Y + 125 - i * 5,
					ballWidth, ballHeight,
					SWP_SHOWWINDOW);
				Sleep(10);
			}
			Sleep(1500);



			//опускаем второй стакан вниз
			for (int i = 39; i >= 0; i--) {
				SetWindowPos(hWnds[1], HWND_TOP,
					glass2X,
					glass2Y - i * 5,//постепенно опускаем вниз
					200, glassHeight,
					SWP_SHOWWINDOW);
				Sleep(15);
			}

			SetWindowPos(hWnds[1], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(Ball, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		HANDLE hProcesses[3];
		for (int i = 0; i < CountGlasses; i++) {
			hProcesses[i] = pi[i].hProcess;
		}
		WaitForMultipleObjects(3, hProcesses, TRUE, INFINITE);

		for (int i = 0; i < CountGlasses; i++) {
			CloseHandle(pi[i].hProcess);
			CloseHandle(pi[i].hThread);
		}
	}
}

int main()
{
	setlocale(0, "rus");
	cout << "          ИГРА В НАПЕРСТКИ" << endl;
	cout << endl;
	cout << "правила игры: " << endl;
	cout << "шарик находится под одним из наперстков" << endl;
	cout << "ваша задача угадать, под каким" << endl;
	cout << endl;
	cout << "нажмите enter, чтобы начать игру" << endl;
	cin.get();
	CreateGlasses();
	return 0;
}

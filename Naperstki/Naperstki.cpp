#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>	
#include <ctime>  
#include <cstdlib> 

#include <io.h>
#include <fcntl.h>

using namespace std;

static int CountGlasses = 3;
HWND Ball;
int ballUnderGlassIndex = 1;


void ShuffleGlasses(vector<HWND>& hWnds) {
	srand(static_cast<unsigned int>(time(0)));

	int shuffleCount = 5; 

	for (int s = 0; s < shuffleCount; s++) {
		// выбираем два случайных разных стакана
		int idx1 = rand() % 3;
		int idx2 = rand() % 3;
		while (idx1 == idx2) {
			idx2 = rand() % 3;
		}

		// текущие позиции окон
		RECT rect1, rect2;
		GetWindowRect(hWnds[idx1], &rect1);
		GetWindowRect(hWnds[idx2], &rect2);

		int x1 = rect1.left, y1 = rect1.top;
		int x2 = rect2.left, y2 = rect2.top;

		int steps = 10;
		int dx = (x2 - x1) / steps;

		for (int i = 0; i <= steps; i++) {
			SetWindowPos(hWnds[idx1], HWND_TOP, x1 + dx * i, y1, 200, 400, SWP_SHOWWINDOW);
			SetWindowPos(hWnds[idx2], HWND_TOP, x2 - dx * i, y2, 200, 400, SWP_SHOWWINDOW);
			Sleep(20);
		}

		swap(hWnds[idx1], hWnds[idx2]);

		// обновляем индекс мяча
		if (ballUnderGlassIndex == idx1) {
			ballUnderGlassIndex = idx2;
		}
		else if (ballUnderGlassIndex == idx2) {
			ballUnderGlassIndex = idx1;
		}

		Sleep(150);
	}
}

void CreateGlasses()
{
	STARTUPINFOA Bsi = { sizeof(Bsi) };
	PROCESS_INFORMATION Bpi;

	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi[3];
	vector<HWND> hWnds(3);

	string appPath = "Glass.exe";
	string BallPath = "Ball.exe";

	for (int i = 0; i < CountGlasses; i++) {
		std::string cmdLine = appPath + " " + std::to_string(i + 1);

		BOOL success = CreateProcessA(
			NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi[i]);

		if (!success) {
			std::cerr << "Failed to create process " << i + 1 << std::endl;
		}
	}
	Sleep(1000);

	std::string cmdLine = BallPath + " " + std::to_string(1);
	BOOL success = CreateProcessA(
		NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &Bsi, &Bpi);

	if (!success) {
		std::cerr << "Failed to create process Ball" << std::endl;
	}
	Sleep(1000);

	// Поиск окон стаканов
	for (int i = 0; i < CountGlasses; i++) {
		std::string title = "Naperstki_" + std::to_string(i + 1);
		hWnds[i] = FindWindowA(NULL, title.c_str());
	}

	// Поиск окна мяча
	Ball = FindWindowA(NULL, "Ball");

	// Расстановка стаканов
	int glass2X = 0, glass2Y = 0;
	int glassHeight = 400;

	for (int i = 0; i < CountGlasses; i++) {
		if (hWnds[i]) {
			int x = 150 + i * 500;
			int y = 300;
			SetWindowPos(hWnds[i], HWND_TOP, x, y, 200, glassHeight, SWP_SHOWWINDOW);
			if (i == 1) {
				glass2X = x;
				glass2Y = y;
			}
		}
	}

	// Позиционирование мяча под вторым стаканом (индекс 1)
	if (Ball != NULL && glass2X != 0) {
		SetWindowPos(Ball, HWND_BOTTOM, glass2X + 30, glass2Y + 125, 200, 200, SWP_SHOWWINDOW);
	}

	// === НАЧАЛЬНАЯ АНИМАЦИЯ (показываем где мяч) ===
	if (hWnds[1] != NULL && Ball != NULL) {
		Sleep(1000);

		// Поднимаем стакан
		for (int i = 0; i < 40; i++) {
			SetWindowPos(hWnds[1], HWND_TOP, glass2X, glass2Y - i * 5, 200, glassHeight, SWP_SHOWWINDOW);
			Sleep(15);
		}
		// Показываем мяч
		for (int i = 0; i < 10; i++) {
			SetWindowPos(Ball, HWND_TOP, glass2X + 25, glass2Y + 125 - i * 5, 200, 200, SWP_SHOWWINDOW);
			Sleep(10);
		}
		Sleep(1500);

		// Опускаем стакан
		for (int i = 39; i >= 0; i--) {
			SetWindowPos(hWnds[1], HWND_TOP, glass2X, glass2Y - i * 5, 200, glassHeight, SWP_SHOWWINDOW);
			Sleep(15);
		}
	}


	if (Ball != NULL) {
		ShowWindow(Ball, SW_HIDE);
	}

	ShuffleGlasses(hWnds);

	if (Ball != NULL) {
		RECT rect;
		GetWindowRect(hWnds[ballUnderGlassIndex], &rect);
		SetWindowPos(Ball, HWND_BOTTOM, rect.left + 30, rect.top + 125, 200, 200, SWP_SHOWWINDOW);
		ShowWindow(Ball, SW_SHOW); 
	}

	cout << "\nПод каким стаканом мяч? Введите число (1-3): ";
	int userGuess;
	cin >> userGuess;
	userGuess--;

	// <-- НОВОЕ: ПРОВЕРЯЕМ ОТВЕТ
	if (userGuess == ballUnderGlassIndex) {
		cout << "ВЫ УГАДАЛИ! ПОБЕДА!" << endl;
	}
	else {
		cout << "НЕВЕРНО! Мяч был под стаканом " << (ballUnderGlassIndex + 1) << endl;
	}

	// Ожидание завершения процессов
	HANDLE hProcesses[3];
	for (int i = 0; i < CountGlasses; i++) {
		hProcesses[i] = pi[i].hProcess;
	}
	WaitForMultipleObjects(3, hProcesses, TRUE, INFINITE);

	// Очистка ресурсов
	for (int i = 0; i < CountGlasses; i++) {
		CloseHandle(pi[i].hProcess);
		CloseHandle(pi[i].hThread);
	}
}

int main()
{
	setlocale(0, "rus");
	cout << "          ИГРА В НАПЕРСТКИ" << endl;
	cout << endl;
	cout << "Правила игры:" << endl;
	cout << "1. Шарик находится под одним из наперстков" << endl;
	cout << "2. Запомните, под каким" << endl;
	cout << "3. Когда стаканы перемешаются - угадайте где мяч" << endl;
	cout << endl;
	cout << "Нажмите Enter, чтобы начать игру..." << endl;
	cin.get();

	CreateGlasses();
	return 0;
}
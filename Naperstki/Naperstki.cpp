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

// ========== GLOBAL ==========

const int GLASS_WIDTH = 620;      // ширина стакана
const int GLASS_HEIGHT = 500;     // высота стакана
const int BALL_WIDTH = 400;        // ширина мяча
const int BALL_HEIGHT = 300;       // высота мяча
const int GAP_BETWEEN = 10;       // зазор между стаканами
const int START_X = 150;           // начальная позиция первого стакана по X
const int START_Y = 300;           // позиция по Y для всех стаканов
const int BALL_Y_OFFSET = 125;     // смещение мяча относительно верха стакана 

// ==============================

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
            SetWindowPos(hWnds[idx1], HWND_TOP, x1 + dx * i, y1, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            SetWindowPos(hWnds[idx2], HWND_TOP, x2 - dx * i, y2, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
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

void StartGame() {
    STARTUPINFOA Bsi = { sizeof(Bsi) };
    PROCESS_INFORMATION Bpi;

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi[3];
    vector<HWND> hWnds(3);

    string appPath = "Glass.exe";
    string BallPath = "Ball.exe";

    // Создание процессов стаканов
    for (int i = 0; i < CountGlasses; i++) {
        string cmdLine = appPath + " " + to_string(i + 1);
        BOOL success = CreateProcessA(
            NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi[i]);
        if (!success) {
            cerr << "Failed to create process " << i + 1 << endl;
        }
    }
    Sleep(100);

    // Создание процесса мяча
    string cmdLine = BallPath + " " + to_string(1);
    BOOL success = CreateProcessA(
        NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &Bsi, &Bpi);
    if (!success) {
        cerr << "Failed to create process Ball" << endl;
    }
    Sleep(1000);

    // Поиск окон стаканов
    for (int i = 0; i < CountGlasses; i++) {
        string title = "Naperstki_" + to_string(i + 1);
        hWnds[i] = FindWindowA(NULL, title.c_str());
    }

    // Поиск окна мяча
    Ball = FindWindowA(NULL, "Ball");

    // Расстановка стаканов с равными промежутками
    int glass2X = 0, glass2Y = 0;
    for (int i = 0; i < CountGlasses; i++) {
        if (hWnds[i]) {
            int x = START_X + i * (GLASS_WIDTH + GAP_BETWEEN);
            int y = START_Y;
            SetWindowPos(hWnds[i], HWND_TOP, x, y, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            if (i == 1) {
                glass2X = x;
                glass2Y = y;
            }
        }
    }
    // Позиционирование мяча под вторым стаканом (индекс 1)
    if (Ball != NULL && glass2X != 0) {
        int ballX = glass2X + (GLASS_WIDTH - BALL_WIDTH) / 2;
        int ballY = glass2Y + BALL_Y_OFFSET;
        SetWindowPos(Ball, HWND_BOTTOM, ballX, ballY, BALL_WIDTH, BALL_HEIGHT, SWP_SHOWWINDOW);
    }

    // === НАЧАЛЬНАЯ АНИМАЦИЯ (показываем где мяч) ===
    if (hWnds[1] != NULL && Ball != NULL) {
        Sleep(1000);

        // Поднимаем стакан
        for (int i = 0; i < 70; i++) {
            SetWindowPos(hWnds[1], HWND_TOP, glass2X, glass2Y - i * 5, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            Sleep(15);
        }
        // Показываем мяч (поднимаем его чуть выше)
        for (int i = 0; i < 10; i++) {
            int ballX = glass2X + (GLASS_WIDTH - BALL_WIDTH) / 2;
            int ballY = glass2Y + BALL_Y_OFFSET;
            SetWindowPos(Ball, HWND_TOP, ballX, ballY - i * 5, BALL_WIDTH, BALL_HEIGHT, SWP_SHOWWINDOW);
            Sleep(10);
        }
        Sleep(1500);

        // Опускаем стакан обратно
        for (int i = 69; i >= 0; i--) {
            SetWindowPos(hWnds[1], HWND_TOP, glass2X, glass2Y - i * 5, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            Sleep(15);
        }
    }

    // Прячем мяч перед перемешиванием
    if (Ball != NULL) {
        ShowWindow(Ball, SW_HIDE);
    }

    // Перемешивание стаканов
    ShuffleGlasses(hWnds);

    // Показываем мяч под тем стаканом, где он оказался
    if (Ball != NULL) {

        RECT rect;
        GetWindowRect(hWnds[ballUnderGlassIndex], &rect);

        int ballX = rect.left + (GLASS_WIDTH - BALL_WIDTH) / 2;
        int ballY = rect.top + BALL_Y_OFFSET;

        SetWindowPos(Ball, HWND_BOTTOM, ballX, ballY, BALL_WIDTH, BALL_HEIGHT, SWP_SHOWWINDOW);
        ShowWindow(Ball, SW_SHOW);
    }













    //--------------------------------------------

    int score = 50;// стартовые очки
    bool firstRound = true;     
    cout << "Текущий счёт: " << score << endl;

    while (score > 0 && score < 100) {
        if (!firstRound) {
            ShowWindow(Ball, SW_HIDE);
            ShuffleGlasses(hWnds);

            if (Ball != NULL) {

                RECT rect;

                GetWindowRect(hWnds[ballUnderGlassIndex], &rect);

                int ballX = rect.left + (GLASS_WIDTH - BALL_WIDTH) / 2;

                int ballY = rect.top + BALL_Y_OFFSET;

                SetWindowPos(Ball, HWND_BOTTOM, ballX, ballY, BALL_WIDTH, BALL_HEIGHT, SWP_SHOWWINDOW);

                ShowWindow(Ball, SW_SHOW);

            }
        }
        firstRound = false;



        //--------------------------------------------










        // Запрос ответа у пользователя 
        cout << "\nПод каким стаканом мяч? Введите число (1-3): ";
        int userGuess;
        cin >> userGuess;
        userGuess--;

        // Проверка ответа 
        if (userGuess == ballUnderGlassIndex) {
            cout << "Правильно +10 очков" << endl;
            score += 10;
        }
        else {
            cout << "неправильно Мяч был под стаканом " << (ballUnderGlassIndex + 1) << ". -10 очков" << endl;
            score -= 10;
        }
        cout << "Текущий счёт: " << score << endl;

        //  анимация
        HWND glassWithBall = hWnds[ballUnderGlassIndex];
        RECT glassRect;
        GetWindowRect(glassWithBall, &glassRect);
        int glassX = glassRect.left;
        int glassY = glassRect.top;

        ShowWindow(Ball, SW_SHOW);
        SetWindowPos(glassWithBall, HWND_TOP, glassX, glassY, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
        for (int i = 0; i <= 40; i++) {
            SetWindowPos(glassWithBall, HWND_TOP, glassX, glassY - i * 5, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            Sleep(15);
        }
        Sleep(1500);
        for (int i = 40; i >= 0; i--) {
            SetWindowPos(glassWithBall, HWND_TOP, glassX, glassY - i * 5, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            Sleep(15);
        }

        // Проверка условий окончания игры
        if (score <= 0) {
            cout << "\nИгра окончена! Вы проиграли. Очков: 0" << endl;
            break;
        }
        if (score >= 100) {
            cout << "\nПоздравляем! Вы набрали 100 очков и победили!" << endl;
            break;
        }

        
        Sleep(1000);
    }
    // =========================================
    // 






















    // Ожидание завершения процессов (оригинальный код, перенесённый после цикла)
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
    CloseHandle(Bpi.hProcess);
    CloseHandle(Bpi.hThread);
}

int main() {
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

    StartGame();
    return 0;
}
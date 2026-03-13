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

HWND FindWindowWithRetry(const string& title, int maxAttempts = 50, int delayMs = 50) {
    for (int i = 0; i < maxAttempts; i++) {
        HWND hWnd = FindWindowA(NULL, title.c_str());
        if (hWnd) return hWnd;
        Sleep(delayMs);
    }
    return NULL;
}

void ShuffleGlasses(vector<HWND>& hWnds) {
    srand(static_cast<unsigned int>(time(0)));
    int shuffleCount = 5;

    for (int s = 0; s < shuffleCount; s++) {
        int idx1 = rand() % 3;
        int idx2 = rand() % 3;
        while (idx1 == idx2) idx2 = rand() % 3;

        RECT rect1, rect2;
        GetWindowRect(hWnds[idx1], &rect1);
        GetWindowRect(hWnds[idx2], &rect2);

        int x1 = rect1.left, y1 = rect1.top;
        int x2 = rect2.left, y2 = rect2.top;

        int steps = 10;
        int dx = (x2 - x1) / steps;

        bool ballUnderIdx1 = (ballUnderGlassIndex == idx1);
        bool ballUnderIdx2 = (ballUnderGlassIndex == idx2);

        for (int i = 0; i <= steps; i++) {
            SetWindowPos(hWnds[idx1], HWND_TOP, x1 + dx * i, y1, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            SetWindowPos(hWnds[idx2], HWND_TOP, x2 - dx * i, y2, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);

            if (ballUnderIdx1) {
                int ballX = (x1 + dx * i) + (GLASS_WIDTH - BALL_WIDTH) / 2;
                int ballY = y1 + BALL_Y_OFFSET;
                SetWindowPos(Ball, HWND_BOTTOM, ballX, ballY, BALL_WIDTH, BALL_HEIGHT, SWP_NOACTIVATE);
            }
            else if (ballUnderIdx2) {
                int ballX = (x2 - dx * i) + (GLASS_WIDTH - BALL_WIDTH) / 2;
                int ballY = y2 + BALL_Y_OFFSET;
                SetWindowPos(Ball, HWND_BOTTOM, ballX, ballY, BALL_WIDTH, BALL_HEIGHT, SWP_NOACTIVATE);
            }

            Sleep(20);
        }

        swap(hWnds[idx1], hWnds[idx2]);

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

    HANDLE hEvents[4];
    // Создаем события. Они изначально в несигнальном состоянии (FALSE)
    hEvents[0] = CreateEventA(NULL, TRUE, FALSE, "BallReadyEvent");
    hEvents[1] = CreateEventA(NULL, TRUE, FALSE, "GlassReadyEvent_1");
    hEvents[2] = CreateEventA(NULL, TRUE, FALSE, "GlassReadyEvent_2");
    hEvents[3] = CreateEventA(NULL, TRUE, FALSE, "GlassReadyEvent_3");



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

    // Создание процесса мяча
    string cmdLine = BallPath + " " + to_string(1);
    BOOL success = CreateProcessA(
        NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &Bsi, &Bpi);
    Ball = FindWindowWithRetry("Ball", 100, 50);
    if (!Ball) {
        cerr << "Ball window not found!" << endl;
    }

    cout << "Ожидание инициализации окон..." << endl;
    WaitForMultipleObjects(4, hEvents, TRUE, INFINITE);

    for (int i = 0; i < 4; i++) {
        CloseHandle(hEvents[i]);
    }

    // Поиск окон стаканов
    for (int i = 0; i < CountGlasses; i++) {
        string title = "Naperstki_" + to_string(i + 1);
        hWnds[i] = FindWindowWithRetry(title, 100, 50);
        if (!hWnds[i]) {
            cerr << "Window " << title << " not found!" << endl;
        }
    }

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

    // После перемешивания мяч уже в правильной позиции (двигали вместе со стаканом)
    // Поднимаем стакан и показываем мяч
    if (Ball != NULL) {
        SetWindowPos(hWnds[ballUnderGlassIndex], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        Sleep(10);
        SetWindowPos(Ball, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    }

    int score = 50; // стартовые очки
    bool firstRound = true;
    cout << "Текущий счёт: " << score << endl;

    while (score > 0 && score < 100) {
        if (!firstRound) {
            ShowWindow(Ball, SW_HIDE);
            ShuffleGlasses(hWnds);

            if (Ball != NULL) {
                // Мяч уже на правильной позиции, поднимаем стакан и показываем
                SetWindowPos(hWnds[ballUnderGlassIndex], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

                // БЫЛО: ShowWindow(Ball, SW_SHOW);
                // СТАЛО:
                SetWindowPos(Ball, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
            }
        }
        firstRound = false;

        // Запрос ответа у пользователя 
        int userGuess = -1;
        string input;

        while (userGuess < 1 || userGuess > 3) {
            cout << "\nПод каким стаканом мяч? Введите число (1-3): ";
            cin >> input;

            if (input == "1" || input == "2" || input == "3") {
                userGuess = stoi(input);
            }
            else {
                cout << "Нужно ввести 1, 2 или 3!" << endl;
            }
        }

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

        // Анимация поднятия стакана с мячом
        HWND glassWithBall = hWnds[ballUnderGlassIndex];
        RECT glassRect;
        GetWindowRect(glassWithBall, &glassRect);
        int glassX = glassRect.left;
        int glassY = glassRect.top;

        // Поднимаем стакан над всеми
        SetWindowPos(glassWithBall, HWND_TOP, glassX, glassY, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
        for (int i = 0; i <= 70; i++) {
            SetWindowPos(glassWithBall, HWND_TOP, glassX, glassY - i * 5, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            Sleep(15);
        }
        Sleep(1500);

        // Опускаем стакан
        for (int i = 70; i >= 0; i--) {
            SetWindowPos(glassWithBall, HWND_TOP, glassX, glassY - i * 5, GLASS_WIDTH, GLASS_HEIGHT, SWP_SHOWWINDOW);
            Sleep(15);
        }

        // Возвращаем мяч под стакан (на всякий случай, если координаты сбились)
        GetWindowRect(glassWithBall, &glassRect);
        int newBallX = glassRect.left + (GLASS_WIDTH - BALL_WIDTH) / 2;
        int newBallY = glassRect.top + BALL_Y_OFFSET;
        SetWindowPos(Ball, HWND_BOTTOM, newBallX, newBallY, BALL_WIDTH, BALL_HEIGHT, SWP_SHOWWINDOW);

        // Убеждаемся, что стакан сверху
        SetWindowPos(glassWithBall, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

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
    CloseHandle(Bpi.hProcess);
    CloseHandle(Bpi.hThread);
}

int main() {
    setlocale(0, "rus");

    HANDLE Mutex = CreateMutexA(NULL, TRUE, "Naperstki");
    Sleep(1000);

    if (!Mutex) {
        cout << "Не создался мьютекс" << endl;
        return 0;
    }
    else {
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

        ReleaseMutex(Mutex);
        Sleep(3000);
        CloseHandle(Mutex);
        return 0;
    }
}
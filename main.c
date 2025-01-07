#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>
#include "render3d.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_CREATE:
            // Inicia o temporizador para atualizar a tela a cada 16ms (aproximadamente 60 FPS)
            SetTimer(hwnd, 1, 16, NULL);
            return 0;

        case WM_KEYDOWN: {
            return 0;
        }

        case WM_PAINT: {
            // Prepare the device context for drawing
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Limpa a tela (preenchendo com fundo branco)
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW+1));

            // Chama a função para desenhar o objeto 3D (o cone)
            readObj("cube.obj", hdc);

            // Finaliza a pintura
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_TIMER: {
            // Força a atualização da tela a cada vez que o timer é chamado
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_SIZE:
            InvalidateRect(hwnd, NULL, TRUE);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {

    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char CLASS_NAME[] = "SimpleWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Falha ao registrar a classe da janela!", "Erro", MB_OK | MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Minha Janela Simples",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // Hide the cursor when the window is active
    ShowCursor(FALSE);

    if (!hwnd) {
        MessageBox(NULL, "Falha ao criar a janela!", "Erro", MB_OK | MB_ICONERROR);
        return -1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

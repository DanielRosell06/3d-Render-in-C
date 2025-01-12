#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>
#include "render3d.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int lastMouseX = -1;
    static int lastMouseY = -1;

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
            char g_lastKeyPressed = (char)wParam;
            float sinAngleZX = sin(angleZX); 
            float cosAngleZX = cos(angleZX);
            float sinTranslatedAngleZX = sin(angleZX + (M_PI / 2)); 
            float cosTranslatedAngleZX = cos(angleZX + (M_PI / 2)); 
            switch (g_lastKeyPressed) {
                case VK_LEFT:
                    angleZX -= 0.2;
                    break;

                case VK_RIGHT:
                    angleZX += 0.2;
                    break;

                case VK_UP:
                    angleZY -= 0.2;
                    break;

                case VK_DOWN:
                    angleZY += 0.2;
                    break;
                
                case 'A':
                    camZ -= cosTranslatedAngleZX;
                    camX -= sinTranslatedAngleZX;
                    break;
                
                case 'D':
                    camZ += cosTranslatedAngleZX;
                    camX += sinTranslatedAngleZX;
                    break;
                
                case 'W':
                    camZ += cosAngleZX;
                    camX += sinAngleZX;
                    break;

                case 'S':
                    camZ -= cosAngleZX;
                    camX -= sinAngleZX;
                    break;

                case VK_SPACE:
                    camY -= 0.5;
                    break;

                case VK_CONTROL:
                    camY += 0.5;
                    break;
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            RECT rect;
            GetClientRect(hwnd, &rect);

            if (lastMouseX != -1 && lastMouseY != -1) {
            int deltaX = mouseX - lastMouseX;
            int deltaY = mouseY - lastMouseY;

            angleZX += deltaX * 0.005f;
            angleZY += deltaY * 0.005f;

            InvalidateRect(hwnd, NULL, TRUE);
            }

            lastMouseX = mouseX;
            lastMouseY = mouseY;

            // Check if the mouse is at the edge of the window and reset the cursor position
            if (mouseX <= 0 || mouseX >= rect.right - 1 || mouseY <= 0 || mouseY >= rect.bottom - 1) {
            POINT center = {rect.right / 2, rect.bottom / 2};
            ClientToScreen(hwnd, &center);
            SetCursorPos(center.x, center.y);
            lastMouseX = rect.right / 2;
            lastMouseY = rect.bottom / 2;
            }

            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Obtenha o retângulo da janela
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // Crie um DC de memória
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(memDC, memBitmap);

            // Limpe o buffer de memória
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(memDC, &rect, hBrush);
            DeleteObject(hBrush);

            // Desenhe no DC de memória
            readDrawObj("cube.obj", memDC);

            // Copie o buffer de memória para o HDC da janela
            BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

            // Libere os recursos do DC de memória
            DeleteObject(memBitmap);
            DeleteDC(memDC);

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
        "3d Render",
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

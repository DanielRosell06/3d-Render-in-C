#ifndef RENDER3D_H
#define RENDER3D_H

#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>

// Definições
#define M_PI 3.14159265358979323846

extern float angleZX;
extern float angleZY;
extern float focalLength;
extern int screenCenterX;
extern int screenCenterY;
extern float max_produtoEixoZ;
extern struct vertice3d* verticesArrayPointer;
extern float camX;
extern float camY;
extern float camZ;

struct vertice2d {
    float x, y;
};

struct vertice3d {
    float x, y, z;
};

struct face3d {
    int v1, v2, v3;
};

struct face2d {
    struct vertice2d v1, v2, v3;
};

struct triangle2d {
    struct vertice2d v1, v2, v3;
};


struct vertice2d project(struct vertice3d v);
void drawTriangle(HDC hdc, struct vertice2d v1, struct vertice2d v2, struct vertice2d v3, unsigned char r, unsigned char g, unsigned char b);
void readObj(char* fileName, HDC hdc);

#endif
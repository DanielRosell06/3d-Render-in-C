#include "render3d.h"
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>

float angleZX = 0;
float angleZY = 0;
float focalLength = 1000.0;
int screenCenterX = 400;
int screenCenterY = 300;
float max_produtoEixoZ;
struct vertice3d* verticesArrayPointer;
float camX = 0;
float camY = 0;
float camZ = 0;


int distanceBetweenVertices(struct vertice3d v1, struct vertice3d v2) {
    return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

struct vertice2d project(struct vertice3d v){
    struct vertice2d v2d;

    float divisor = focalLength / v.z;

    v2d.x = (v.x * divisor) + screenCenterX;
    v2d.y = (v.y * divisor) + screenCenterY;

    return v2d;
}

void translate(float camera_x, float camera_y, float camera_z, struct vertice3d* vertices, int tamanhoVertices){
    int i;
    for(i = 0; i < tamanhoVertices; i++){
        vertices[i].x = vertices[i].x - camera_x;
        vertices[i].y = vertices[i].y - camera_y;
        vertices[i].z = vertices[i].z - camera_z;
    }
}

void rotate(float angleZX, float angleZY, struct vertice3d* vertices, int tamanhoVertices) {
    float cosZX = cos(angleZX);
    float sinZX = sin(angleZX);
    float cosZY = cos(angleZY);
    float sinZY = sin(angleZY);

    for (int i = 0; i < tamanhoVertices; i++) {
        float x = vertices[i].x;
        float y = vertices[i].y;
        float z = vertices[i].z;

        // Rotação em torno de ZX
        float x_rotated = cosZX * x - sinZX * z;
        float z_temp = sinZX * x + cosZX * z;

        // Rotação em torno de ZY
        float y_rotated = cosZY * y - sinZY * z_temp;
        float z_rotated = sinZY * y + cosZY * z_temp;

        vertices[i].x = x_rotated;
        vertices[i].y = y_rotated;
        vertices[i].z = z_rotated;
    }
}

void drawTriangle(HDC hdc, struct vertice2d v1, struct vertice2d v2, struct vertice2d v3, unsigned char r, unsigned char g, unsigned char b) {
    // Cria um pincel sólido para preencher o triângulo
    HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
    SelectObject(hdc, hBrush);

    // Define que não haverá contorno usando NULL_PEN
    HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
    SelectObject(hdc, hPen);

    // Define os vértices do triângulo
    POINT vertices[] = {
        {v1.x, v1.y},
        {v2.x, v2.y},
        {v3.x, v3.y}
    };

    // Desenha o triângulo preenchido
    Polygon(hdc, vertices, 3);

    // Libera os recursos
    DeleteObject(hBrush);
}


int compare(const void *x_void, const void *y_void){
    struct face3d face1 = *(struct face3d*)x_void;
    struct face3d face2 = *(struct face3d*)y_void;

    float z1 = (verticesArrayPointer[face1.v1].z + verticesArrayPointer[face1.v2].z + verticesArrayPointer[face1.v3].z) / 3.0;
    float z2 = (verticesArrayPointer[face2.v1].z + verticesArrayPointer[face2.v2].z + verticesArrayPointer[face2.v3].z) / 3.0;

    if (z1 < z2) return 1;
    if (z1 > z2) return -1;
    return 0;
}


void readDrawObj(char* fileName, HDC hdc) {

    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Não foi possível encontrar o arquivo\n");
        return;
    }

    int tamanhoVertices = 0, tamanhoFaces = 0;
    struct vertice3d* vertices = NULL; // Ponteiro para o array dinâmico de vértices
    struct face3d* faces = NULL; // Ponteiro para o array dinâmico de faces
    char line[128];

    while (fgets(line, sizeof(line), file)) {
        // Processa os vértices
        if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;
            sscanf(line + 2, "%f %f %f", &x, &y, &z);

            // Realoca memória para o novo vértice
            struct vertice3d* temp = realloc(vertices, sizeof(struct vertice3d) * (tamanhoVertices + 1));
            if (temp == NULL) {
                printf("Erro ao alocar memória para os vértices\n");
                free(vertices);
                fclose(file);
                return;
            }
            vertices = temp;
            verticesArrayPointer = vertices;

            // Armazena o vértice no array
            vertices[tamanhoVertices].x = x;
            vertices[tamanhoVertices].y = y;
            vertices[tamanhoVertices].z = z;
            tamanhoVertices++;
        }
        // Processa as faces
        else if (line[0] == 'f' && line[1] == ' ') {
            int v1, v2, v3;
            // Lê os índices das faces
            sscanf(line + 2, "%d %d %d", &v1, &v2, &v3);

            // Realoca memória para a nova face
            struct face3d* temp = realloc(faces, sizeof(struct face3d) * (tamanhoFaces + 1));
            if (temp == NULL) {
                printf("Erro ao alocar memória para as faces\n");
                free(vertices);
                free(faces);
                fclose(file);
                return;
            }
            faces = temp;

            // Armazena a face no array (note que as faces geralmente usam 1-base index, então subtraímos 1 para 0-base)
            faces[tamanhoFaces].v1 = v1 - 1;
            faces[tamanhoFaces].v2 = v2 - 1;
            faces[tamanhoFaces].v3 = v3 - 1;
            tamanhoFaces++;
        }
    }

    fclose(file);

    // Aplica a rotação aos vértices
    translate(camX, camY, camZ, vertices, tamanhoVertices);
    rotate(angleZX, angleZY, vertices, tamanhoVertices);
    qsort(faces, tamanhoFaces, sizeof(struct face3d), compare);
    

    // Projeta as faces na tela
    int i = 0;
    struct vertice2d vertice2d1, vertice2d2, vertice2d3;
    for(i = 0; i < tamanhoFaces; i++){
        struct vertice3d v3d1 = vertices[faces[i].v1];
        struct vertice3d v3d2 = vertices[faces[i].v2];
        struct vertice3d v3d3 = vertices[faces[i].v3];

        int validateV1 = 1;
        int validateV2 = 1;
        int validateV3 = 1;

        if((v3d1.x)/v3d1.z < -2.0/5.0 || (v3d1.x)/v3d1.z > 2.0/5.0 || v3d1.z < 2){
            validateV1 = 0;
        }

        if((v3d2.x)/v3d2.z < -2.0/5.0 || (v3d2.x)/v3d2.z > 2.0/5.0 || v3d2.z < 2){
            validateV2 = 0;
        }

        if((v3d3.x)/v3d3.z < -2.0/5.0 || (v3d3.x)/v3d3.z > 2.0/5.0 || v3d3.z < 2){
            validateV3 = 0;
        }
        
        if(validateV1 || validateV2 || validateV3){
            float distanciaP3P1 = sqrt(pow(v3d3.x - v3d1.x, 2) + pow(v3d3.y - v3d1.y, 2) + pow(v3d3.z - v3d1.z, 2));
            float distanciaP1P2 = sqrt(pow(v3d1.x - v3d2.x, 2) + pow(v3d1.y - v3d2.y, 2) + pow(v3d1.z - v3d2.z, 2));

            float produtoEixoZ = ((((v3d3.x - v3d1.x)/distanciaP3P1) * ((v3d2.y - v3d1.y))/distanciaP1P2) - ((((v3d2.x - v3d1.x)/distanciaP3P1)) * ((v3d3.y - v3d1.y)/distanciaP1P2)));

            if(produtoEixoZ < 0){

                struct vertice2d v2d1 = project(v3d1);
                struct vertice2d v2d2 = project(v3d2);
                struct vertice2d v2d3 = project(v3d3);

                float temporaryRGB = (255 - (255 * produtoEixoZ));
                unsigned char rgb = temporaryRGB;
                drawTriangle(hdc, v2d1, v2d2, v2d3, rgb, rgb, rgb);

            }
        }
    }

    // Libera a memória alocada
    free(vertices);
    free(faces);
}

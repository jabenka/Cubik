#include<stdio.h>
#include<cmath>
#include<cstring>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
void usleep(__int64 usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif

class Cube {
private:
    float A, B, C;
    float CubeWidth;
    int width, height;
    float zBuffer[160 * 44];
    char buffer[160 * 44];
    int backgroundCode;
    int CamDistance;
    float horizOffset;
    float K1;
    float Speedinc;
    float x, y, z;
    float ooz;
    int xp, yp;
    int idx;
public:
    Cube():A(0.0),B(0.0),C(0.0),CubeWidth(28),width(160),height(44),
        backgroundCode(' '),CamDistance(100),Speedinc(0.5),horizOffset(CubeWidth+2),K1(40){}
    ~Cube(){}
    float calcX(int i, int j, int k) {
        return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
            j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
    }
    float calcY(int i, int j, int k) {
        return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
            j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
            i * cos(B) * sin(C);
    }
    float calcZ(int i, int j, int k) {
        return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
    }
    void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
        x = calcX(cubeX, cubeY, cubeZ);
        y = calcY(cubeX, cubeY, cubeZ);
        z = calcZ(cubeX, cubeY, cubeZ) + CamDistance;
        ooz = 1 / z;
        xp = (int)(width / 2 + horizOffset + K1 * ooz * x * 2);
        yp = (int)(height / 2 + K1 * ooz * y);
        idx = xp + yp * width;
        if (idx >= 0 && idx < width * height) {
            if (ooz > zBuffer[idx]) {
                zBuffer[idx] = ooz;
                buffer[idx] = ch;
            }
        }
    }
    void init() {
        printf("\x1b[2J");
        while (1) {
            memset(buffer, backgroundCode, width * height);
            memset(zBuffer, 0, width * height * 4);
            CubeWidth = 28;
            for (float cubeX = -CubeWidth; cubeX < CubeWidth; cubeX += Speedinc) {
                for (float cubeY = -CubeWidth; cubeY < CubeWidth;
                    cubeY +=Speedinc) {
                    calculateForSurface(cubeX, cubeY, -CubeWidth, '!');
                    calculateForSurface(CubeWidth, cubeY, cubeX, '#');
                    calculateForSurface(-CubeWidth, cubeY, -cubeX, '<>');
                    calculateForSurface(-cubeX, cubeY, CubeWidth, '^');
                    calculateForSurface(cubeX, -CubeWidth, -cubeY, '~');
                    calculateForSurface(cubeX, CubeWidth, cubeY, '*');
                }
            }
            printf("\x1b[H");
            for (int k = 0; k < width * height; k++) {
                putchar(k % width ? buffer[k] : 10);
            }
            A += 0.09;
            B += 0.09;
            C += 0.07;
            usleep(1);
        }
    }
};
int main() {
    Cube cubik;
    cubik.init();

    return 0;
}
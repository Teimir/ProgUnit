#define _CRT_SECURE_NO_WARNINGS 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//Локальные заголовки
#include "ComIface.h"

int _tmain(int argc, TCHAR* argv[])
{
    int numOfTests = 0;
    ComIface comiface(0);
    byte data[] = { 0x1b, 0x1c, 0x12, 0x25 };
    byte buffer = 0;
    for (int i = 0; i < 256; i++) {
        comiface.ComNum = i;
        if (comiface.Open()) {
            printf("COM%D id avalible!", i);
            comiface.Close();
            numOfTests++;
        }
    }
    if (!numOfTests) {
        printf("No one COM-port is avalible");
        exit(0);
    }
    printf("Set number of port: ");
    scanf("%d", &numOfTests);
    comiface.ComNum = numOfTests;
    comiface.Open();
    printf("Set number of tests: ");
    scanf("%d", &numOfTests);
    comiface.PrintState();
    int c = 0;
    //_tprintf (TEXT("Serial port %s successfully reconfigured.\n"), pcCommPort);
    for (int i = 0; i < numOfTests; i++) {
        comiface.Write(&data[i % 4], 1);
        Sleep(100);
        comiface.Read(&buffer, 1);
        printf("Translated - %x, recieved - %x     ", data[i % 4], buffer);
        printf("%d  ", i+1);
        printf(data[i % 4] == buffer ? "YES\n" : "NO\n");
        c = data[i % 4] != buffer ? c + 1 : c;
        Sleep(100);
    }
    printf("Errors - %d\n", c);
    comiface.Close();
    getchar();
    return (0);
}

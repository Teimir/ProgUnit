#define _CRT_SECURE_NO_WARNINGS 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//Локальные заголовки
#include "src/Header_Ts.h"


void PrintCommState(DCB dcb)
{
    //  Print some of the DCB structure values
    _tprintf(TEXT("\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n"),
        dcb.BaudRate,
        dcb.ByteSize,
        dcb.Parity,
        dcb.StopBits);
}


int _tmain(int argc, TCHAR* argv[])
{
    int numOfTests = 0;
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;
    byte data[] = { 0x1b, 0x1c, 0x12, 0x25 };
    byte buffer = 0;

    printf("Set number of tests: ");
    scanf("%d", &numOfTests);

    //TCHAR *pcCommPort = TEXT("COM8"); //  Most systems have a COM1 port

    //  Open a handle to the specified com port.
    hCom = CreateFile(L"\\\\.\\COM8",
        GENERIC_READ | GENERIC_WRITE,
        0,      //  must be opened with exclusive-access
        NULL,   //  default security attributes
        OPEN_EXISTING, //  must use OPEN_EXISTING
        0,      //  not overlapped I/O
        NULL); //  hTemplate must be NULL for comm devices

    if (hCom == INVALID_HANDLE_VALUE)
    {
        //  Handle the error.
        printf("CreateFile failed with error %d.\n", GetLastError());
        return (1);
    }

    //  Initialize the DCB structure.
    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    //  Build on the current configuration by first retrieving all current
    //  settings.
    fSuccess = GetCommState(hCom, &dcb);

    if (!fSuccess)
    {
        //  Handle the error.
        printf("GetCommState failed with error %d.\n", GetLastError());
        return (2);
    }

    PrintCommState(dcb);       //  Output to console

    //  Fill in some DCB values and set the com state: 
    //  57,600 bps, 8 data bits, no parity, and 1 stop bit.
    dcb.BaudRate = CBR_9600;     //  baud rate
    dcb.ByteSize = 8;             //  data size, xmit and rcv
    dcb.Parity = NOPARITY;      //  parity bit
    dcb.StopBits = ONESTOPBIT;    //  stop bit

    fSuccess = SetCommState(hCom, &dcb);

    if (!fSuccess)
    {
        //  Handle the error.
        printf("SetCommState failed with error %d.\n", GetLastError());
        return (3);
    }

    //  Get the comm config again.
    fSuccess = GetCommState(hCom, &dcb);

    if (!fSuccess)
    {
        //  Handle the error.
        printf("GetCommState failed with error %d.\n", GetLastError());
        return (2);
    }
    int c = 0;
    //_tprintf (TEXT("Serial port %s successfully reconfigured.\n"), pcCommPort);
    for (int i = 0; i < numOfTests; i++) {
        WriteFile(hCom, &data[i % 4], 1, 0, 0);
        Sleep(100);
        ReadFile(hCom, &buffer, 1, 0, 0);
        printf("Translated - %x, recieved - %x     ", data[i % 4], buffer);
        printf("%d  ", i+1);
        printf(data[i % 4] == buffer ? "YES\n" : "NO\n");
        c = data[i % 4] != buffer ? c + 1 : c;
        Sleep(100);
    }
    printf("Errors - %d\n", c);
    CloseHandle(hCom);
    getchar();
    return (0);
}

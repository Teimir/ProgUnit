#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "ComIface.h"


ComIface::ComIface(byte ComNum, DWORD BaudRate) {
    wchar_t strbuffer[11];
    this->ComNum = ComNum;
    swprintf_s(strbuffer, L"\\\\.\\COM%d", ComNum);
    this->PortHandle = CreateFileW(strbuffer,
        GENERIC_READ | GENERIC_WRITE,
        0,      //  must be opened with exclusive-access
        NULL,   //  default security attributes
        OPEN_EXISTING, //  must use OPEN_EXISTING
        0,      //  not overlapped I/O
        NULL); //  hTemplate must be NULL for comm devices
    if (this->PortHandle == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile failed with error %d.\n", GetLastError());
        exit(1);
    }
    this->dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(this->PortHandle, &this->dcb)) {
        printf("GetCommState failed with error %d.\n", GetLastError());
        exit(2);
    }
    this->dcb.BaudRate = BaudRate;     //  baud rate
    this->dcb.ByteSize = 8;             //  data size, xmit and rcv
    this->dcb.Parity = NOPARITY;      //  parity bit
    this->dcb.StopBits = ONESTOPBIT;    //  stop bit
    if (!SetCommState(this->PortHandle, &this->dcb)) {
        printf("SetCommState failed with error %d.\n", GetLastError());
        exit(3);
    }
};

void ComIface::ChangeRate(DWORD BaudRate) {
    this->dcb.BaudRate = BaudRate;
    if (!SetCommState(this->PortHandle, &this->dcb)) {
        printf("SetCommState failed with error %d.\n", GetLastError());
        exit(3);
    }
}

DWORD ComIface::Write(byte* data, int count) {
    DWORD NumOfWritten;
    WriteFile(this->PortHandle, data, count, &NumOfWritten, 0);
    return NumOfWritten;
}

DWORD ComIface::Read(byte* buffer, int count) {
    DWORD NumOfRead;
    ReadFile(this->PortHandle, buffer, count, &NumOfRead, 0);
    return NumOfRead;
}

void ComIface::PrintState() {
    printf("\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n",
        this->dcb.BaudRate,
        this->dcb.ByteSize,
        this->dcb.Parity,
        this->dcb.StopBits);
}

void ComIface::Close() {
    CloseHandle(this->PortHandle);
}
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "ComIface.h"

ComIface::ComIface(byte ComNum, DWORD BaudRate) {
    this->ComNum = ComNum;
    this->BaudRate = BaudRate;
    this->IsOpen = false;
};

bool ComIface::Open() {
    this->IsOpen = true;
    wchar_t strbuffer[11];
    swprintf_s(strbuffer, L"\\\\.\\COM%d", this->ComNum);

    
    this->PortHandle = CreateFileW(strbuffer,
        GENERIC_READ | GENERIC_WRITE,
        0,      //  must be opened with exclusive-access
        NULL,   //  default security attributes
        OPEN_EXISTING, //  must use OPEN_EXISTING
        0,      //  not overlapped I/O
        NULL); //  hTemplate must be NULL for comm devices
    this->dcb.DCBlength = sizeof(DCB);
    if (this->PortHandle == INVALID_HANDLE_VALUE || !GetCommState(this->PortHandle, &this->dcb)) {
        return false;
    }

    GetCommTimeouts(PortHandle, &this->timeouts);
    this->timeouts.ReadIntervalTimeout = MAXDWORD; // Maximum time between two characters in milliseconds
    this->timeouts.ReadTotalTimeoutConstant = 100; // Total time (constant part) for reading in milliseconds
    this->timeouts.ReadTotalTimeoutMultiplier = 0;  // Total time multiplier in milliseconds per byte read

    SetCommTimeouts(PortHandle, &this->timeouts);
    SetupComm(PortHandle, 1024 * 8, 1024 * 8);
    this->dcb.BaudRate = this->BaudRate;     //  baud rate
    this->dcb.ByteSize = 8;             //  data size, xmit and rcv
    this->dcb.Parity = NOPARITY;      //  parity bit
    this->dcb.StopBits = ONESTOPBIT;    //  stop bit
    if (!SetCommState(this->PortHandle, &this->dcb)) {
        return false;
    }
    return true;
}

void ComIface::ChangeRate(DWORD BaudRate) {
    this->BaudRate = BaudRate;
    if(this->IsOpen){
        this->dcb.BaudRate = BaudRate;
        if (!SetCommState(this->PortHandle, &this->dcb)) {
            printf("SetCommState failed with error %d.\n", GetLastError());
            exit(3);
        }
    }
}

DWORD ComIface::Write(byte* data, int count) {
    DWORD NumOfWritten;
    WriteFile(this->PortHandle, data, count, &NumOfWritten, NULL);
    /*while (NumOfWritten != count) {
        printf("%x     \n", NumOfWritten);
        Sleep(10);
    }*/
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
    this->IsOpen = false;
    CloseHandle(this->PortHandle);
}
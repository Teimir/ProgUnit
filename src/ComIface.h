#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

class ComIface {
public:
    HANDLE PortHandle = INVALID_HANDLE_VALUE;
    int ComNum = 0;
    bool IsOpen = false;
    DCB dcb;
    DWORD BaudRate;
    ComIface(byte ComNum, DWORD BaudRate = CBR_9600);
    void ChangeRate(DWORD NewBaudRate);
    DWORD Write(byte* data, int count);
    DWORD Read(byte* buffer, int count);
    void PrintState();
    void Open();
    void Close();
};
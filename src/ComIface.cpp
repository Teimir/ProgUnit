#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "ComIface.h"

ComIface::ComIface(DWORD BaudRate) {
    dcb.BaudRate = BaudRate;
    dcb.DCBlength = sizeof(DCB);
};
ComIface::~ComIface() {
    if (open_state) {
        close();
    }
}
bool ComIface::open(int _port_num, DWORD read_ms, bool log) {
    //create port name
    wchar_t strbuffer[11];
    swprintf_s(strbuffer, 11, L"\\\\.\\COM%d", _port_num);
    //create port handle
    port_handle = CreateFileW(
        strbuffer,
        GENERIC_READ | GENERIC_WRITE,
        0,                              //must be opened with exclusive-access
        NULL,                           //default security attributes
        OPEN_EXISTING,                  //must use OPEN_EXISTING
        FILE_FLAG_OVERLAPPED,           //async I/O
        NULL                            //hTemplate must be NULL for comm devices
    );
    //check if handle is valid
    if (port_handle == INVALID_HANDLE_VALUE) {
        if (log) {
            printf("CreateFileW failed with error %d.\n", GetLastError());
        }
        return false;
    }
    if (!GetCommState(port_handle, &dcb)) {
        if (log) {
            printf("GetCommState failed with error %d.\n", GetLastError());
        }
        return false;
    }
    //set dcb
    dcb.ByteSize = 8;           //  data size, xmit and rcv
    dcb.Parity = NOPARITY;      //  parity bit
    dcb.StopBits = ONESTOPBIT;  //  stop bit
    //set port settings
    if (!SetCommState(port_handle, &dcb)) {
        if (log) {
            printf("SetCommState failed with error %d.\n", GetLastError());
        }
        return false;
    }
    //set port state
    open_state = true;
    port_num = _port_num;
    read_delay = read_ms;
    return true;
}
DWORD ComIface::write(byte* data, int count) {
    DWORD NumOfWritten, status;
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlap.hEvent) {
        printf("CreateEvent failed with error %d.\n", GetLastError());
        return 0;
    }
    if (!WriteFile(port_handle, data, count, &NumOfWritten, &overlap)) {
        status = GetLastError();
        if (status != ERROR_IO_PENDING) {
            printf("WriteFile failed with error %d.\n", status);
            return 0;
        }
        if (!GetOverlappedResult(port_handle, &overlap, &NumOfWritten, TRUE)) {
            NumOfWritten = 0;
        }
    }
    CloseHandle(overlap.hEvent);
    return NumOfWritten;
}
DWORD ComIface::read(byte* buffer, int count) {
    DWORD NumberOfBytesRead, status;
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlap.hEvent) {
        printf("CreateEvent failed with error %d.\n", GetLastError());
        return 0;
    }
    if (!ReadFile(port_handle, buffer, count, &NumberOfBytesRead, &overlap)) {
        status = GetLastError();
        if (status != ERROR_IO_PENDING) {
            printf("ReadFile failed with error %d.\n", status);
            return 0;
        }
        status = WaitForSingleObject(overlap.hEvent, read_delay);
        switch (status) {
            case WAIT_TIMEOUT:
                //ok, do other work then
                NumberOfBytesRead = 0;
                break;
            default:
                printf("OVERLAPPED event handle error\n");
                NumberOfBytesRead = 0;
                break;
        }
    }
    CloseHandle(overlap.hEvent);
    return NumberOfBytesRead;
}
void ComIface::close() {
    open_state = false;
    CloseHandle(port_handle);
}
void ComIface::set_rate(DWORD BaudRate) {
    dcb.BaudRate = BaudRate;
    if (open_state) {
        if (!SetCommState(port_handle, &dcb)) {
            printf("SetCommState failed with error %d.\n", GetLastError());
            exit(3);
        }
    }
}
void ComIface::set_read_delay(DWORD read_ms) {
    read_delay = read_ms;
}
bool ComIface::is_open() {
    return open_state;
}
void ComIface::log_state() {
    printf(
        "\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n",
        dcb.BaudRate,
        dcb.ByteSize,
        dcb.Parity,
        dcb.StopBits
    );
}
int ComIface::get_port_num() {
    return port_num;
}
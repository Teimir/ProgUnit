#include "ComIface.h"
using namespace dte_utils;
ComIface::ComIface(DWORD BaudRate, DWORD read_delay) : read_delay(read_delay) {
    dcb.BaudRate = BaudRate;
    dcb.DCBlength = sizeof(DCB);
    dcb.ByteSize = DATABITS_8;  //  data size, xmit and rcv
    dcb.Parity = PARITY_NONE;   //  parity bit
    dcb.StopBits = ONE5STOPBITS;  //  stop bit
};
bool ComIface::open(int _port_num, bool log) {
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
    //set port settings
    if (!SetCommState(port_handle, &dcb)) {
        if (log) {
            printf("SetCommState failed with error %d.\n", GetLastError());
        }
        close();
        return false;
    }
    //set port timings
    COMMTIMEOUTS timings{
        1,      /* Maximum time between read chars. */
        0,     /* Multiplier of characters.        */
        0,     /* Constant in milliseconds.        */
        0,      /* Multiplier of characters.        */
        0       /* Constant in milliseconds.        */
    };
    SetCommTimeouts(port_handle, &timings);
    PurgeComm(port_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    //set port state
    port_num = _port_num;
    return true;
}
DWORD ComIface::write(byte* data, int count) {
    DWORD NumOfWritten, status;
    OVERLAPPED overlap = { 0 };
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlap.hEvent) {
        printf("CreateEvent failed with error %d.\n", GetLastError());
        return 0;
    }
    if (!WriteFile(port_handle, data, count, &NumOfWritten, &overlap)) {
        status = GetLastError();
        if (status != ERROR_IO_PENDING) {
            printf("WriteFile failed with error %d.\n", status);
            CloseHandle(overlap.hEvent);
            return 0;
        }
        if (!GetOverlappedResult(port_handle, &overlap, &NumOfWritten, TRUE)) {
            printf("GetOverlappedResult failed with error %d.\n", GetLastError());
        }
    }
    CloseHandle(overlap.hEvent);
    return NumOfWritten;
}
DWORD ComIface::read(byte* buffer, int size) {
    DWORD NumberOfBytesReaded = 0, status = 0;
    OVERLAPPED overlap = { 0 };
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlap.hEvent) {
        printf("CreateEvent failed with error %d.\n", GetLastError());
        return 0;
    }
    if (!ReadFile(port_handle, buffer, size, NULL, &overlap)) {
        status = GetLastError();
        if (status != ERROR_IO_PENDING) {
            printf("ReadFile failed with error %d.\n", status);
            CloseHandle(overlap.hEvent);
            return 0;
        }
        if (!GetOverlappedResult(port_handle, &overlap, &NumberOfBytesReaded, TRUE)) {
            printf("GetOverlappedResult failed with error %d.\n", GetLastError());
        }
    }
    CloseHandle(overlap.hEvent);
    return NumberOfBytesReaded;
}
void ComIface::close() {
    if (!is_not_open()) {
        CloseHandle(port_handle);
        port_handle = INVALID_HANDLE_VALUE;
    }
}
void ComIface::set_rate(DWORD BaudRate) {
    dcb.BaudRate = BaudRate;
    if (!is_not_open()) {
        if (!SetCommState(port_handle, &dcb)) {
            GetCommState(port_handle, &dcb);    //return back to our rate
            printf("SetCommState failed with error %d.\n", GetLastError());
        }
    }
}
void ComIface::set_read_delay(DWORD _read_delay) {
    read_delay = _read_delay;
}
void ComIface::set_buffer(DWORD r_size, DWORD t_size, bool purge) {
    SetupComm(port_handle, r_size, t_size);
    if (purge) {
        PurgeComm(port_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }
}
bool ComIface::is_not_open() {
    return port_handle == INVALID_HANDLE_VALUE;
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
COMSTAT ComIface::get_stats() {
    COMSTAT stats;
    ClearCommError(port_handle, NULL, &stats);
    return stats;
}
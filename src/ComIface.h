#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

class ComIface {
    public:
        ComIface(DWORD BaudRate = CBR_9600);
        ComIface(const ComIface&) = delete;
        ~ComIface();
        DWORD write(byte* data, int count);
        DWORD read(byte* buffer, int count);
        bool open(int com_num, DWORD read_ms = 500, bool log = true);
        void close();
        //set methods
        void set_rate(DWORD BaudRate);
        void set_read_delay(DWORD read_ms);
        //get methods
        bool is_open();
        void log_state();
        int get_port_num();
    protected:
        int port_num = 0;
        bool open_state = false;
        DCB dcb;
        DWORD read_delay;
        HANDLE port_handle = INVALID_HANDLE_VALUE;
        OVERLAPPED overlap = { 0 };
};
#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

class ComIface {
    public:
        ComIface(DWORD BaudRate = CBR_9600, DWORD read_delay = 500);
        ComIface(const ComIface&) = delete;
        //why close() in ~ComIface() cause crashes?
        DWORD write(byte* data, int count);
        DWORD read(byte* buffer, int count);
        bool open(int _port_num, bool log = true);
        void close();
        //set methods
        void set_rate(DWORD BaudRate);
        void set_read_delay(DWORD _read_delay);
        //get methods
        bool is_not_open();
        void log_state();
        int get_port_num();
        HANDLE port_handle = INVALID_HANDLE_VALUE;
    protected:
        int port_num = 0;
        DCB dcb;
        DWORD read_delay;
        
};
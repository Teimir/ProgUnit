#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "utils/include/exec_time.h"

class ComIface {
    public:
        ComIface(DWORD BaudRate = CBR_9600, DWORD read_delay = 10); //~1024 byte transmitions will be near 106,(6)ms
        ComIface(const ComIface&) = delete;
        //why close() in ~ComIface() cause crashes?
        DWORD write(byte* data, int count);
        DWORD read(byte* buffer, int count);
        bool open(int _port_num, bool log = true);
        void close();
        //set methods
        void set_rate(DWORD BaudRate);
        void set_read_delay(DWORD _read_delay);
        void set_buffer(DWORD r_size, DWORD t_size, bool purge = false);
        //get methods
        bool is_not_open();
        void log_state();
        int get_port_num();
        COMSTAT get_stats();
    protected:
        int port_num = 0;
        DCB dcb;
        DWORD read_delay;
        HANDLE port_handle = INVALID_HANDLE_VALUE;
};
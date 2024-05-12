#define _CRT_SECURE_NO_WARNINGS 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <algorithm>
#include <thread>
//Локальные заголовки
#include "ComIface.h"
using namespace dte_utils;
//can be used later




bool auto_connection(ComIface& c, int to = 256, int from = 0) {
    for (int i = from; i < to; ++i) {
        if (c.open(i, false)) {
            printf("Connected to COM %d\n", i);
            return true;
            c.close();
        }
    }
    printf("There is no COM id avalible from %d to %d\n", from, to);
    return false;
}
int mass_test_sync(ComIface& c) {
    int failures = 0;
    const int byte_num = 1024 * 67;
    if (!c.is_not_open()) {
        //declare R & T
        byte T[byte_num];
        byte R[byte_num];
        //fill T
        for (int i = 0; i < byte_num; i++) {
            T[i] = i % 255;
        }
        //prepare port buffer
        c.set_buffer(byte_num*16535, byte_num * 16535, true);
        //read & write
        printf("WRITED: %d\n", c.write(T, byte_num));
        printf("READED: %d\n", c.read(R, byte_num));
        for (int i = 0; i < byte_num; ++i) {
            if (T[i] != R[i]) {
                printf("%d\tTranslated - %02hhx,\trecieved - %02hhx\tERROR\n", i, T[i], R[i]);
                ++failures;
            }
        }
    }
    else {
        printf("Can`t test closed port\n");
        failures = byte_num;
    }
    return failures;
}


/*
* tests port RX-TX connection, N is array length, it_num is number of test
*/
template<typename T>
bool test(ComIface& c_rx, ComIface& c_tx, T* transmit, int N, int it_num = 0) {
    //return true if failed
    byte* recieve = new byte[sizeof(T) * N]{0};
    bool failed = true;
    DWORD state = c_tx.write(transmit, sizeof(T) * N);
    if (state == sizeof(T) * N) {
        state = c_rx.read(recieve, 1);
        if (state == sizeof(T) * N) {
            std::reverse(recieve, recieve + sizeof(T) * N);
            failed = memcmp(transmit, recieve, sizeof(T) * N);
        }
        else {
            printf("Read failed (%d/%zd)!\t", state, sizeof(T) * N);
        }
    }
    else {
        printf("Write failed (%d/%zd)!\t", state, sizeof(T) * N);
    }
    printf("%d\tTranslated - ", it_num);
    for (int i = 0; i < sizeof(T) * N; ++i) {
        printf("%02hhx", ((byte*)transmit)[i]);
    }
    printf(",\trecieved - ");
    for (int i = 0; i < sizeof(T) * N; ++i) {
        printf("%02hhx", recieve[i]);
    }
    printf("\t%s", failed ? "NO\n" : "YES\n");
    delete[] recieve;
    return failed;
}



//In full - speed USB, the maximum packet size is 64 bytes
//In high - speed USB, the maximum packet size is 512 bytes
//internal port buffers (IN, OUT) can be changed - SetupComm
//////////////////COMSTAT (remember it!)
int _tmain(int argc, TCHAR* argv[]) {
    int numOfTests = 0;
    ComIface comiface(CBR_9600);
    byte data[] = { 0x1b, 0x1c, 0x12, 0x25 };
    byte edata[8][8] = {
        {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
        {0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
        {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17},
        {0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f},
        {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27},
        {0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f},
        {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37},
        {0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f}
    };
    byte sw_ch[] = { 0xff, 0xf3 }; //256000
    byte sw_ch2[] = { 0xff, 0xf4 }; //512000
    byte rw_ch[] = { 0xff, 0xf1 };
    byte buffer = 0;
    byte ebuffer[8];
    
    
    auto_connection(comiface);
    //comiface.open(12);
    
    if (comiface.is_not_open()) {
        printf("No one COM-port is avalible\n");
        exit(1);
    }
    comiface.log_state();
    
    //mass test
    mass_test_sync(comiface);

    //change rate
    comiface.write(&sw_ch2[0], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %02hhx\n", buffer);
    comiface.write(&sw_ch2[1], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %02hhx\n", buffer);
    
    //log
    //comiface.set_rate(CBR_256000);
    comiface.set_rate(512000);
    
    comiface.log_state();
    
    //mass test
    for (int i = 0; i < 16; i++) mass_test_sync(comiface);
    printf("Summary translated - %d Bytes\n", 16 * 67 * 1024);



    //Restore
    comiface.write(&rw_ch[0], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %02hhx\n", buffer);
    comiface.write(&rw_ch[1], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %02hhx\n", buffer);
        
  
    comiface.close();
    //getchar();
    return (0);
}

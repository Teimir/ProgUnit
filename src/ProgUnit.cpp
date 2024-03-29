#define _CRT_SECURE_NO_WARNINGS 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//Локальные заголовки
#include "ComIface.h"

//can be used later
bool auto_connection(ComIface& c, int to = 256, int from = 0) {
    for (int i = from; i < to; ++i) {
        if (c.open(i, false)) {
            printf("Connected to COM %d\n", i);
            return true;
        }
    }
    printf("There is no COM id avalible from %d to %d\n", from, to);
    return false;
}
int mass_test_sync(ComIface& c) {
    int failures = 0;
    if (c.is_open()) {
        byte buffer, i = 0;
        while (--i) {
            if (c.write(&i, 1)) {
                if (c.read(&buffer, 1)) {
                    if (i != buffer) {
                        ++failures;
                    }
                }
                else {
                    ++failures;
                }
            }
            else {
                ++failures;
            }
            printf("Translated - %x,\trecieved - %x\t\t%s\n", i, buffer, i == buffer ? "OK" : "FAIL");
        }
        if (c.write(&i, 1)) {
            if (c.read(&buffer, 1)) {
                if (i != buffer) {
                    ++failures;
                }
            }
            else {
                ++failures;
            }
        }
        else {
            ++failures;
        }
        printf("Translated - %x,\trecieved - %x\t\t%s\n", i, buffer, i == buffer ? "OK" : "FAIL");
    }
    else {
        printf("Can`t test closed port\n");
        failures = 256;
    }
    return failures;
}





int _tmain(int argc, TCHAR* argv[]) {
    int numOfTests = 0;
    ComIface comiface(CBR_9600);
    byte data[] = { 0x1b, 0x1c, 0x12, 0x25 };
    byte sw_ch[] = { 0xff, 0xf2 };
    byte rw_ch[] = { 0xff, 0xf1 };
    byte buffer = 0;
    for (int i = 0; i < 256; i++) {
        if (comiface.open(i, 0, false)) {
            printf("COM %d id avalible! \n", i);
            comiface.close();
            numOfTests++;
        }
    }
    if (!numOfTests) {
        printf("No one COM-port is avalible");
        exit(0);
    }
    printf("Set number of port: ");
    scanf("%d", &numOfTests);
    comiface.open(numOfTests);
    printf("Set number of tests: ");
    scanf("%d", &numOfTests);
    comiface.log_state();
    int c = 0;
    //_tprintf (TEXT("Serial port %s successfully reconfigured.\n"), pcCommPort);
    for (int i = 0; i < numOfTests; i++) {
        if (!comiface.write(&data[i % 4], 1)) {
            printf("Write failed!\t");
        };
        if (!comiface.read(&buffer, 1)) {
            printf("Read failed!\t");
        };
        printf("Translated - %x, recieved - %x     ", data[i % 4], buffer);
        printf("%d  ", i+1);
        printf(data[i % 4] == buffer ? "YES\n" : "NO\n");
        c = data[i % 4] != buffer ? c + 1 : c;
    }
    printf("Errors - %d / Tests - %d\n", c, numOfTests);
    
    
    comiface.write(&sw_ch[0], 1);
    Sleep(4);
    comiface.read(&buffer, 1);
    printf("recieved - %x     ", buffer);
    comiface.write(&sw_ch[1], 1);
    Sleep(4);
    comiface.read(&buffer, 1);
    printf("recieved - %x     ", buffer);

    comiface.set_rate(CBR_115200);

    c = 0;
    //_tprintf (TEXT("Serial port %s successfully reconfigured.\n"), pcCommPort);
    for (int i = 0; i < numOfTests; i++) {
        if (!comiface.write(&data[i % 4], 1)) {
            printf("Write failed!\t");
        };
        Sleep(4);
        if (!comiface.read(&buffer, 1)) {
            printf("Read failed!\t");
        };
        printf("Translated - %x, recieved - %x     ", data[i % 4], buffer);
        printf("%d  ", i + 1);
        printf(data[i % 4] == buffer ? "YES\n" : "NO\n");
        c = data[i % 4] != buffer ? c + 1 : c;
    }
    printf("Errors - %d / Tests - %d\n", c, numOfTests);

    comiface.write(&rw_ch[0], 1);
    Sleep(4);
    comiface.read(&buffer, 1);
    printf("recieved - %x     ", buffer);
    comiface.write(&rw_ch[1], 1);
    Sleep(4);
    comiface.read(&buffer, 1);
    printf("recieved - %x     ", buffer);

    mass_test_sync(comiface);

    getchar();
    return (0);
}

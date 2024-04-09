#define _CRT_SECURE_NO_WARNINGS 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <algorithm>
//Локальные заголовки
#include "ComIface.h"

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
    if (!c.is_not_open()) {
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
    byte edata[8][8] = {
        {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
        {0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15}
    }; //to do - fill
    byte sw_ch[] = { 0xff, 0xf2 };
    byte rw_ch[] = { 0xff, 0xf1 };
    byte buffer = 0;
    byte ebuffer[8];
    for (int i = 0; i < 256; i++) {
        if (comiface.open(i, false)) {
            printf("COM %d id avalible! \n", i);
            comiface.close();
            numOfTests++;
        }
    }
    if (!numOfTests) {
        printf("No one COM-port is avalible");
        exit(1);
    }
    printf("Set number of port: ");
    scanf("%d", &numOfTests);
    comiface.open(numOfTests);
    if (comiface.is_not_open()) {
        exit(2);
    }
    printf("Set number of tests: ");
    scanf("%d", &numOfTests);
    comiface.log_state();
    int c = 0;
    for (int i = 0; i < numOfTests; ++i) {
        if (!comiface.write(&data[i % 4], 1)) {
            printf("Write failed!\t");
        };
        if (!comiface.read(&buffer, 1)) {
            printf("Read failed!\t");
        };
        printf(
            "%d\tTranslated - %x,\trecieved - %x\t%s",
            i,
            data[i % 4],
            buffer,
            data[i % 4] == buffer ? "YES\n" : "NO\n"
        );
        c = data[i % 4] != buffer ? c + 1 : c;
    }
    printf("Errors - %d / Tests - %d\n", c, numOfTests);


    //change rate
    comiface.write(&sw_ch[0], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %x\n", buffer);
    comiface.write(&sw_ch[1], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %x\n", buffer);
    //while (buffer == 0) {
    //    comiface.write(&sw_ch[1], 1);
    //    comiface.read(&buffer, 1);
    //    printf("recieved - %x\t\n", buffer);
    //}
    comiface.set_rate(CBR_115200);
    comiface.log_state();
    c = 0;
    //_tprintf (TEXT("Serial port %s successfully reconfigured.\n"), pcCommPort);
    for (int i = 0; i < numOfTests; i++) {
        if (!comiface.write(&data[i % 4], 1)) {
            printf("Write failed!\t");
        };
        if (!comiface.read(&buffer, 1)) {
            printf("Read failed!\t");
        };
        printf(
            "%d\tTranslated - %x,\trecieved - %x\t%s",
            i,
            data[i % 4],
            buffer,
            data[i % 4] == buffer ? "YES\n" : "NO\n"
        );
        c = data[i % 4] != buffer ? c + 1 : c;
    }
    printf("Errors - %d / Tests - %d\n", c, numOfTests);

    comiface.write(&rw_ch[0], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %x\n", buffer);
    comiface.write(&rw_ch[1], 1);
    comiface.read(&buffer, 1);
    printf("recieved - %x\n", buffer);
    c = 0;
    //test with 8 byte write/read
    for (int i = 0; i < numOfTests; ++i) {
        if (ARRAYSIZE(edata[i % 8]) != comiface.write(edata[i % 8], ARRAYSIZE(edata[i % 8]))) {
            printf("Write failed!\t");
        }
        if (ARRAYSIZE(ebuffer) == comiface.read(ebuffer, ARRAYSIZE(ebuffer))) {
            std::reverse(ebuffer, ebuffer + ARRAYSIZE(ebuffer));
        }
        else {
            printf("Read failed!\t");
        }
        printf(
            "%d\tTranslated - %x,\trecieved - %x\t%s",
            i,
            edata[i % 8],
            ebuffer,
            !memcmp(edata[i % 8], ebuffer, ARRAYSIZE(ebuffer)) ? "YES\n" : "NO\n"
        );
        c = data[i % 4] != buffer ? c + 1 : c;
    }
    printf("Errors - %d / Tests - %d\n", c, numOfTests);


    //mass_test_sync(comiface);
    comiface.close();
    getchar();
    return (0);
}

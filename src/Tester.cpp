#include "Header_Ts.h"
#include "ComIface.h"

int Test(ComIface comiface, int numOfTests) {
    byte data[] = { 0x1b, 0x1c, 0x12, 0x25 };
    byte buffer = 0;
    int c = 0;
    for (int i = 0; i < numOfTests; i++) {
        comiface.Write(&data[i % 4], 1);
        if (!comiface.Write(&data[i % 4], 1)) {
            printf("Write failed!\t");
            Sleep(10);
            i--;
            continue;
        };
        Sleep(4);
        comiface.Read(&buffer, 1);
        if (!comiface.Read(&buffer, 1)) {
            printf("Read failed!\t");
            printf("Translated - %x, recieved - %x     \n", data[i % 4], buffer);
            Sleep(10);
            i--;
            continue;
        };
        if (data[i % 4] != buffer) {
            printf("Translated - %x, recieved - %x     %d   NO \n", data[i % 4], buffer, i + 1);
            c++;
        }
        
    }

    printf("Errors - %d / Tests - %d\n", c, numOfTests);
    return 0;
}
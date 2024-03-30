#define _CRT_SECURE_NO_WARNINGS 

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//Локальные заголовки
#include "ComIface.h"
#include "Header_Ts.h"

int _tmain(int argc, TCHAR* argv[])
{
    int spd = 9600;
    int numOfTests = 0;
    ComIface comiface(0);
    byte data[] = { 0x1b, 0x1c, 0x12, 0x25 };
    byte sw_ch[] = { 0xff, 0xf2 };
    byte rw_ch[] = { 0xff, 0xf0 };
    byte buffer = 0;
    for (int i = 0; i < 256; i++) {
        comiface.ComNum = i;
        if (comiface.Open()) {
            printf("COM %d id avalible! \n", i);
            comiface.Close();
            numOfTests++;
        }
    }
    if (!numOfTests) {
        printf("No one COM-port is avalible");
        exit(0);
    }
    printf("Set number of port: ");
    scanf("%d", &numOfTests);
    comiface.ComNum = numOfTests;
    //comiface.BaudRate = CBR_115200;
    comiface.BaudRate = CBR_4800;
    comiface.Open();
    printf("Set number of tests: ");
    scanf("%d", &numOfTests);
    comiface.PrintState();
    
    Test(comiface);
    
    comiface.Write(&sw_ch[0], 1);
    Sleep(4);
    comiface.Read(&buffer, 1);
    printf("recieved - %x     ", buffer);
    comiface.Write(&sw_ch[1], 1);
    Sleep(4);
    comiface.Read(&buffer, 1);
    printf("recieved - %x     \n", buffer);

    comiface.ChangeRate(CBR_115200);
    //comiface.ChangeRate(CBR_9600);

    Test(comiface);

    comiface.Write(&rw_ch[0], 1);
    Sleep(4);
    comiface.Read(&buffer, 1);
    printf("recieved - %x     ", buffer);
    comiface.Write(&rw_ch[1], 1);
    Sleep(4);
    comiface.Read(&buffer, 1);
    printf("recieved - %x     ", buffer);

    comiface.Close();

    getchar();
    return (0);
}

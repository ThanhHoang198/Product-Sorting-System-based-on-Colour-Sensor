#include <16F877A.h>
#device ADC=16
#INCLUDE <stdlib.h>
#INCLUDE<string.h>

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES HS

#use delay(crystal=20000000)
#use FIXED_IO( A_outputs=PIN_A3,PIN_A2,PIN_A1,PIN_A0,PIN_A4 )
#use rs232(stream=com1,baud=9600,xmit=PIN_C6,rcv=PIN_c7))

#define S_0   PIN_A0
#define S_1   PIN_A1
#define S_2   PIN_A2
#define S_3   PIN_A3
#define READ   PIN_B0




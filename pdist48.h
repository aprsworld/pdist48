#include <18F25K80.h>
#device ADC=10
#device *=16


#if 1
/* no boot loader */


//	config FOSC = INTIO2
#fuses INTRC_IO
//	config FCMEN = OFF
#fuses NOFCMEN
//	config IESO = OFF 
#fuses NOIESO
//	config PWRTEN = ON
#fuses PUT
//	config BOREN = SBORDIS
//	config BORV = 0
#fuses BORV30
//	config WDTEN = SWDTDIS
//	config WDTPS = 32768
#fuses WDT32768
//	config MCLRE = OFF
#fuses NOMCLR
//	config STVREN = ON
#fuses STVREN
// 	config SOSCSEL = DIG
#fuses SOSC_DIG
// 	config XINST = OFF
#fuses NOXINST

#fuses NODEBUG


//#fuses WDT32768, NOMCLR
//#fuses NOIESO, , NOFCMEN, NODEBUG, NOSTVREN, SOSC_DIG


#else
/* leave from 0x7dc0 to 0x7fff alone for boot loader */
#org 0x7dc0, 0x7fff {}
#endif


#include <stdlib.h>
#use delay(clock=8000000, restart_wdt)

/* 
Parameters are stored in EEPROM
*/
#define PARAM_CRC_ADDRESS  0x000
#define PARAM_ADDRESS      PARAM_CRC_ADDRESS+2


/* UART1 - RS-485 connection modbus */
#use rs232(UART1,stream=MODBUS_SERIAL,baud=9600,xmit=PIN_C6,rcv=PIN_C7)	


#byte TXSTA=GETENV("SFR:txsta1")
#bit  TRMT=TXSTA.1
#byte ANCON0=GETENV("SFR:ancon0")
#byte ANCON1=GETENV("SFR:ancon1")



/* UART2 - FTDI cable */
#use rs232(UART2,stream=WORLD_SERIAL, baud=57600)	


#use standard_io(A)
#use standard_io(B)
#use standard_io(C)


#define LED_RED         PIN_B4
#define LED_GREEN       PIN_B5

//#define SER_TO_NET      PIN_B6
//#define SER_FROM_NET    PIN_B7
#define RS485_DE        PIN_C5

#define CTRL_A PIN_A7
#define CTRL_B PIN_A6
#define CTRL_C PIN_C0
#define CTRL_D PIN_C1
#define CTRL_E PIN_C2
#define CTRL_F PIN_C3
#define CTRL_G PIN_C4
#define CTRL_H PIN_B3


/* Modbus defines */
#define MODBUS_MODE_RTU     0
#define MODBUS_MODE_TCP_RTU 1

#define SERIAL_PREFIX_DEFAULT 'Z'
#define SERIAL_NUMBER_DEFAULT 2222

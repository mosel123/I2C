#include <18F4620.h>
#include <stdlib.h>

#use delay(clock=16000000)
#fuses HS, NOWDT, NOPROTECT, PUT, BROWNOUT, NOLVP, NOMCLR

#use i2c(Master, Fast, sda=PIN_C4, scl=PIN_C3)

#define RTC_DS1307_WRITTE_ADD 0xD0
#define RTC_DS1307_READ_ADD 0xD1
#define INIT_READ_DATA 0x00
#define ACK_ACUSE 0x00

//int BufferDatos[9];
//int cont_Buff = 0;
int hrs = 0, min = 0, sec = 0;
int print = 0;

#define __DEBUG_SERIAL__

#ifdef __DEBUG_SERIAL__
   #define TX_232        PIN_C6
   #define RX_232        PIN_C7
   #use RS232(BAUD=9600, XMIT=TX_232, BITS=8, PARITY=N, STOP=1, UART1, RCV=RX_232)
   #use fast_io(c)
#endif

int converBCD(BYTE bcd);
void rtc_get_time(BYTE& hr, BYTE& min, BYTE& sec);

void main(void)
{
   enable_interrupts(GLOBAL | INT_SSP);
   set_tris_c(0x19);
   rtc_get_time(hrs, min, sec);
   while(TRUE)
   {
      if(print == 1)
      {
         printf("Hora %u, Minuto %u, Segundos %u",hrs,min,sec);
         print = 0;
      }
   }
}

int converBCD(BYTE bcd)
{
   int cambia = 0;
   cambia = bcd;
   cambia >>= 1;
   cambia &= 0x78;
   
   return(cambia + (cambia>>2) + (bcd & 0x0f));
}

void rtc_get_time(BYTE& hr, BYTE& min, BYTE& sec) 
{
   i2c_start();   //Escritura
   i2c_write(0xD0);  //Codigo de escritura
   i2c_write(0x00);  //Puntero de la primera dirección
   i2c_start();   //Lectura
   i2c_write(0xD1);  //Codigo de lectura
   sec = converBCD(i2c_read()&0x7f); //Lectura de los 7 bit de los segundos
   min = converBCD(i2c_read()&0x7f); //Lectura de los 7 bit de los minutos
   hrs = converBCD(i2c_read(0)&0x3f); //Lectura de los 6 bit de las horas
   i2c_stop(); //Finaliza comunicación
}


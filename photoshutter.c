/*****************************************************
Universal autoshutter for digital cameras
26.02.2018 - ...
ATtiny2313 8 MHz
*****************************************************/

#include <tiny2313.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>

const int x0 = 24;
const int x1 = 48;
const int ON = 0;  // reversed for both transistor and buttons
const int OFF = 1;
int FOREVER = 1;
float U = 0;
int BASE = 0;
int TIMEOUT = 0;
int RELAY = 0;
int IR = 1;
float SUBTRACT = 0;
//1s/40kHz=25us total (6+19) pulse (microseconds)
//600us/25us=24 cycles
void pulse(int cycles){
    int c = 0;
    while(c<cycles){
        c++;
        PORTA.0 = ON;
        delay_us(6);
        PORTA.0 = OFF;
        delay_us(19);
    }
    delay_us(600);
}

//1800us*4 + 1200us*3 = 7200+3600 = 10800us = 10.8ms
void shutter(){
    pulse(x1); //1800us
    pulse(x0); //1200us
    pulse(x1); //1800us
    pulse(x1);
    pulse(x0);
    pulse(x1);
    pulse(x0);
}

//1800us*5 + 1200us*2 = 9000+2400 = 11400us = 11.4ms
void shutter2sec(){
    pulse(x1);
    pulse(x1);
    pulse(x0);
    pulse(x1);
    pulse(x1);
    pulse(x1);
    pulse(x0);
}

//1800us*2 + 1200us*5 = 3600+6000 = 9600us = 9.6ms
void videobutton(){
    pulse(x1);
    pulse(x0);
    pulse(x0);
    pulse(x1);
    pulse(x0);
    pulse(x0);
    pulse(x0);
}

//1800us*8 + 1200us*5 = 14400+6000 = 20400us = 20.4ms
void address(){
    pulse(x0);
    pulse(x1);
    pulse(x0);
    pulse(x1);
    pulse(x1);
    pulse(x1);
    pulse(x0);
    pulse(x0);
    pulse(x0);
    pulse(x1);
    pulse(x1);
    pulse(x1);
    pulse(x1);
}

void main(void)
{
// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif


//DDRx 0 - input pin
    //PORTxn 1 - pull-up enabled
//DDRx 1 - output pin

// Port A
//Should be high for out
//VOID RESET FREE RELAY
DDRA =0b0001;
PORTA=0b0011;
PORTA.0 = OFF;

// Port B
//All input
//high by default, low when shorted
//30 20 10 5 4 3 2 1
DDRB=0b00000000;
PORTB=0b11111111;

// Port D
//All input
//VOID RELAY_ON +0.5 +0.25  day  hour  min  sec 
DDRD=0b00000000;
PORTD=0b11111111;

delay_ms(200);

if(PINA.1 == ON){/*doesn't work as input*/}

if(PINB.0 == ON){U += 1;}
if(PINB.1 == ON){U += 2;}
if(PINB.2 == ON){U += 3;}
if(PINB.3 == ON){U += 4;}
if(PINB.4 == ON){U += 5;} 
if(PINB.5 == ON){U += 10;}
if(PINB.6 == ON){U += 20;}
if(PINB.7 == ON){U += 30;}

if(PIND.0 == ON){BASE = 1000;}
if(PIND.1 == ON){BASE = 60*1000;}
if(PIND.2 == ON){BASE = 60*60*1000;}
if(PIND.3 == ON){BASE = 24*60*60*1000;}
if(PIND.4 == ON){U += 0.25;}
if(PIND.5 == ON){U += 0.5;}

if(PIND.6 == ON){RELAY = 1;}

if(IR){
    SUBTRACT+= 73.4;
}
if(RELAY){
    SUBTRACT+= 200;
}

TIMEOUT = (int)( ((float)BASE * U)-SUBTRACT );

while(FOREVER){
    if(IR){
        //Sony IR start (73.4 ms total)
        shutter();
        address();
        delay_ms(11);
        shutter();
        address();
    }
    
    if(RELAY){
        //relay
        PORTA.0 = ON;
        delay_ms(200);
        PORTA.0 = OFF;  
    }

    
    if(!TIMEOUT){
        FOREVER = 0; //fire only this time if no timeout is set
    } else {
        delay_ms(TIMEOUT);
    }    

}


    
};

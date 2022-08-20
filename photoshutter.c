/*****************************************************
Universal autoshutter for digital cameras
26.02.2018 - 20.08.2022 by https://github.com/minch-yoda/
ATtiny2313 8 MHz
*****************************************************/

#include <tiny2313.h>
#include <delay.h>
//#include <stdio.h>
//#include <stdlib.h>

const int x0 = 24;
const int x1 = 48;
const int x2 = 96;

int U = 0;
int BASE = 0;
int TIMEOUT = 0;
int RELAY = 1;
int CMD = 1;
//1s/40kHz=25us total (6+19) pulse (microseconds)
//600us/25us=24 cycles
void pulse(int cycles){
    int c = 0;
    while(c<cycles){
        c++;         
        PORTA.0 = 0; //ON  reversed because we use transistor
        delay_us(6);
        PORTA.0 = 1; //OFF
        delay_us(19);
    }
    delay_us(600);
}

//1800us*4 + 1200us*3 +3000 = 7200+3600 +3000 = 13800us = 13.8ms
void shutter(){
    pulse(x2); //3000us
	pulse(x1); //1800us
    pulse(x0); //1200us
    pulse(x1); //1800us
    pulse(x1);
    pulse(x0);
    pulse(x1);
    pulse(x0);
}

//1800us*5 + 1200us*2 +3000 = 9000+2400+3000 = 14400us = 14.4ms
void shutter2sec(){
    pulse(x2); //3000us
	pulse(x1);
    pulse(x1);
    pulse(x1);
    pulse(x0);
    pulse(x1);
    pulse(x1);
    pulse(x0);
}

//1800us*2 + 1200us*5 +3000 = 3600+6000+3000 = 12600us = 12.6ms
void video(){
    pulse(x2); //3000us 
    pulse(x0);
    pulse(x0);
    pulse(x0);
	pulse(x1);
    pulse(x0);
    pulse(x0);
    pulse(x1);
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

delay_ms(100);

//0 means pin is shorted (ON) so we check for false value


if(!PIND.0){BASE = 1000;}      //seconds
if(!PIND.1){BASE = 60000;}     //minutes 60*1000
if(!PIND.2){BASE = 3600000;}   //hours 60*60*1000
if(!PIND.3){BASE = 86400000;}  //days 24*60*60*1000

if(!PINB.0){U += 1;}
if(!PINB.1){U += 2;}
if(!PINB.2){U += 3;}
if(!PINB.3){U += 4;}
if(!PINB.4){U += 5;} 
if(!PINB.5){U += 10;}
if(!PINB.6){U += 20;}
if(!PINB.7){U += 30;}

TIMEOUT = (BASE * U);

if(!PIND.4){TIMEOUT += BASE/4;}   //+0.25
if(!PIND.5){TIMEOUT += BASE/2;}   //+0.5

if(!PINA.1){CMD = 2;}             //shutter 2 sec (Nex5 native command)
if(!PIND.6){CMD = 3;}             //video
if(!PINA.1 && !PIND.6){CMD = 4;} //shutter once in x sec

if(TIMEOUT>=100){ //we don't want UINT_MAX
	TIMEOUT -= 100; //time buffer to make IR commands execution time identical
}
if(RELAY && CMD!=4 && TIMEOUT>=200){ //we don't want UINT_MAX, also we don't need to subtract in "shutter once in x sec" mode
	TIMEOUT -= 200; //accounting for relay interval
}

while(1){
    //Sony IR start 
    switch(CMD){
        case 1:	 //shutter
            shutter();
            address();
            delay_ms(11);
            shutter();
            address();
            delay_ms(20.6); //100 - 79.4
        break;
        case 2:  //shutter 2 sec
            shutter2sec();
            address();
            delay_ms(11);
            shutter2sec();
            address();
            delay_ms(19.4); //100 - 80.6
        break;
        case 3:  //video
            video();
            address();
            delay_ms(11);
            video();
            address();
			delay_ms(23); //100 - 77
        break;
        case 4:	 //shutter once in x sec
			delay_ms(TIMEOUT);
            shutter();
            address();
            delay_ms(11);
            shutter();
            address();
            delay_ms(20.6); //100 - 79.4
        break;
        default: break;
    }
    
    if(RELAY){
        //relay
        PORTA.0 = 0;  //ON
        delay_ms(200);
        PORTA.0 = 1;  //OFF
    }

    
    if(CMD!=4 && TIMEOUT){
        delay_ms(TIMEOUT);
    } else {
        break; //fire only this time if no timeout is set or the cmd is "shutter once in x sec"
    }    

}


    
};

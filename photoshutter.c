/*****************************************************
Universal autoshutter for digital cameras
26.02.2018 - ...
ATtiny2313 8 MHz
*****************************************************/

#include <tiny2313.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>

void delay_second(int n){
    while(n--){
        delay_ms(1000);
    }

}

void delay_minute(int n){
    while(n--){
        delay_ms(60000);
    }

}

void delay_hour(int n){
    while(n--){
        delay_ms(3600000);
    }

}

void delay_day(int n){
    while(n--){
        delay_ms(86400000);
    }

}
void delay_shot(){
      delay_ms(200);   //for camera to take a shot
}
void main(void)
{
// Declare your local variables here
int unit_count = 0;

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif


// Port A
//NOTHING RESET UNUSED_OUT RELAY
DDRA=0b0011;
//Should be high for out
PORTA=0x0011;

// Port B
// 30 20 10 5 4 3 2 1
//All input
DDRB=0b00000000;
//Should be high (we short it to ground to make it low?)
PORTB=0b00000000;

// Port D
//All input
//  CLCK  day  hour  min  sec  UNUSED UNUSED
DDRD=0b00000000;
//Should be low
PORTD=0b00000000;

PORTA.0 = 0;
delay_ms(200);

unit_count = 0;
if(PINB.0 == 1){unit_count = 1;}
if(PINB.1 == 1){unit_count = 2;}
if(PINB.2 == 1){unit_count = 3;}
if(PINB.3 == 1){unit_count = 4;}
if(PINB.4 == 1){unit_count = 5;} 
if(PINB.5 == 1){unit_count = 10;}
if(PINB.6 == 1){unit_count = 20;}
if(PINB.7 == 1){unit_count = 30;}
if(unit_count==0){
    unit_count=1; //no pins
}

//this SHIT SDOESNT'T WORK
//if(PIND.4 == 1){ //minutes
//    while(1){
//        //take photo    
//        PORTA.0 = 1;
//        delay_shot();
//        PORTA.0 = 0;
//        delay_minute(unit_count);
//    }
//} else if(PIND.3 == 1){ //hours
//    while(1){
//        //take photo    
//        PORTA.0 = 1;
//        delay_shot();
//        PORTA.0 = 0;
//        delay_hour(unit_count);
//    }
//} else if(PIND.2 == 1){ //days
//    while(1){
//        //take photo    
//        PORTA.0 = 1;
//        delay_shot();
//        PORTA.0 = 0;
//        delay_day(unit_count);
//    }
//}  else {  //seconds
//    while(1){
//        //take photo    
//        PORTA.0 = 1;
//        delay_shot();
//        PORTA.0 = 0;
//        delay_second(unit_count);
//    }
//}
    while(1){
        //take photo    
        PORTA.0 = 0;
        delay_ms(200);
        PORTA.0 = 1;
        //sleep
        //delay_ms(359800); //360000      6min   
        delay_ms(800);     //1sec
    }


    
};

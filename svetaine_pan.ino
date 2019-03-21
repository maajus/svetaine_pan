#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "config.h"               //package builtin configuration file
#include <Wire.h>

uint8_t Data = 0;
bool interruptFlag = false;

int ledPins[8] = {13,12,8,7,6,5,4,3};


/*******************************
  Setup Function
 *******************************/

void setup() {

    Serial.begin(9600);

    for(int i = 0; i<8; i++){
        pinMode(ledPins[i], OUTPUT);
    }

    pinMode(INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), interrupt, FALLING);

    Wire.begin();//start i2c
    PCF_write8(PCF_IN_ADDRESS,255); 
    /*PCF_read8(PCF_IN_ADDRESS);*/
    Serial.println("Setup op");

}

/*******************************
 Serial wait for commands 
 *******************************/

void serial_listen(){

       // read data from the connected client
        if (Serial.available() > 0) {
            int req = Serial.parseInt();
            Serial.print(req,DEC);
            updateLeds(req);
            while(Serial.read()!=-1);
    }
}

/*******************************
  Loop Function
 *******************************/
void loop()
{
    serial_listen();

    if(interruptFlag){

        uint8_t pin_num = PCF_detect_low_pin();

        if(pin_num != 8){

            char buf[2];
            sprintf(buf,"L%d",pin_num);
            Serial.print(buf);

            if(digitalRead(ledPins[pin_num]))
                digitalWrite(ledPins[pin_num], LOW);
            else
                digitalWrite(ledPins[pin_num], HIGH);

        }
        interruptFlag = false;
        _delay_ms(350);

    }

}

/*******************************
PCF gpio interrupt
 *******************************/

void interrupt(){

    interruptFlag = true;
}

void updateLeds(int data){

    for(uint8_t i = 0; i<8; i++){
        digitalWrite(ledPins[i], (data & (1 << i)));
    }

}


/*******************************
  End of the file
 *******************************/

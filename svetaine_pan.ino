#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "config.h"               //package builtin configuration file
#include <Wire.h>

uint8_t Data = 0;
uint8_t State = 0;
bool interruptFlag = false;

int ledPins[8] = {13,12,8,7,6,5,4,3};

int buttonMap[8] = {4,2,
                    0,5,
                    1,7,
                    3,6};


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

}

/*******************************
 Serial wait for commands 
 *******************************/

void serial_listen(){

        if (Serial.available() > 0) {
            int data = Serial.read();
            State = data;
            updateLeds(data);
            /*while(Serial.read()!=-1);*/
    }
}

/*******************************
  Loop Function
 *******************************/
void loop()
{
    serial_listen();

    if(interruptFlag){

        while(digitalRead(INT_PIN) == HIGH);
        uint8_t pin_num = PCF_detect_low_pin();

        if(pin_num != 8){

            char buf[2];

            if(pin_num == 7){
                //sprintf(buf,"N");
                Serial.write(0);
            }
            else{
                uint8_t data = State;
                data ^= (1 << buttonMap[pin_num]);
                Serial.write(data);
                /*sprintf(buf,"L%d",buttonMap[pin_num]);*/
            }
            //Serial.print(buf);

        }
        delay(50);
        interruptFlag = false;
        interrupts();

    }

}

/*******************************
PCF gpio interrupt
 *******************************/

void interrupt(){

    interruptFlag = true;
    noInterrupts();
}

void updateLeds(int data){

    int i = 0;
    for(i = 0; i<8; i++){
        bool val = true;

        if((data & (1 << i)))
            val = false;

        for(int j = 0; j < 8; j++){
            if(i == buttonMap[j])
                digitalWrite(ledPins[j], val);
        }

    }

}


/*******************************
  End of the file
 *******************************/

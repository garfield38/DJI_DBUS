/*
  DJI DBUS data decoder library example
  (c) S. Driussi 20141215
  Not for commercial use

  Work started from mikeshub Futaba library
  https://github.com/mikeshub/FUTABA_SBUS
  
  Requires Teensy 2.0 board for dual serial communication or Atmega 328p for decoder to PPM only
  Refer to naza_dbus_decoder_wiring.jpg diagram for proper connection
*/

#include <DJI_DBUS.h>
#include <Servo.h> 
#include <avr/eeprom.h>

boolean running = false;
const byte servos = 7;

#ifdef __AVR_ATmega32U4__
  DJI_DBUS dBus(Serial1);
  uint8_t PWM_PIN[servos] = {0,1,2,3,4,5,6};
  static uint16_t rcValue[servos]  = {1502, 1502, 1502, 1502, 1502, 1502, 1502};
  int led = 11; // Teensy 2.0
  int progPin = 13;
#else
  DJI_DBUS dBus(Serial);
  uint8_t PWM_PIN[servos] = {2,3,4,5,6,7,8};
  int led = 13; // Arduino Pro Mini
  int progPin = 9;
#endif

Servo myservo[servos];
uint32_t currTime, displayTime = 0;
uint8_t i;

void setup(){
  eeprom_read_block((void*)&rcValue, (void*)0, sizeof(rcValue));
  pinMode(led, OUTPUT);
  pinMode(progPin, INPUT);
  digitalWrite(progPin, HIGH);      // turn on pullup resistor 
#ifdef __AVR_ATmega32U4__
  Serial.begin(115200);
  Serial.println("DBUS Status");
#endif
  dBus.begin();
  myservo[0].attach(PWM_PIN[0]); 
  myservo[1].attach(PWM_PIN[1]); 
  myservo[2].attach(PWM_PIN[2]); 
  myservo[3].attach(PWM_PIN[3]); 
  myservo[4].attach(PWM_PIN[4]); 
  myservo[5].attach(PWM_PIN[5]); 
  myservo[6].attach(PWM_PIN[6]); 
}

void loop(){
  if (dBus.Failsafe() == DBUS_SIGNAL_OK) digitalWrite(led, HIGH);
  dBus.FeedLine();
  digitalWrite(led, LOW);
  currTime = millis();
  if (digitalRead(progPin) == LOW && currTime < 5000) {
    for (i = 0; i < servos; i++) {
      rcValue[i] = dBus.channels[i];
    }
    eeprom_write_block((const void*)&rcValue, (void*)0, sizeof(rcValue));
    while (progPin) {
      running = !running;
      digitalWrite(led, running);
      delay(100);
    }
  }
  if (dBus.toChannels == 1){
    dBus.UpdateServos();
    dBus.UpdateChannels();
    dBus.toChannels = 0;

    for (i = 0; i < servos; i++) {
      if (dBus.Failsafe() == DBUS_SIGNAL_OK) myservo[i].writeMicroseconds(dBus.channels[i]);
      else myservo[i].writeMicroseconds(rcValue[i]);
    }
    
#ifdef __AVR_ATmega32U4__
    if(displayTime < currTime) {
      displayTime = currTime + 1000;
      printDBUSStatus();
    }
#endif

  }
}

void printDBUSStatus()
{
  Serial.print("Thr ");
  Serial.print(dBus.channels[2]);
  Serial.print(" Ail ");
  Serial.print(dBus.channels[0]);
  Serial.print(" Ele ");
  Serial.print(dBus.channels[1]);
  Serial.print(" Rud ");
  Serial.print(dBus.channels[3]);
  Serial.print(" Fm ");
  Serial.print(dBus.channels[6]);
  Serial.print(" IOC ");
  Serial.print(dBus.channels[5]);
  Serial.print(" Pit  ");
  Serial.print(dBus.channels[4]);
  Serial.print(" Stat ");
  if (dBus.Failsafe() == DBUS_SIGNAL_FAILSAFE) {
    Serial.print("FailSafe");
  } else if (dBus.Failsafe() == DBUS_SIGNAL_LOST) {
    Serial.print("Signal Lost");
     for (i = 0; i < servos; i++) {
      Serial.print(" ");
      Serial.print(rcValue[i]);
    }
  } else if (dBus.Failsafe() == DBUS_SIGNAL_OK) {
    Serial.print("OK");
  }
  Serial.println(".");
}

/*
  DJI DBUS data decoder library
  (c) S. Driussi 20141215
  Not for commercial use

  Work started from mikeshub Futaba library
  https://github.com/mikeshub/FUTABA_SBUS
  
  Refer to naza_dbus_decoder_wiring.jpg diagram for proper connection
*/
#ifndef DJI_DBUS_h
#define DJI_DBUS_h

#include <Arduino.h>


#define DBUS_SIGNAL_OK          0x00
#define DBUS_SIGNAL_LOST        0x01
#define DBUS_SIGNAL_FAILSAFE    0x03
#define BAUDRATE 100000
#define port Serial1
//#define ALL_CHANNELS


class DJI_DBUS
{
	public:
		DJI_DBUS(HardwareSerial & serial) : _serial (serial) {}
		uint8_t sbusData[25];
		int16_t channels[18];
		int16_t servos[18];
		uint8_t  failsafe_status;
		int sbus_passthrough;
		int toChannels;
		void begin();
		int16_t Channel(uint8_t ch);
		uint8_t DigiChannel(uint8_t ch);
		void Servo(uint8_t ch, int16_t position);
		void DigiServo(uint8_t ch, uint8_t position);
		uint8_t Failsafe(void);
		void PassthroughSet(int mode);
		int PassthroughRet(void);
		void UpdateServos(void);
		void UpdateChannels(void);
		void FeedLine(void);
	private:
                HardwareSerial & _serial;
		uint8_t byte_in_sbus;
		uint8_t bit_in_sbus;
		uint8_t ch;
		uint8_t bit_in_channel;
		uint8_t bit_in_servo;
		uint8_t inBuffer[25];
		int bufferIndex;
		uint8_t inData;
		int feedState;

};

#endif

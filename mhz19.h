#ifndef MHZ19_H
#define MHZ19_H

#include <Arduino.h>

#define MHZ19_TIMEOUT 3
#define MHZ19_SAMPLING 2
#define MHZ19_READY 1
#define MHZ19_STANDBY 0


void ICACHE_RAM_ATTR isr_handleCO2(); //function for interrupt

class sensorMHZ19 {
	public:
		sensorMHZ19(int pin, int sensorLimit);
		void _isr_handleCO2();//call ICACHE_RAM_ATTR to avoid errors 
		void readCO2(); //
		int status();
		float getVal();
		void setTimoutperSample(int millisec);
	private:
		int _CO2; //pin for input
		int max_samples; // sum of sample values for mean value
		int sample; //active sample
		long startlow;
		long timelow;
		boolean skiplow;
		long starthigh;
		long timehigh;
		float value;
		int val_status;
		int _sensorLimit;
		long startCO2;
		long _timeoutS;
		long timeoutpersample;
};
#endif

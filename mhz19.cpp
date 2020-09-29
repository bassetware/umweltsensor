#include <Arduino.h>
#include "mhz19.h"

sensorMHZ19 *_mhz;

void isr_handleCO2() {
  _mhz->_isr_handleCO2();
}

sensorMHZ19::sensorMHZ19(int pin, int sensorLimit) {
	this->_CO2 = pin;
	this->_sensorLimit = sensorLimit;
	pinMode(this->_CO2, INPUT);
	this->timeoutpersample = 5000;
	_mhz = this;
	this->val_status = MHZ19_STANDBY;
}

void sensorMHZ19::_isr_handleCO2() {
	int value = digitalRead(_CO2);
	unsigned long ms = micros();
	if(value == HIGH) {
		if(this->skiplow) {this->skiplow = false;}
		if(this->timelow > 0 && this->timehigh > 0) {
			//we got a time for high and for low, now we can calculate the pwm value;
			float val_pwm = (float)(this->timehigh - 2) / (this->timehigh + this->timelow - 4);
			this->value = (val_pwm * this->_sensorLimit); 
				detachInterrupt(digitalPinToInterrupt(this->_CO2));
				this->val_status = MHZ19_READY;
				return;
			}
		this->starthigh = ms;
		this->timelow = this->startlow > 0 ? (float)(ms - this->startlow) / 1000 : 0;
	}
	else {
		if(this->skiplow) {
			this->skiplow = false;
			return;
		}
		this->startlow = ms;
		this->timehigh = this->starthigh > 0 ? (float)(ms - this->starthigh) / 1000 : 0;
	}
}

void sensorMHZ19::readCO2() { //void (*ISR)(void)
	this->skiplow = true; // ensure the FIRST change is from low to HIGH
	this->value = 0;
	this->sample = 0;
	this->_timeoutS = millis() + this->timeoutpersample;
	this->val_status = MHZ19_SAMPLING;
	this->starthigh  = 0;
	this->startlow = 0;
	this->timehigh = 0;
	this->timelow = 0;
	attachInterrupt(digitalPinToInterrupt(this->_CO2), &isr_handleCO2, CHANGE); //attach an interrput to measure high and lowtime
  	
}

int sensorMHZ19::status() {
	if(millis() > this->_timeoutS && this->val_status == MHZ19_SAMPLING) {
		this->val_status = MHZ19_TIMEOUT;
		detachInterrupt(digitalPinToInterrupt(this->_CO2));
	}
	return this->val_status;
}

float sensorMHZ19::getVal() {
	return this->value;
 this->val_status = MHZ19_STANDBY;
}

void sensorMHZ19::setTimoutperSample(int millisec) {
	this->_timeoutS = millisec;
}

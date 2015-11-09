/*
 * LM335_tools.c
 *
 *  Created on: Nov 9, 2015
 *      Author: Jesse
 */

float getVoltage(int raw, float pinVoltage) {
	return (raw * pinVoltage) / 1024.0;
}

float getKelvin(int raw, float pinVoltage) {
	return getVoltage(raw, pinVoltage)/0.01;
}

float getCelsius(int raw, float pinVoltage) {
	return getKelvin(raw, pinVoltage) - 273.15;
}

float getFahrenheit(int raw, float pinVoltage) {
	return (getCelsius(raw, pinVoltage) * 1.8) + 32;
}

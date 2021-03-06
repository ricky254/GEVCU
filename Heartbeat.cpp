/*
 * Heartbeat.c
 *
 Copyright (c) 2013 Collin Kidder, Michael Neuweiler, Charles Galpin

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include "config.h"
#include "Heartbeat.h"

Heartbeat::Heartbeat() {
	led = false;
	throttleDebug = false;
}

void Heartbeat::setup() {
	TickHandler::getInstance()->detach(this);

	TickHandler::getInstance()->attach(this, CFG_TICK_INTERVAL_HEARTBEAT);
}

void Heartbeat::setThrottleDebug(bool debug) {
	throttleDebug = debug;
}

bool Heartbeat::getThrottleDebug() {
	return throttleDebug;
}

void Heartbeat::handleTick() {
	// Print a dot if no other output has been made since the last tick
	if (Logger::getLastLogTime() < lastTickTime) {
		SerialUSB.print('.');
		if ((++dotCount % 80) == 0) {
			SerialUSB.println();
		}
	}
	lastTickTime = millis();

	if (led) {
		digitalWrite(BLINK_LED, HIGH);
	} else {
		digitalWrite(BLINK_LED, LOW);
	}
	led = !led;

	if (throttleDebug) {
		MotorController *motorController = DeviceManager::getInstance()->getMotorController();
		Throttle *accelerator = DeviceManager::getInstance()->getAccelerator();
		Throttle *brake = DeviceManager::getInstance()->getBrake();

		Logger::console("");
		if (motorController) {
			Logger::console("Motor Controller Status: isRunning: %T isFaulted: %T", motorController->isRunning(), motorController->isFaulted());
		}
		Logger::console("A0: %d, A1: %d, A2: %d, A3: %d", getAnalog(0), getAnalog(1), getAnalog(2), getAnalog(3));
		Logger::console("D0: %d, D1: %d, D2: %d, D3: %d", getDigital(0), getDigital(1), getDigital(2), getDigital(3));
		if (accelerator) {
			Logger::console("Throttle Status: isFaulted: %T level: %i", accelerator->isFaulted(), accelerator->getLevel());
			RawSignalData *rawSignal = accelerator->acquireRawSignal();
			Logger::console("Throttle rawSignal1: %d, rawSignal2: %d", rawSignal->input1, rawSignal->input2);
		}
		if (brake) {
			Logger::console("Brake Output: %i", brake->getLevel());
			RawSignalData *rawSignal = brake->acquireRawSignal();
			Logger::console("Brake rawSignal1: %d", rawSignal->input1);
		}
	}
}


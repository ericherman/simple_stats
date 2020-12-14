/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* simple_stats_arduino_tests_arduino.ino */
/* Copyright (C) 2020 Eric Herman */

#include <Arduino.h>
#include <HardwareSerial.h>

#include "eembed-arduino.h"

int test_sstats_basic(int verbose);

/* globals */
uint32_t loop_count;

void setup(void)
{
	Serial.begin(9600);

	eembed_arduino_serial_log_init();

	delay(50);

	Serial.println("Begin");

	loop_count = 0;
}

void loop(void)
{
	Serial.println("=================================================");
	++loop_count;
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	int verbose = 1;
	int error = test_sstats_basic(verbose);

	Serial.println("=================================================");
	if (error) {
		Serial.println("FAIL");
	} else {
		Serial.println("SUCCESS");
	}
	Serial.println("=================================================");

	uint32_t delay_ms = 10 * 1000;
	delay(delay_ms);
}

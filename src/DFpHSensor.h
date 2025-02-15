/*
 * This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2024 Sam Groveman
 * 
 * External libraries needed:
 * ArduinoJSON: https://arduinojson.org/
 * DFRobot_ESP_PH: https://github.com/GreenPonik/DFRobot_ESP_PH_BY_GREENPONIK
 *
 * pH Sensor: https://www.dfrobot.com/product-2069.html
 *
 * Contributors: Sam Groveman
 */

#pragma once
#include <GenericAnalogInput.h>
#include <Storage.h>
#include <ArduinoJson.h>
#include <DFRobot_ESP_PH.h>

class DFpHSensor : public GenericAnalogInput {
	public:
		DFpHSensor(int Pin = A0, String ConfigFile = "DFpH.json");
		bool begin();
		bool takeMeasurement();
		
	protected:
		/// @brief pH probe
		DFRobot_ESP_PH ph;
};
/*
 * This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2024 Sam Groveman
 * 
 * External libraries needed:
 * ArduinoJSON: https://arduinojson.org/
 * 
 * Based off of DFRobot_ESP_PH: https://github.com/GreenPonik/DFRobot_ESP_PH_BY_GREENPONIK
 *
 * pH Sensor: https://www.dfrobot.com/product-2069.html
 *
 * Contributors: Sam Groveman
 */

#pragma once
#include <GenericAnalogInput.h>
#include <Storage.h>
#include <ArduinoJson.h>

class DFpHSensor : public GenericAnalogInput {
	public:
		/// @brief Settings for pH sensor
		struct {
			/// @brief Slope of calibration curve
			float slope = -0.0169033130494;

			/// @brief Y-intercept of calibration curve
			float intercept = 7.0;
		
		} ph_config;

		DFpHSensor(String Name, int Pin = 36, String ConfigFile = "DFpH.json");
		bool begin();
		bool takeMeasurement();
		String getConfig();
		bool setConfig(String config, bool save);
		std::tuple<Sensor::calibration_response, String> calibrate(int step);

	protected:
		float getPHValue(int voltage);
		JsonDocument addAdditionalConfig();
};
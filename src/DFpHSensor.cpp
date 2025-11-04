#include "DFpHSensor.h"

/// @brief Creates a pH sensor
/// @param Name The device name
/// @param Pin The analog pin to use
/// @param ConfigFile The name of the configuration file to use
DFpHSensor::DFpHSensor(String Name, int Pin, String ConfigFile) : GenericAnalogInput(Name, Pin, ConfigFile) {}

/// @brief Starts a Data Template object
/// @return True on success
bool DFpHSensor::begin() {
	Description.parameterQuantity = 1;
	Description.type = "Water Sensor";
	Description.parameters = {"pH"};
	Description.units = {"pH"};
	values.resize(Description.parameterQuantity);
	bool result = false;
	// Create settings directory if necessary
	if (!checkConfig(config_path)){
		result = saveConfig(config_path, getConfig());
	} else {
		// Load settings
		result = setConfig(Storage::readFile(config_path), false);
	}
	return result;
}

/// @brief Takes a measurement
/// @return True on success
bool DFpHSensor::takeMeasurement() {
	int ph_voltage = analogToMV(getAnalogValue(analog_config.RollingAverage));
	values[0] = getPHValue(ph_voltage);
	return true;
}

/// @brief Gets the current config
/// @return A JSON string of the config
String DFpHSensor::getConfig() {
	// Allocate the JSON document
	JsonDocument doc = addAdditionalConfig();

	// Create string to hold output
	String output;
	
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool DFpHSensor::setConfig(String config, bool save) {
	if (GenericAnalogInput::setConfig(config, false)) {
		// Allocate the JSON document
		JsonDocument doc;
		// Deserialize file contents
		DeserializationError error = deserializeJson(doc, config);
		// Test if parsing succeeds.
		if (error) {
			Logger.print(F("Deserialization failed: "));
			Logger.println(error.f_str());
			return false;
		}
		ph_config.intercept = doc["cal_intercept"].as<float>();
		ph_config.slope = doc["cal_slope"].as<float>();
		if (save) {
			return saveConfig(config_path, config);
		}
		return true;
	}
	return false;
}

/// @brief Used to calibrate sensor
/// @param step The calibration step to execute for multi-step calibration processes
/// @return A tuple with the fist element as a Sensor::calibration_response and the second an optional message String accompanying the response
std::tuple<Sensor::calibration_response, String> DFpHSensor::calibrate(int step) {
	Logger.println("Calibrating pH sensor, step " + String(step));
	std::tuple<Sensor::calibration_response, String> response;
	int neutralVoltage = 1500;
	int acidVoltage = 2032.44;
	switch (step) {
		case 0:
			response = { Sensor::calibration_response::NEXT, "Place sensor in pH 4.0 solution and wait for it to stabilize." };
			break;
		case 1:
			acidVoltage = analogToMV(getAnalogValue(false));
			for (int i = 0; i < 9; i++) {
                acidVoltage += analogToMV(getAnalogValue(false));
            }
			acidVoltage /= 10;
			response = { Sensor::calibration_response::NEXT, "Place sensor in pH 7.0 and wait for it to stabilize." };
			break;
		case 2:
			neutralVoltage = analogToMV(getAnalogValue(false));
			for (int i = 0; i < 9; i++) {
                neutralVoltage += analogToMV(getAnalogValue(false));
            }
			neutralVoltage /= 10;
    		ph_config.slope = (7.0 - 4.0) / ((neutralVoltage - 1500.0) / 3.0 - (acidVoltage - 1500.0) / 3.0);
   			ph_config.intercept = 7.0 - ph_config.slope * (neutralVoltage - 1500.0) / 3.0;
			response = { Sensor::calibration_response::DONE, "Calibration complete." };
			break;
		default:
		response = { Sensor::calibration_response::ERROR, "No such calibration step: " + String(step) };
		break;
	}
	return response;
}


/// @brief Converts a voltage value to pH
/// @param voltage The electrode voltage in mV
/// @return The pH
float DFpHSensor::getPHValue(int voltage) {
    return ph_config.slope * (voltage - 1500.0) / 3.0 + ph_config.intercept;
}

/// @brief Collects all the base class parameters and additional parameters
/// @return a JSON document with all the parameters
JsonDocument DFpHSensor::addAdditionalConfig() {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize settings contents
	DeserializationError error = deserializeJson(doc, GenericAnalogInput::getConfig());
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return doc;
	}
	doc["cal_slope"] = ph_config.slope;
	doc["cal_intercept"] = ph_config.intercept;

	return doc;
}
#include "DFpHSensor.h"

/// @brief Creates a soil moisture sensor
/// @param Pin The analog pin to use
/// @param ConfigFile The name of the configuration file to use
DFpHSensor::DFpHSensor(int Pin, String ConfigFile) : GenericAnalogInput(Pin, ConfigFile) {}

/// @brief Starts a Data Template object
/// @return True on success
bool DFpHSensor::begin() {
	Description.parameterQuantity = 1;
	Description.type = "Water Sensor";
	Description.name = "pH Sensor";
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
	values[0] = ph.readPH(ph_voltage, 25); // The temperature value here doesn't seem to be used in any way
	return true;
}
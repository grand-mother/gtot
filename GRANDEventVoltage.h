//
// Created by lewhoo on 17/02/2022.
//

#ifndef GTOT_GRANDEVENTVOLTAGE_H
#define GTOT_GRANDEVENTVOLTAGE_H

#include <vector>
//#include "scope.h"
#include "Traces.h"
#include "TTree.h"
#include "TTimeStamp.h"
#include "TLeaf.h"
#include "GRANDEventADC.h"

using namespace std;

class GRANDEventVoltage
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:
	// Better have local storage for all the variables if one day we don't init from the structure
	// Common for the whole event
//	unsigned int event_size;
	unsigned int run_number;
	unsigned int event_number;
	// ?
	unsigned int t3_number;
	// ?
	unsigned int first_du;
	unsigned int time_seconds;
	unsigned int time_nanoseconds;
	unsigned int event_type;
	unsigned int event_version;
	//! Number of Digitizer Units in the event - basically the antennas count
	unsigned int du_count;
	// Separate for each DU (antenna) in the event
	// As number of triggered antennas is variable, we need vectors here
//	vector<unsigned short> event_id;
	vector<unsigned short> du_id;
	vector<unsigned int> du_seconds;
	vector<unsigned int> du_nanoseconds;
//	vector<double> du_time;
	// ?
	vector<unsigned short> trigger_position;
	// ?
	vector<unsigned short> trigger_flag;
	vector<unsigned short> atm_temperature;
	vector<unsigned short> atm_pressure;
	vector<unsigned short> atm_humidity;
	vector<unsigned short> acceleration_x;
	vector<unsigned short> acceleration_y;
	vector<unsigned short> acceleration_z;
//	vector<unsigned short> battery_adc;
	// ToDo: Is this the same as event_version for the whole event?
	vector<unsigned short> event_version_in;
//	vector<unsigned short> adc_sampling_frequency;
//	vector<unsigned short> adc_sampling_resolution;
//	vector<unsigned short> adc_input_channels;
//	vector<unsigned short> adc_enabled_channels;
//	vector<unsigned short> adc_samples_count_total;
//	vector<unsigned short> adc_samples_count_channel0;
//	vector<unsigned short> adc_samples_count_channel1;
//	vector<unsigned short> adc_samples_count_channel2;
//	vector<unsigned short> adc_samples_count_channel3;
	// ?
	vector<unsigned short> trigger_pattern;
	// ?
	vector<unsigned short> trigger_rate;
//	vector<unsigned short> clock_tick;
//	vector<unsigned int> clock_tics_per_second;
//	vector<float> gps_offset;
//	vector<unsigned short> gps_leap_second;
//	vector<unsigned short> gps_status;
//	vector<unsigned short> gps_alarms;
//	vector<unsigned short> gps_warnings;
	vector<unsigned int> gps_time;
	vector<double> gps_long;
	vector<double> gps_lat;
	vector<double> gps_alt;
//	vector<float> gps_temp;
//	vector<vector<unsigned short>> digi_ctrl;
	// ?
	vector<vector<unsigned short>> digi_prepost_trig_windows;
//	vector<vector<unsigned short>> channel_properties0;
//	vector<vector<unsigned short>> channel_properties1;
//	vector<vector<unsigned short>> channel_properties2;
//	vector<vector<unsigned short>> channel_properties3;
//	vector<vector<unsigned short>> channel_trig_settings0;
//	vector<vector<unsigned short>> channel_trig_settings1;
//	vector<vector<unsigned short>> channel_trig_settings2;
//	vector<vector<unsigned short>> channel_trig_settings3;
	// ToDo: What is it?
	// ?
	vector<unsigned short> ioff;
	vector<vector<float>> x;
	vector<vector<float>> y;
	vector<vector<float>> z;

	//! The TTree for holding the data
	TTree *teventvoltage;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! General constructor
	GRANDEventVoltage();
	//! Constructor computing values from GRANDEventADC
	GRANDEventVoltage(GRANDEventADC *adc);

private:
	//! Clear the vectors for another fill
	void ClearVectors();

	//! Converts the ADC traces from teventadc into Voltage traces
	void ADCs2Voltages(GRANDEventADC *adc);

	//! Converts a specific ADC trace from teventadc into a Voltage trace
	void ADC2Voltage(int du_num, GRANDEventADC *adc);

};



#endif //GTOT_GRANDEVENTVOLTAGE_H

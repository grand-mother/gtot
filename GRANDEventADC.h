//
// Created by lewhoo on 19/01/2022.
//

#ifndef GTOT_GRANDEVENTADC_H
#define GTOT_GRANDEVENTADC_H

#include <vector>
#include "scope.h"
#include "Traces.h"
#include "TTree.h"

using namespace std;

class GRANDEventADC
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:
	// Better have local storage for all the variables if one day we don't init from the structure
	// Common for the whole event
	unsigned int event_size;
	unsigned int run_number;
	unsigned int event_number;
	unsigned int t3_number;
	unsigned int first_du;
	unsigned int time_seconds;
	unsigned int time_nanoseconds;
	unsigned int event_type;
	unsigned int event_version;
	//! Number of Digitizer Units in the event - basically the antennas count
	unsigned int du_count;
	// Separate for each DU (antenna) in the event
	// As number of triggered antennas is variable, we need vectors here
	vector<unsigned short> event_id;
	vector<unsigned short> du_id;
	vector<unsigned short> du_second;
	vector<unsigned short> du_nanosecond;
	vector<unsigned short> trigger_position;
	vector<unsigned short> trigger_flag;
	vector<unsigned short> atm_temperature;
	vector<unsigned short> atm_pressure;
	vector<unsigned short> atm_humidity;
	vector<unsigned short> acceleration_x;
	vector<unsigned short> acceleration_y;
	vector<unsigned short> acceleration_z;
	vector<unsigned short> battery_adc;
	// ToDo: Is this the same as event_version for the whole event?
	vector<unsigned short> event_version_in;
	vector<unsigned short> adc_sampling_frequency;
	vector<unsigned short> adc_sampling_resolution;
	vector<unsigned short> adc_input_channels;
	vector<unsigned short> adc_enabled_channels;
	vector<unsigned short> adc_samples_count_total;
	vector<unsigned short> adc_samples_count_channel0;
	vector<unsigned short> adc_samples_count_channel1;
	vector<unsigned short> adc_samples_count_channel2;
	vector<unsigned short> adc_samples_count_channel3;
	vector<unsigned short> trigger_pattern;
	vector<unsigned short> trigger_rate;
	vector<unsigned short> clock_tick;
	vector<unsigned short> clock_tics_per_second;
	vector<unsigned short> gps_offset;
	vector<unsigned short> gps_leap_second;
	vector<unsigned short> gps_status;
	vector<unsigned short> gps_alarms;
	vector<unsigned short> gps_warnings;
	vector<unsigned short> gps_date;
	vector<unsigned short> gps_time;
	vector<unsigned short> gps_long;
	vector<unsigned short> gps_lat;
	vector<unsigned short> gps_alt;
	vector<unsigned short> gps_temp;
	vector<vector<unsigned short>> digi_ctrl;
	vector<vector<unsigned short>> digi_prepost_trig_windows;
	vector<vector<unsigned short>> channel_properties0;
	vector<vector<unsigned short>> channel_properties1;
	vector<vector<unsigned short>> channel_properties2;
	vector<vector<unsigned short>> channel_properties3;
	vector<vector<unsigned short>> channel_trig_settings0;
	vector<vector<unsigned short>> channel_trig_settings1;
	vector<vector<unsigned short>> channel_trig_settings2;
	vector<vector<unsigned short>> channel_trig_settings3;
	// ToDo: What is it?
	vector<unsigned short> ioff;
	vector<vector<short>> adc_track0;
	vector<vector<short>> adc_track1;
	vector<vector<short>> adc_track2;
	vector<vector<short>> adc_track3;

	//! The TTree for holding the data
	TTree *teventadc;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! Set the object variables from pointer intialised by Charles' functions from Traces.c
	int SetValuesFromPointers(unsigned short *pevent);

	GRANDEventADC();

private:
	//! Clear the vectors for another fill
	void ClearVectors();

};


#endif //GTOT_GRANDEVENTADC_H

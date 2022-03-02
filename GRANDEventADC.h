//
// Created by lewhoo on 19/01/2022.
//

#ifndef GTOT_GRANDEVENTADC_H
#define GTOT_GRANDEVENTADC_H

#include <vector>
#include "scope.h"
#include "Traces.h"
#include "TTree.h"
#include "TTimeStamp.h"

using namespace std;

class GRANDEventADC
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:
	// Better have local storage for all the variables if one day we don't init from the structure
	// Common for the whole event
	//! Event size
	unsigned int event_size;
	//! Run number
	unsigned int run_number;
	//! Event in the run number
	unsigned int event_number;
	//! ?? Unknown
	unsigned int t3_number;
	//! ?? First data unit that triggered in the event?
	unsigned int first_du;
	//! ?? Event unix time, but what does it correspond to really?
	unsigned int time_seconds;
	//! ?? Event nanoseconds
	unsigned int time_nanoseconds;
	//! ?? Trigger type 0x1000 10 s trigger and 0x8000 random trigger, else shower
	unsigned int event_type;
	//! ?? Event version - version of exactly what?
	unsigned int event_version;
	//! Number of Digitizer Units in the event - basically the antennas count
	unsigned int du_count;
	// Separate for each DU (antenna) in the event
	// As number of triggered antennas is variable, we need vectors here
	//! ?? What is it? The same as event_num above?
	vector<unsigned short> event_id;
	//! Data Unit (antenna) ID
	vector<unsigned short> du_id;
	//! ?? Unix time of... the start of the traces for this DU?
	vector<unsigned int> du_seconds;
	//! Nanoseconds for this DU
	vector<unsigned int> du_nanoseconds;
	//! ?? Trigger position (sample number of trigger) - is it the trigger position in the trace?
	vector<unsigned short> trigger_position;
	//! ?? What is it? Labeled as T3 flag
	vector<unsigned short> trigger_flag;
	//! Atmospheric temperature (read via I2C)
	vector<unsigned short> atm_temperature;
	//! Atmospheric pressure
	vector<unsigned short> atm_pressure;
	//! Atmospheric humidity
	vector<unsigned short> atm_humidity;
	//! Acceleration of the antenna in X
	vector<unsigned short> acceleration_x;
	//! Acceleration of the antenna in Y
	vector<unsigned short> acceleration_y;
	//! Acceleration of the antenna in Z
	vector<unsigned short> acceleration_z;
	//! ?? Battery voltage - but is it really a voltage, or needs to be recalculated to such? Because labelled as ADC
	vector<unsigned short> battery_adc;
	//! ?? Event version Is this the same as event_version for the whole event?
	vector<unsigned short> event_version_in;
	//! ADC sampling frequency in MHz
	vector<unsigned short> adc_sampling_frequency;
	//! ADC sampling resolution in bits
	vector<unsigned short> adc_sampling_resolution;
	//! ?? ADC input channels - how to interpret this values?
	vector<unsigned short> adc_input_channels;
	//! ?? ADC enabled channels - how to interpret this values?
	vector<unsigned short> adc_enabled_channels;
	//! ADC samples callected in all channels
	vector<unsigned short> adc_samples_count_total;
	//! ADC samples callected in channel 0
	vector<unsigned short> adc_samples_count_channel0;
	//! ADC samples callected in channel 1
	vector<unsigned short> adc_samples_count_channel1;
	//! ADC samples callected in channel 2
	vector<unsigned short> adc_samples_count_channel2;
	//! ADC samples callected in channel 3
	vector<unsigned short> adc_samples_count_channel3;
	//! ?? Trigger pattern - which of the trigger sources (more than one may be present) fired to actually the trigger the digitizer - but what format is it?
	vector<unsigned short> trigger_pattern;
	//! ?? Trigger rate (stamp by last PPS) - what does it mean?
	vector<unsigned short> trigger_rate;
	//! ?? Internal clock tick of the trigger?
	vector<unsigned short> clock_tick;
	//! ?? Clock ticks per second
	vector<unsigned int> clock_tics_per_second;
	//! GPS offset - is this GALILEO to GPS offset? Do we need to include it somehow in the final time?
	vector<float> gps_offset;
	//! GPS leap second
	vector<unsigned short> gps_leap_second;
	//! GPS status
	vector<unsigned short> gps_status;
	//! GPS alarms
	vector<unsigned short> gps_alarms;
	//! GPS warnings
	vector<unsigned short> gps_warnings;
	//! GPS time
	vector<unsigned int> gps_time;
	//! Longitude
	vector<double> gps_long;
	//! Latitude
	vector<double> gps_lat;
	//! Altitude
	vector<double> gps_alt;
	//! GPS temperature
	vector<float> gps_temp;
	//! ?? Control parameters - what are they?
	vector<vector<unsigned short>> digi_ctrl;
	//! ?? Window parameters - what do they mean?
	vector<vector<unsigned short>> digi_prepost_trig_windows;
	//! ?? Channel 0 properties - what are they?
	vector<vector<unsigned short>> channel_properties0;
	//! ?? Channel 1 properties - what are they?
	vector<vector<unsigned short>> channel_properties1;
	//! ?? Channel 2 properties - what are they?
	vector<vector<unsigned short>> channel_properties2;
	//! ?? Channel 3 properties - what are they?
	vector<vector<unsigned short>> channel_properties3;
	//! ?? Channel 0 trigger settings - what are they?
	vector<vector<unsigned short>> channel_trig_settings0;
	//! ?? Channel 1 trigger settings - what are they?
	vector<vector<unsigned short>> channel_trig_settings1;
	//! ?? Channel 2 trigger settings - what are they?
	vector<vector<unsigned short>> channel_trig_settings2;
	//! ?? Channel 3 trigger settings - what are they?
	vector<vector<unsigned short>> channel_trig_settings3;
	//! ?? What is it? Some kind of the adc track offset?
	vector<unsigned short> ioff;
	//! ADC track 0
	vector<vector<short>> adc_track0;
	//! ADC track 1
	vector<vector<short>> adc_track1;
	//! ADC track 2
	vector<vector<short>> adc_track2;
	//! ADC track 3
	vector<vector<short>> adc_track3;

	//! The TTree for holding the data
	TTree *teventadc;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! Set the object variables from pointer intialised by Charles' functions from Traces.c
	int SetValuesFromPointers(unsigned short *pevent);

	//! The default constructor
	GRANDEventADC();

private:
	//! Clear the vectors for another fill
	void ClearVectors();

};


#endif //GTOT_GRANDEVENTADC_H

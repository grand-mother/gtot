//
// Created by lewhoo on 19/01/2022.
//
// The main class for holding the ADC counts and other data coming from the detectors

#ifndef GTOT_TADC_H
#define GTOT_TADC_H

#include <vector>
#include <algorithm>
#include <functional>
#include "scope.h"
#include "Traces.h"
#include "TTree.h"
#include "TTimeStamp.h"
#include "gtot.h"

using namespace std;

class TADC
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:

	// *** Metadata ***

	//! Tree type
	string type = "TADC";
	//! Comment - if needed, added by user
	string comment = "";
	//! TTree creation date/time in UTC - a naive time, without timezone set
	int creation_datetime;
	//! Modification history - JSON
	string modification_history = "";

	//! Unix creation datetime of the source tree; 0 s means no source
	int source_datetime = 0;
	//! The tool used to generate this tree's values from another tree
	string modification_software = "";
	//! The version of the tool used to generate this tree's values from another tree
	string modification_software_version = "";
	//! The analysis level of this tree
	int analysis_level = 0;

	// *** Entry values ***

	// Better have local storage for all the variables if one day we don't init from the structure
	// Common for the whole event
	//! Event size
	unsigned int event_size;
	//! Run number
	unsigned int run_number;
	//! Event in the run number
	unsigned int event_number;
	//! Trigger number from T3 maker
	unsigned int t3_number;
	//! First detector unit that triggered in the event
	unsigned int first_du;
	//! Unix time corresponding to the GPS seconds of the earliest trigger time of all the stations
	unsigned int time_seconds;
	//! GPS nanoseconds corresponding to the earliest trigger time of all the stations
	unsigned int time_nanoseconds;
	//! Trigger type 0x1000 10 s trigger and 0x8000 random trigger, else shower
	unsigned int event_type;
	//! Event format version of the DAQ
	unsigned int event_version;
	//! Number of detector units in the event - basically the antennas count
	unsigned int du_count;
	// Separate for each DU (antenna) in the event
	// As number of triggered antennas is variable, we need vectors here
	//! The T3 trigger number
	vector<unsigned short> event_id;
	//! Detector unit (antenna) ID
	vector<unsigned short> du_id;
	//! Unix time of the trigger for this DU
	vector<unsigned int> du_seconds;
	//! Nanoseconds of the trigger for this DU
	vector<unsigned int> du_nanoseconds;
	//! Trigger position in the trace (trigger start = nanoseconds - 2*sample number)
	vector<unsigned short> trigger_position;
	//! Same as event_type, but event_type could consist of different triggered DUs
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
	//! Battery level in ADC units (corresponding to some voltage)
	vector<unsigned short> battery_level;
	//! Firmware version
	vector<unsigned short> firmware_version;
	//! ADC sampling frequency in MHz
	vector<unsigned short> adc_sampling_frequency;
	//! ADC sampling resolution in bits
	vector<unsigned short> adc_sampling_resolution;

	//! ADC input channels - > 16 BIT WORD (4*4 BITS) LOWEST IS CHANNEL 1, HIGHEST CHANNEL 4. FOR EACH CHANNEL IN THE EVENT WE HAVE: 0: ADC1, 1: ADC2, 2:ADC3, 3:ADC4 4:FILTERED ADC1, 5:FILTERED ADC 2, 6:FILTERED ADC3, 7:FILTERED ADC4.
//	vector<unsigned short> adc_input_channels;
	vector<vector<unsigned char>> adc_input_channels_ch;

	//! ADC enabled channels - LOWEST 4 BITS STATE WHICH CHANNEL IS READ OUT
//	vector<unsigned short> adc_enabled_channels;
	vector<vector<bool>> adc_enabled_channels_ch;


	//! ADC samples collected in all channels
	vector<unsigned short> adc_samples_count_total;

//	//! ADC samples callected in channel 0
//	vector<unsigned short> adc_samples_count_channel0;
//	//! ADC samples callected in channel 1
//	vector<unsigned short> adc_samples_count_channel1;
//	//! ADC samples callected in channel 2
//	vector<unsigned short> adc_samples_count_channel2;
//	//! ADC samples callected in channel 3
//	vector<unsigned short> adc_samples_count_channel3;
	//! ADC samples count per channel
	vector<vector<unsigned short>> adc_samples_count_ch;

	//! Trigger pattern - which of the trigger sources (more than one may be present) fired to actually the trigger the digitizer - explained in the docs.
//	vector<unsigned short> trigger_pattern;

	vector<vector<bool>> trigger_pattern_ch;
	vector<bool> trigger_pattern_ch0_ch1;
	vector<bool> trigger_pattern_notch0_ch1;
	vector<bool> trigger_pattern_redch0_ch1;
	vector<bool> trigger_pattern_ch2_ch3;
	vector<bool> trigger_pattern_calibration;
	vector<bool> trigger_pattern_10s;
	vector<bool> trigger_pattern_external_test_pulse;

	//! Trigger rate - the number of triggers recorded in the second preceding the event
	vector<unsigned short> trigger_rate;
	//! Clock tick at which the event was triggered (used to calculate the trigger time)
	vector<unsigned int> clock_tick;
	//! Clock ticks per second
	vector<unsigned int> clock_ticks_per_second;
	//! GPS offset - offset between the PPS and the real second (in GPS). ToDo: is it already included in the time calculations?
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
//	vector<unsigned short> gps_long;
	vector<unsigned long long> gps_long;
	//! Latitude
//	vector<unsigned short> gps_lat;
	vector<unsigned long long> gps_lat;
	//! Altitude
//	vector<unsigned short> gps_alt;
	vector<unsigned long long> gps_alt;
	//! GPS temperature
	vector<unsigned int> gps_temp;

	// Digitizer mode parameters

	//! Control parameters - the list of general parameters that can set the mode of operation, select trigger sources and preset the common coincidence read out time window (Digitizer mode parameters in the manual).
//	vector<vector<unsigned short>> digi_ctrl;

	// Digitizer control register
	vector<bool> enable_auto_reset_timeout;
	vector<bool> force_firmware_reset;
	vector<vector<bool>> enable_filter_ch;
	vector<bool> enable_1PPS;
	vector<bool> enable_DAQ;

	// Trigger enable mask register
	vector<vector<bool>> enable_trigger_ch;
	vector<bool> enable_trigger_ch0_ch1;
	vector<bool> enable_trigger_notch0_ch1;
	vector<bool> enable_trigger_redch0_ch1;
	vector<bool> enable_trigger_ch2_ch3;
	vector<bool> enable_trigger_calibration;
	vector<bool> enable_trigger_10s;
	vector<bool> enable_trigger_external_test_pulse;

	// Test pulse rate divider and channel readout enable
	vector<vector<bool>> enable_readout_ch;
	vector<bool> fire_single_test_pulse;
	vector<unsigned char> test_pulse_rate_divider;

	// Common coincidence readout time window
	vector<unsigned short> common_coincidence_time;

	// Input selector for readout channel
	vector<vector<unsigned char>> selector_readout_ch;


	//! Digitizer window parameters - describe Pre Coincidence, Coincidence and Post Coincidence readout windows (Digitizer window parameters in the manual).
//	vector<vector<unsigned short>> digi_prepost_trig_windows;
	vector<vector<unsigned short>> pre_coincidence_window_ch;
	vector<vector<unsigned short>> post_coincidence_window_ch;

	// Channel property parameters

//	//! Channel 0 properties - described in Channel property parameters in the manual.
//	vector<vector<unsigned short>> channel_properties0;
//	//! Channel 1 properties - described in Channel property parameters in the manual.
//	vector<vector<unsigned short>> channel_properties1;
//	//! Channel 2 properties - described in Channel property parameters in the manual.
//	vector<vector<unsigned short>> channel_properties2;
//	//! Channel 3 properties - described in Channel property parameters in the manual.
//	vector<vector<unsigned short>> channel_properties3;

	vector<vector<unsigned short>> gain_correction_ch;
	vector<vector<unsigned char>> integration_time_ch;
	vector<vector<unsigned char>> offset_correction_ch;
	vector<vector<unsigned short>> base_maximum_ch;
	vector<vector<unsigned short>> base_minimum_ch;

	// Channel trigger parameters

//	//! Channel 0 trigger settings - described in Channel trigger parameters in the manual.
//	vector<vector<unsigned short>> channel_trig_settings0;
//	//! Channel 1 trigger settings - described in Channel trigger parameters in the manual.
//	vector<vector<unsigned short>> channel_trig_settings1;
//	//! Channel 2 trigger settings - described in Channel trigger parameters in the manual.
//	vector<vector<unsigned short>> channel_trig_settings2;
//	//! Channel 3 trigger settings - described in Channel trigger parameters in the manual.
//	vector<vector<unsigned short>> channel_trig_settings3;

	vector<vector<unsigned short>> signal_threshold_ch;
	vector<vector<unsigned short>> noise_threshold_ch;
	vector<vector<unsigned char>> tper_ch;
	vector<vector<unsigned char>> tprev_ch;
	vector<vector<unsigned char>> ncmax_ch;
	vector<vector<unsigned char>> tcmax_ch;
	vector<vector<unsigned char>> qmax_ch;
	vector<vector<unsigned char>> ncmin_ch;
	vector<vector<unsigned char>> qmin_ch;


	//! ?? What is it? Some kind of the adc trace offset?
	vector<unsigned short> ioff;
//	//! ADC trace 0
//	vector<vector<short>> trace_0;
//	//! ADC trace 1
//	vector<vector<short>> trace_1;
//	//! ADC trace 2
//	vector<vector<short>> trace_2;
//	//! ADC trace 3
//	vector<vector<short>> trace_3;
	//! ADC traces for channels (0,1,2,3)
	vector<vector<vector<short>>> trace_ch;

	//! The TTree for holding the data
	TTree *tadc;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! Set the object variables from pointer intialised by Charles' functions from Traces.c
	int SetValuesFromPointers(unsigned short *pevent, string file_format="");

	//! The default constructor
	TADC();

private:
	//! Clear the vectors for another fill
	void ClearVectors();

	//! Initialises the TTree metadata fields
	void InitialiseMetadata();

	//! Decode the trigger pattern
	void TriggerPatternDecodeAndFill(unsigned short);
	//! Decode the digital control
	void DigiCtrlDecodeAndFill(unsigned short[8]);
	//! Decode the digital window
	void DigiWindowDecodeAndFill(unsigned short[8]);
	//! Decode the channels' properties
	void ChannelPropertyDecodeAndFill(unsigned short[24]);
	//! Decode the channels' trigger parameters
	void ChannelTriggerParameterDecodeAndFill(unsigned short[24]);
	//! Decode the ADC input channels
	void ADCInputChannelsDecodeAndFill(unsigned short);
	//! Decode the ADC enabled channels
	void ADCEnabledChannelsDecodeAndFill(unsigned short);

	//! Convert the 14-bit ADC value to short
	static short ADC2short(short value);
};


#endif //GTOT_TADC_H

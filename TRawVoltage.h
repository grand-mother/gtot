//
// Created by lewhoo on 17/02/2022.
//
// The main class for holding the Voltage traces and some other data relative to them

#ifndef GTOT_TRAWVOLTAGE_H
#define GTOT_TRAWVOLTAGE_H

#include <vector>
#include "Traces.h"
#include "TTree.h"
#include "TTimeStamp.h"
#include "TLeaf.h"
#include "TADC.h"

using namespace std;

class TRawVoltage
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:
	// Better have local storage for all the variables if one day we don't init from the structure
	// Common for the whole event
	//! Run number
	unsigned int run_number;
	//! Event in the run number
	unsigned int event_number;
	//! Event size
	unsigned int event_size;
//	//! Trigger number from T3 maker
//	unsigned int t3_number;
	//! First detector unit that triggered in the event
	unsigned int first_du;
	//! Unix time corresponding to the GPS seconds of the trigger
	unsigned int time_seconds;
	//! GPS nanoseconds corresponding to the trigger of the first triggered station
	unsigned int time_nanoseconds;
//	//! Trigger type 0x1000 10 s trigger and 0x8000 random trigger, else shower
//	unsigned int event_type;
//	//! Event format version of the DAQ
//	unsigned int event_version;
	//! Number of detector units in the event - basically the antennas count
	unsigned int du_count;
	// Separate for each DU (antenna) in the event
	// As number of triggered antennas is variable, we need vectors here
//	//! The T3 trigger number
//	vector<unsigned short> event_id;
	//! Detector unit (antenna) ID
	vector<unsigned short> du_id;
	//! Unix time of the trigger for this DU
	vector<unsigned int> du_seconds;
	//! Nanoseconds of the trigger for this DU
	vector<unsigned int> du_nanoseconds;
//	//! Unix time of the start of the trace for this DU
//	vector<unsigned int> du_t0_seconds;
//	//! Nanoseconds of the start of the trace for this DU
//	vector<unsigned int> du_t0_nanoseconds;
//	//! Trigger position in the trace (trigger start = nanoseconds - 2*sample number)
//	vector<unsigned short> trigger_position;
	//! Same as event_type, but event_type could consist of different triggered DUs
	vector<unsigned short> trigger_flag;
	//! Atmospheric temperature (read via I2C)
	vector<float> atm_temperature;
	//! Atmospheric pressure
	vector<float> atm_pressure;
	//! Atmospheric humidity
	vector<float> atm_humidity;
	//! Acceleration of the antenna in (x,y,z) in m/s2
	vector<vector<float>> du_acceleration;
//	//! Acceleration of the antenna in X
//	vector<float> acceleration_x;
//	//! Acceleration of the antenna in Y
//	vector<float> acceleration_y;
//	//! Acceleration of the antenna in Z
//	vector<float> acceleration_z;
	//! Battery voltage
	vector<float> battery_level;
//	//! Firmware version
//	vector<unsigned short> firmware_version;
//	//! ADC sampling frequency in MHz
//	vector<unsigned short> adc_sampling_frequency;
//	//! ADC sampling resolution in bits
//	vector<unsigned short> adc_sampling_resolution;
//	//! ADC input channels - > 16 BIT WORD (4*4 BITS) LOWEST IS CHANNEL 1, HIGHEST CHANNEL 4. FOR EACH CHANNEL IN THE EVENT WE HAVE: 0: ADC1, 1: ADC2, 2:ADC3, 3:ADC4 4:FILTERED ADC1, 5:FILTERED ADC 2, 6:FILTERED ADC3, 7:FILTERED ADC4. ToDo: decode this?
//	vector<unsigned short> adc_input_channels;
//	//! ADC enabled channels - LOWEST 4 BITS STATE WHICH CHANNEL IS READ OUT ToDo: Decode this?
//	vector<unsigned short> adc_enabled_channels;
//	//! ADC samples callected in all channels
//	vector<unsigned short> adc_samples_count_total;
	//! ADC samples callected in channels (0,1,2,3)
	vector<vector<unsigned short>> adc_samples_count_channel;
//	//! ADC samples callected in channel 0
//	vector<unsigned short> adc_samples_count_channel0;
//	//! ADC samples callected in channel 1
//	vector<unsigned short> adc_samples_count_channel1;
//	//! ADC samples callected in channel 2
//	vector<unsigned short> adc_samples_count_channel2;
//	//! ADC samples callected in channel 3
//	vector<unsigned short> adc_samples_count_channel3;
	//! Trigger pattern - which of the trigger sources (more than one may be present) fired to actually the trigger the digitizer - explained in the docs. ToDo: Decode this?
	vector<unsigned short> trigger_pattern;
	//! Trigger rate - the number of triggers recorded in the second preceding the event
	vector<unsigned short> trigger_rate;
	//! Clock tick at which the event was triggered (used to calculate the trigger time)
	vector<unsigned short> clock_tick;
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
	vector<float> gps_long;
	//! Latitude
	vector<float> gps_lat;
	//! Altitude
	vector<float> gps_alt;
	//! GPS temperature
	vector<float> gps_temp;
//	//! X position in site's referential
//	// ToDo: need to convert all the geodetic to the site's referential!
//	vector<float> pos_x;
//	//! Y position in site's referential
//	vector<float> pos_y;
//	//! Z position in site's referential
//	vector<float> pos_z;
//	//! Control parameters - the list of general parameters that can set the mode of operation, select trigger sources and preset the common coincidence read out time window (Digitizer mode parameters in the manual). ToDo: Decode?
//	vector<vector<unsigned short>> digi_ctrl;
//	//! Window parameters - describe Pre Coincidence, Coincidence and Post Coincidence readout windows (Digitizer window parameters in the manual). ToDo: Decode?
//	vector<vector<unsigned short>> digi_prepost_trig_windows;
//	//! Channel 0 properties - described in Channel property parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_properties0;
//	//! Channel 1 properties - described in Channel property parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_properties1;
//	//! Channel 2 properties - described in Channel property parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_properties2;
//	//! Channel 3 properties - described in Channel property parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_properties3;
//	//! Channel 0 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_trig_settings0;
//	//! Channel 1 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_trig_settings1;
//	//! Channel 2 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_trig_settings2;
//	//! Channel 3 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
//	vector<vector<unsigned short>> channel_trig_settings3;
//	//! ?? What is it? Some kind of the adc trace offset?
	vector<unsigned short> ioff;
	vector<vector<float>> trace_0;
	vector<vector<float>> trace_1;
	vector<vector<float>> trace_2;
	vector<vector<float>> trace_3;

	//! The TTree for holding the data
	TTree *trawvoltage;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! General constructor
	TRawVoltage();
	//! Constructor computing values from TADC
	TRawVoltage(TADC *adc);

private:
	//! Clear the vectors for another fill
	void ClearVectors();

	//! Converts the ADC traces from tadc into Voltage traces
	void ADCs2Real(TADC *adc);

	//! Converts a specific ADC trace from tadc into a Voltage trace
	void TraceADC2Voltage(int du_num, TADC *adc);

	//! Converts a specific GPS ADC values from tadc into a real values
	void GPSADC2Real(int du_num, TADC *adc);

	//! Calculate the absolute start times - t0 - of all the traces
	void CalculateT0s(TADC *adc);

	//! Calculate the absolute start time of a trace - t0
	pair<unsigned int, unsigned int> CalculateT0(unsigned int seconds, unsigned int nanoseconds, unsigned int trigger_pos_ns);

};



#endif //GTOT_TRAWVOLTAGE_H

//
// Created by lewhoo on 17/02/2022.
//
// The main class for holding the Voltage traces and some other data relative to them

#ifndef GTOT_GRANDEVENTVOLTAGE_H
#define GTOT_GRANDEVENTVOLTAGE_H

#include <vector>
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
	//! Unix time corresponding to the GPS seconds of the trigger
	unsigned int time_seconds;
	//! GPS nanoseconds corresponding to the trigger of the first triggered station
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
	vector<float> atm_temperature;
	//! Atmospheric pressure
	vector<float> atm_pressure;
	//! Atmospheric humidity
	vector<float> atm_humidity;
	//! Acceleration of the antenna in X
	vector<float> acceleration_x;
	//! Acceleration of the antenna in Y
	vector<float> acceleration_y;
	//! Acceleration of the antenna in Z
	vector<float> acceleration_z;
	//! Battery voltage
	vector<float> battery_level;
	//! Firmware version
	vector<unsigned short> firmware_version;
	//! ADC sampling frequency in MHz
	vector<unsigned short> adc_sampling_frequency;
	//! ADC sampling resolution in bits
	vector<unsigned short> adc_sampling_resolution;
	//! ADC input channels - > 16 BIT WORD (4*4 BITS) LOWEST IS CHANNEL 1, HIGHEST CHANNEL 4. FOR EACH CHANNEL IN THE EVENT WE HAVE: 0: ADC1, 1: ADC2, 2:ADC3, 3:ADC4 4:FILTERED ADC1, 5:FILTERED ADC 2, 6:FILTERED ADC3, 7:FILTERED ADC4. ToDo: decode this?
	vector<unsigned short> adc_input_channels;
	//! ADC enabled channels - LOWEST 4 BITS STATE WHICH CHANNEL IS READ OUT ToDo: Decode this?
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
	//! Trigger pattern - which of the trigger sources (more than one may be present) fired to actually the trigger the digitizer - explained in the docs. ToDo: Decode this?
	vector<unsigned short> trigger_pattern;
	//! Trigger rate - the number of triggers recorded in the second preceding the event
	vector<unsigned short> trigger_rate;
	//! Clock tick at which the event was triggered (used to calculate the trigger time)
	vector<unsigned short> clock_tick;
	//! Clock ticks per second
	vector<unsigned int> clock_tics_per_second;
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
	//! Control parameters - the list of general parameters that can set the mode of operation, select trigger sources and preset the common coincidence read out time window (Digitizer mode parameters in the manual). ToDo: Decode?
	vector<vector<unsigned short>> digi_ctrl;
	//! Window parameters - describe Pre Coincidence, Coincidence and Post Coincidence readout windows (Digitizer window parameters in the manual). ToDo: Decode?
	vector<vector<unsigned short>> digi_prepost_trig_windows;
	//! Channel 0 properties - described in Channel property parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_properties0;
	//! Channel 1 properties - described in Channel property parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_properties1;
	//! Channel 2 properties - described in Channel property parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_properties2;
	//! Channel 3 properties - described in Channel property parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_properties3;
	//! Channel 0 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_trig_settings0;
	//! Channel 1 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_trig_settings1;
	//! Channel 2 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_trig_settings2;
	//! Channel 3 trigger settings - described in Channel trigger parameters in the manual. ToDo: Decode?
	vector<vector<unsigned short>> channel_trig_settings3;
	//! ?? What is it? Some kind of the adc trace offset?
	vector<unsigned short> ioff;
	vector<vector<float>> trace_x;
	vector<vector<float>> trace_y;
	vector<vector<float>> trace_z;

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
	void ADCs2Real(GRANDEventADC *adc);

	//! Converts a specific ADC trace from teventadc into a Voltage trace
	void TraceADC2Voltage(int du_num, GRANDEventADC *adc);

	//! Converts a specific GPS ADC values from teventadc into a real values
	void GPSADC2Real(int du_num, GRANDEventADC *adc);
};



#endif //GTOT_GRANDEVENTVOLTAGE_H

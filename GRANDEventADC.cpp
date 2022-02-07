//
// Created by lewhoo on 19/01/2022.
//

#include "GRANDEventADC.h"
#include <iostream>

GRANDEventADC::GRANDEventADC() {}

TTree *GRANDEventADC::CreateTree()
{
	teventadc = new TTree("teventadc", "Event with ADC counts and information");

	// Create Branches for all stored data
	// Int branches
	teventadc->Branch("event_size", &event_size, "event_size/i");
	teventadc->Branch("run_number", &run_number, "run_number/i");
	teventadc->Branch("event_number", &event_number, "event_number/i");
	teventadc->Branch("t3_number", &t3_number, "t3_number/i");
	teventadc->Branch("first_du", &first_du, "first_du/i");
	teventadc->Branch("time_seconds", &time_seconds, "time_seconds/i");
	teventadc->Branch("time_nanoseconds", &time_nanoseconds, "time_nanoseconds/i");
	teventadc->Branch("event_type", &event_type, "event_type/i");
	teventadc->Branch("event_version", &event_version, "event_version/i");
	teventadc->Branch("du_count", &du_count, "du_count/i");
	// Vector branches
	teventadc->Branch("event_id", &event_id);
	teventadc->Branch("du_id", &du_id);
	teventadc->Branch("du_second", &du_second);
	teventadc->Branch("du_nanosecond", &du_nanosecond);
	teventadc->Branch("trigger_position", &trigger_position);
	teventadc->Branch("trigger_flag", &trigger_flag);
	teventadc->Branch("atm_temperature", &atm_temperature);
	teventadc->Branch("atm_pressure", &atm_pressure);
	teventadc->Branch("atm_humidity", &atm_humidity);
	teventadc->Branch("acceleration_x", &acceleration_x);
	teventadc->Branch("acceleration_y", &acceleration_y);
	teventadc->Branch("acceleration_z", &acceleration_z);
	teventadc->Branch("battery_adc", &battery_adc);
	teventadc->Branch("event_version_in", &event_version_in);
	teventadc->Branch("adc_sampling_frequency", &adc_sampling_frequency);
	teventadc->Branch("adc_sampling_resolution", &adc_sampling_resolution);
	teventadc->Branch("adc_input_channels", &adc_input_channels);
	teventadc->Branch("adc_enabled_channels", &adc_enabled_channels);
	teventadc->Branch("adc_samples_count_total", &adc_samples_count_total);
	teventadc->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
	teventadc->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
	teventadc->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
	teventadc->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
	teventadc->Branch("trigger_pattern", &trigger_pattern);
	teventadc->Branch("trigger_rate", &trigger_rate);
	teventadc->Branch("clock_tick", &clock_tick);
	teventadc->Branch("clock_tics_per_second", &clock_tics_per_second);
	teventadc->Branch("gps_offset", &gps_offset);
	teventadc->Branch("gps_leap_second", &gps_leap_second);
	teventadc->Branch("gps_status", &gps_status);
	teventadc->Branch("gps_alarms", &gps_alarms);
	teventadc->Branch("gps_warnings", &gps_warnings);
	teventadc->Branch("gps_date", &gps_date);
	teventadc->Branch("gps_time", &gps_time);
	teventadc->Branch("gps_long", &gps_long);
	teventadc->Branch("gps_lat", &gps_lat);
	teventadc->Branch("gps_alt", &gps_alt);
	teventadc->Branch("gps_temp", &gps_temp);
	teventadc->Branch("digi_ctrl", &digi_ctrl);
	teventadc->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
	teventadc->Branch("channel_properties0", &channel_properties0);
	teventadc->Branch("channel_properties1", &channel_properties1);
	teventadc->Branch("channel_properties2", &channel_properties2);
	teventadc->Branch("channel_properties3", &channel_properties3);
	teventadc->Branch("channel_trig_settings0", &channel_trig_settings0);
	teventadc->Branch("channel_trig_settings1", &channel_trig_settings1);
	teventadc->Branch("channel_trig_settings2", &channel_trig_settings2);
	teventadc->Branch("channel_trig_settings3", &channel_trig_settings3);
	teventadc->Branch("ioff", &ioff);
	teventadc->Branch("adc_track0", &adc_track0);
	teventadc->Branch("adc_track1", &adc_track1);
	teventadc->Branch("adc_track2", &adc_track2);
	teventadc->Branch("adc_track3", &adc_track3);


	return teventadc;
}

int GRANDEventADC::SetValuesFromPointers(unsigned short *pevent)
{
	// Clear the vectors
	ClearVectors();

	unsigned int *evptr = (unsigned int *)pevent;

	int idu = EVENT_DU; //parameter indicating start of LS
	int ev_end = ((int)(pevent[EVENT_HDR_LENGTH+1]<<16)+(int)(pevent[EVENT_HDR_LENGTH]))/SHORTSIZE;

	event_size = *evptr++;
	run_number = *evptr++;
	event_number = *evptr++;
	t3_number = *evptr++;
	first_du = *evptr++;
	time_seconds = *evptr++;
	time_nanoseconds = *evptr++;
	event_type = *evptr++;
	event_version = *evptr++;
	du_count = *evptr++;

	uint16_t *evdu;
	while(idu<ev_end)
	{
		evdu = (uint16_t *)(&pevent[idu]);

		event_id.push_back(evdu[EVT_ID]);
		du_id.push_back(evdu[EVT_HARDWARE]);
		du_second.push_back(evdu[EVT_SECOND]);
		du_nanosecond.push_back(evdu[EVT_NANOSEC]);
		trigger_position.push_back(evdu[EVT_TRIGGERPOS]);
		trigger_flag.push_back(evdu[EVT_T3FLAG]);
		atm_temperature.push_back(evdu[EVT_ATM_TEMP]);
		atm_pressure.push_back(evdu[EVT_ATM_PRES]);
		atm_humidity.push_back(evdu[EVT_ATM_HUM]);
		acceleration_x.push_back(evdu[EVT_ACCEL_X]);
		acceleration_y.push_back(evdu[EVT_ACCEL_Y]);
		acceleration_z.push_back(evdu[EVT_ACCEL_Z]);
		battery_adc.push_back(evdu[EVT_ACCEL_Z]);
		// ToDo: Is this the same as event_version for the whole event?
		event_version_in.push_back(evdu[EVT_VERSION]);
		adc_sampling_frequency.push_back(evdu[EVT_MSPS]);
		adc_sampling_resolution.push_back(evdu[EVT_ADC_RES]);
		adc_input_channels.push_back(evdu[EVT_INP_SELECT]);
		adc_enabled_channels.push_back(evdu[EVT_CH_ENABLE]);
		adc_samples_count_total.push_back(16*evdu[EVT_TOT_SAMPLES]);
		adc_samples_count_channel0.push_back(evdu[EVT_TOT_SAMPLES]);
		adc_samples_count_channel1.push_back(evdu[EVT_TOT_SAMPLES+1]);
		adc_samples_count_channel2.push_back(evdu[EVT_TOT_SAMPLES+2]);
		adc_samples_count_channel3.push_back(evdu[EVT_TOT_SAMPLES+3]);
		trigger_pattern.push_back(evdu[EVT_TRIG_PAT]);
		trigger_rate.push_back(evdu[EVT_TRIG_RATE]);
		clock_tick.push_back(evdu[EVT_CTD]);
		clock_tics_per_second.push_back(evdu[EVT_CTP]);
		gps_offset.push_back(evdu[EVT_PPS_OFFSET]);
		gps_leap_second.push_back(evdu[EVT_LEAP]);
		gps_status.push_back(evdu[EVT_GPS_STATFLAG]);
		gps_alarms.push_back(evdu[EVT_GPS_CRITICAL]);
		gps_warnings.push_back(evdu[EVT_GPS_WARNING]);
		// ToDo: Do I want to have one unix time here already, stuff as it comes from electronics (eg. DAYMONTH) or like it is in Charles' printout?
		gps_date.push_back(0);
		// ToDo: Do I want to have one unix time here already, stuff as it comes from electronics (eg. DAYMONTH) or like it is in Charles' printout?
		gps_time.push_back(0);
		gps_long.push_back(evdu[EVT_LONGITUDE]);
		gps_lat.push_back(evdu[EVT_LATITUDE]);
		gps_alt.push_back(evdu[EVT_ALTITUDE]);
		gps_temp.push_back(evdu[EVT_GPS_TEMP]);
		// Maybe this could be prettier with lambdas...
		digi_ctrl.push_back(vector<unsigned short>());
		for(int i=0;i<8;i++) digi_ctrl.back().push_back(evdu[EVT_CTRL+i]);
		digi_prepost_trig_windows.push_back(vector<unsigned short>());
		for(int i=0;i<8;i++) digi_prepost_trig_windows.back().push_back(evdu[EVT_WINDOWS+i]);
		channel_properties0.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties0.back().push_back(evdu[EVT_CHANNEL+6*0+i]);
		channel_properties1.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties0.back().push_back(evdu[EVT_CHANNEL+6*1+i]);
		channel_properties2.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties0.back().push_back(evdu[EVT_CHANNEL+6*2+i]);
		channel_properties3.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties0.back().push_back(evdu[EVT_CHANNEL+6*3+i]);
		channel_trig_settings0.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings0.back().push_back(evdu[EVT_TRIGGER+6*0+i]);
		channel_trig_settings1.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings0.back().push_back(evdu[EVT_TRIGGER+6*1+i]);
		channel_trig_settings2.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings0.back().push_back(evdu[EVT_TRIGGER+6*2+i]);
		channel_trig_settings3.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings0.back().push_back(evdu[EVT_TRIGGER+6*3+i]);
		// ToDo: What is it?
		ioff.push_back(evdu[EVT_HDRLEN]);

		int start_addr = ioff.back();
		int end_addr = start_addr+evdu[EVT_TOT_SAMPLES+1];
		adc_track0.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		start_addr+=evdu[EVT_TOT_SAMPLES+1];
		end_addr = start_addr+evdu[EVT_TOT_SAMPLES+2];
		adc_track1.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		start_addr+=evdu[EVT_TOT_SAMPLES+2];
		end_addr = start_addr+evdu[EVT_TOT_SAMPLES+3];
		adc_track2.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		start_addr+=evdu[EVT_TOT_SAMPLES+3];
		end_addr = start_addr+evdu[EVT_TOT_SAMPLES+4];
		adc_track3.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));

		idu +=(evdu[EVT_LENGTH]);
	}

	return 0;
}

void GRANDEventADC::ClearVectors()
{
	event_id.clear();
	du_id.clear();
	du_second.clear();
	du_nanosecond.clear();
	trigger_position.clear();
	trigger_flag.clear();
	atm_temperature.clear();
	atm_pressure.clear();
	atm_humidity.clear();
	acceleration_x.clear();
	acceleration_y.clear();
	acceleration_z.clear();
	battery_adc.clear();
	// ToDo: Is this the same as event_version for the whole event?
	event_version_in.clear();
	adc_sampling_frequency.clear();
	adc_sampling_resolution.clear();
	adc_input_channels.clear();
	adc_enabled_channels.clear();
	adc_samples_count_total.clear();
	adc_samples_count_channel0.clear();
	adc_samples_count_channel1.clear();
	adc_samples_count_channel2.clear();
	adc_samples_count_channel3.clear();
	trigger_pattern.clear();
	trigger_rate.clear();
	clock_tick.clear();
	clock_tics_per_second.clear();
	gps_offset.clear();
	gps_leap_second.clear();
	gps_status.clear();
	gps_alarms.clear();
	gps_warnings.clear();
	gps_date.clear();
	gps_time.clear();
	gps_long.clear();
	gps_lat.clear();
	gps_alt.clear();
	gps_temp.clear();
	digi_ctrl.clear();
	digi_prepost_trig_windows.clear();
	channel_properties0.clear();
	channel_properties1.clear();
	channel_properties2.clear();
	channel_properties3.clear();
	channel_trig_settings0.clear();
	channel_trig_settings1.clear();
	channel_trig_settings2.clear();
	channel_trig_settings3.clear();
	// ToDo: What is it?
	ioff.clear();
	adc_track0.clear();
	adc_track1.clear();
	adc_track2.clear();
	adc_track3.clear();
}

//
// Created by lewhoo on 19/01/2022.
//
// The main class for holding the ADC counts and other data coming from the detectors

#include "TADC.h"
#include <iostream>

TADC::TADC()
{
	CreateTree();
}

TTree *TADC::CreateTree()
{
	tadc = new TTree("tadc", "Event with ADC counts and information");

	// Create Branches for all stored data
	// Int branches
	tadc->Branch("run_number", &run_number, "run_number/i");
	tadc->Branch("event_number", &event_number, "event_number/i");
	tadc->Branch("event_size", &event_size, "event_size/i");
	tadc->Branch("t3_number", &t3_number, "t3_number/i");
	tadc->Branch("first_du", &first_du, "first_du/i");
	tadc->Branch("time_seconds", &time_seconds, "time_seconds/i");
	tadc->Branch("time_nanoseconds", &time_nanoseconds, "time_nanoseconds/i");
	tadc->Branch("event_type", &event_type, "event_type/i");
	tadc->Branch("event_version", &event_version, "event_version/i");
	tadc->Branch("du_count", &du_count, "du_count/i");
	// Vector branches
	tadc->Branch("event_id", &event_id);
	tadc->Branch("du_id", &du_id);
	tadc->Branch("du_seconds", &du_seconds);
	tadc->Branch("du_nanoseconds", &du_nanoseconds);
	tadc->Branch("trigger_position", &trigger_position);
	tadc->Branch("trigger_flag", &trigger_flag);
	tadc->Branch("atm_temperature", &atm_temperature);
	tadc->Branch("atm_pressure", &atm_pressure);
	tadc->Branch("atm_humidity", &atm_humidity);
	tadc->Branch("acceleration_x", &acceleration_x);
	tadc->Branch("acceleration_y", &acceleration_y);
	tadc->Branch("acceleration_z", &acceleration_z);
	tadc->Branch("battery_level", &battery_level);
	tadc->Branch("firmware_version", &firmware_version);
	tadc->Branch("adc_sampling_frequency", &adc_sampling_frequency);
	tadc->Branch("adc_sampling_resolution", &adc_sampling_resolution);
	tadc->Branch("adc_input_channels", &adc_input_channels);
	tadc->Branch("adc_enabled_channels", &adc_enabled_channels);
	tadc->Branch("adc_samples_count_total", &adc_samples_count_total);
	tadc->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
	tadc->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
	tadc->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
	tadc->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
	tadc->Branch("trigger_pattern", &trigger_pattern);
	tadc->Branch("trigger_rate", &trigger_rate);
	tadc->Branch("clock_tick", &clock_tick);
	tadc->Branch("clock_ticks_per_second", &clock_ticks_per_second);
	tadc->Branch("gps_offset", &gps_offset);
	tadc->Branch("gps_leap_second", &gps_leap_second);
	tadc->Branch("gps_status", &gps_status);
	tadc->Branch("gps_alarms", &gps_alarms);
	tadc->Branch("gps_warnings", &gps_warnings);
	tadc->Branch("gps_time", &gps_time);
	tadc->Branch("gps_long", &gps_long);
	tadc->Branch("gps_lat", &gps_lat);
	tadc->Branch("gps_alt", &gps_alt);
	tadc->Branch("gps_temp", &gps_temp);
	tadc->Branch("digi_ctrl", &digi_ctrl);
	tadc->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
	tadc->Branch("channel_properties0", &channel_properties0);
	tadc->Branch("channel_properties1", &channel_properties1);
	tadc->Branch("channel_properties2", &channel_properties2);
	tadc->Branch("channel_properties3", &channel_properties3);
	tadc->Branch("channel_trig_settings0", &channel_trig_settings0);
	tadc->Branch("channel_trig_settings1", &channel_trig_settings1);
	tadc->Branch("channel_trig_settings2", &channel_trig_settings2);
	tadc->Branch("channel_trig_settings3", &channel_trig_settings3);
	tadc->Branch("ioff", &ioff);
//	tadc->Branch("trace_0", &trace_0);
//	tadc->Branch("trace_1", &trace_1);
//	tadc->Branch("trace_2", &trace_2);
//	tadc->Branch("trace_3", &trace_3);
	tadc->Branch("trace_ch", &trace_ch);

	return tadc;
}

int TADC::SetValuesFromPointers(unsigned short *pevent, string file_format)
{
	int file_shift = 0;

	// A bool means a lot of bools for more formats, but hopefully it won't come to that
	bool gp13v1 = false;
	if(strstr(file_format.c_str(),"gp13v1"))
	{
		gp13v1 = true;
		file_shift = 8;
	}

	// Clear the vectors
	ClearVectors();

	unsigned int *evptr = (unsigned int *)pevent;

	int idu = EVENT_DU; //parameter indicating start of LS
	int ev_end = ((int)(pevent[EVENT_HDR_LENGTH+1]<<16)+(int)(pevent[EVENT_HDR_LENGTH]))/SHORTSIZE;
	if(gp13v1)
	{
		idu = 0;
	}

	uint16_t *evdu;
	char Event_ID[4];
	char TriggerDuNumber[4];
	char HitId[4];

	event_size = *evptr++;
	if(gp13v1)
	{
		run_number=event_size;
		printf("event_size is %lld\n", event_size);
		printf("msg_type is %lld\n", *evptr++);
		du_id.push_back(*evptr++);
		printf("duID is %d\n", du_id[0]);
		//memcpy(HitId, (char*)evptr++, 4);
		//printf("hit id is %lld\n",*evptr++);
		event_number = *evptr++;
		cout << "Event number " << event_number << endl;
	}
	else
	{
		run_number = *evptr++;
		event_number = *evptr++;
		t3_number = *evptr++;
		first_du = *evptr++;
		time_seconds = *evptr++;
		time_nanoseconds = *evptr++;
		evdu = (uint16_t *) evptr;
		event_type = evdu[0];
		event_version = *evdu++;
		evptr += 2;
		// This value is rubbish now. Need to check the traces vector sizes
		du_count = *evptr;
	}

	int NewDataAdded = 8;//Byte
	if (gp13v1) 
	{
		ev_end = event_size/sizeof(uint16_t);
		cout<<"Test "<<event_size/sizeof(uint16_t)<<endl;
	}

	while(idu<ev_end)
	{
		evdu = (uint16_t *)(&pevent[idu]);
		if(gp13v1) cout<<"EVT_LENGTH "<<evdu[file_shift + EVT_LENGTH]<<endl;

		event_id.push_back(evdu[file_shift + EVT_ID]);
		if(!gp13v1) du_id.push_back(evdu[file_shift + EVT_HARDWARE]);
		du_seconds.push_back(*(uint32_t *)&evdu[file_shift + EVT_SECOND]);
		du_nanoseconds.push_back(*(uint32_t *)&evdu[file_shift + EVT_NANOSEC]);
		trigger_position.push_back(evdu[file_shift + EVT_TRIGGERPOS]);
		trigger_flag.push_back(evdu[file_shift + EVT_T3FLAG]);
		atm_temperature.push_back(evdu[file_shift + EVT_ATM_TEMP]);
		atm_pressure.push_back(evdu[file_shift + EVT_ATM_PRES]);
		atm_humidity.push_back(evdu[file_shift + EVT_ATM_HUM]);
		acceleration_x.push_back(evdu[file_shift + EVT_ACCEL_X]);
		acceleration_y.push_back(evdu[file_shift + EVT_ACCEL_Y]);
		acceleration_z.push_back(evdu[file_shift + EVT_ACCEL_Z]);
		battery_level.push_back(evdu[file_shift + EVT_ACCEL_Z]);
		// ToDo: Is this the same as event_version for the whole event?
		firmware_version.push_back(evdu[file_shift + EVT_VERSION]);
		adc_sampling_frequency.push_back(evdu[file_shift + EVT_MSPS]);
		adc_sampling_resolution.push_back(evdu[file_shift + EVT_ADC_RES]);
		adc_input_channels.push_back(evdu[file_shift + EVT_INP_SELECT]);
		adc_enabled_channels.push_back(evdu[file_shift + EVT_CH_ENABLE]);
		adc_samples_count_total.push_back(16*evdu[file_shift + EVT_TOT_SAMPLES]);
		adc_samples_count_channel0.push_back(evdu[file_shift + EVT_TOT_SAMPLES+1]);
		adc_samples_count_channel1.push_back(evdu[file_shift + EVT_TOT_SAMPLES+2]);
		adc_samples_count_channel2.push_back(evdu[file_shift + EVT_TOT_SAMPLES+3]);
		adc_samples_count_channel3.push_back(evdu[file_shift + EVT_TOT_SAMPLES+4]);
		trigger_pattern.push_back(evdu[file_shift + EVT_TRIG_PAT]);
		trigger_rate.push_back(evdu[file_shift + EVT_TRIG_RATE]);
		clock_tick.push_back(*(uint32_t *)&evdu[file_shift + EVT_CTD]);
		clock_ticks_per_second.push_back(*(uint32_t *)&evdu[file_shift + EVT_CTP]);
		gps_offset.push_back(*(float *)&evdu[file_shift + EVT_PPS_OFFSET]);
		gps_leap_second.push_back(evdu[file_shift + EVT_LEAP]);
		gps_status.push_back(evdu[file_shift + EVT_GPS_STATFLAG]);
		gps_alarms.push_back(evdu[file_shift + EVT_GPS_CRITICAL]);
		gps_warnings.push_back(evdu[file_shift + EVT_GPS_WARNING]);

		// Convert the GPS times into unix time. This assumes we get UTC from the GPS
		TTimeStamp ts;
		ts.Set(evdu[file_shift + EVT_YEAR], (evdu[file_shift + EVT_DAYMONTH]>>8)&0xff, evdu[file_shift + EVT_DAYMONTH]&0xff, evdu[file_shift + EVT_MINHOUR]&0xff,(evdu[file_shift + EVT_MINHOUR]>>8)&0xff,evdu[file_shift + EVT_STATSEC]&0xff, 0, true, 0);
		gps_time.push_back(ts.GetSec());

//		gps_long.push_back(57.3*(*(double *)&evdu[EVT_LONGITUDE]));
//		gps_lat.push_back(57.3*(*(double *)&evdu[EVT_LATITUDE]));
//		gps_alt.push_back(*(double *)&evdu[EVT_ALTITUDE]);
//		gps_temp.push_back(*(float *)&evdu[EVT_GPS_TEMP]);
		gps_long.push_back(*(unsigned long long*)&evdu[file_shift + EVT_LONGITUDE]);
		gps_lat.push_back(*(unsigned long long*)&evdu[file_shift + EVT_LATITUDE]);
		gps_alt.push_back(*(unsigned long long*)&evdu[file_shift + EVT_ALTITUDE]);
		gps_temp.push_back(*(unsigned long long*)&evdu[file_shift + EVT_GPS_TEMP]);
		// Maybe this could be prettier with lambdas...
		digi_ctrl.push_back(vector<unsigned short>());
		for(int i=0;i<8;i++) digi_ctrl.back().push_back(evdu[file_shift + EVT_CTRL+i]);
		digi_prepost_trig_windows.push_back(vector<unsigned short>());
		for(int i=0;i<8;i++) digi_prepost_trig_windows.back().push_back(evdu[file_shift + EVT_WINDOWS+i]);
		channel_properties0.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties0.back().push_back(evdu[file_shift + EVT_CHANNEL+6*0+i]);
		channel_properties1.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties1.back().push_back(evdu[file_shift + EVT_CHANNEL+6*1+i]);
		channel_properties2.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties2.back().push_back(evdu[file_shift + EVT_CHANNEL+6*2+i]);
		channel_properties3.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_properties3.back().push_back(evdu[file_shift + EVT_CHANNEL+6*3+i]);
		channel_trig_settings0.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings0.back().push_back(evdu[file_shift + EVT_TRIGGER+6*0+i]);
		channel_trig_settings1.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings1.back().push_back(evdu[file_shift + EVT_TRIGGER+6*1+i]);
		channel_trig_settings2.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings2.back().push_back(evdu[file_shift + EVT_TRIGGER+6*2+i]);
		channel_trig_settings3.push_back(vector<unsigned short>());
		for(int i=0;i<6;i++) channel_trig_settings3.back().push_back(evdu[file_shift + EVT_TRIGGER+6*3+i]);
		// ToDo: What is it?
		ioff.push_back(evdu[file_shift + EVT_HDRLEN]);

		int start_addr = ioff.back();
		int end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+1];
		trace_0.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		start_addr+=evdu[file_shift + EVT_TOT_SAMPLES+1];
		end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+2];
		trace_1.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		start_addr+=evdu[file_shift + EVT_TOT_SAMPLES+2];
		end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+3];
		trace_2.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		start_addr+=evdu[file_shift + EVT_TOT_SAMPLES+3];
		end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+4];
		trace_3.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));

		if(gp13v1) idu +=(evdu[file_shift + EVT_LENGTH] + NewDataAdded);
		else idu +=(evdu[file_shift + EVT_LENGTH]);
	}
	// Merge the traces
	trace_ch.push_back(trace_0);
	trace_ch.push_back(trace_1);
	trace_ch.push_back(trace_2);
	trace_ch.push_back(trace_3);

	return 0;
}

void TADC::ClearVectors()
{
	event_id.clear();
	du_id.clear();
	du_seconds.clear();
	du_nanoseconds.clear();
	trigger_position.clear();
	trigger_flag.clear();
	atm_temperature.clear();
	atm_pressure.clear();
	atm_humidity.clear();
	acceleration_x.clear();
	acceleration_y.clear();
	acceleration_z.clear();
	battery_level.clear();
	// ToDo: Is this the same as event_version for the whole event?
	firmware_version.clear();
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
	clock_ticks_per_second.clear();
	gps_offset.clear();
	gps_leap_second.clear();
	gps_status.clear();
	gps_alarms.clear();
	gps_warnings.clear();
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
	trace_0.clear();
	trace_1.clear();
	trace_2.clear();
	trace_3.clear();
	trace_ch.clear();
}

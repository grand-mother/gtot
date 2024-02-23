//
// Created by lewhoo on 19/01/2022.
//
// The main class for holding the ADC counts and other data coming from the detectors

#include <iostream>
#include <bitset>
#include "inc/TADC.h"
#include "TDatime.h"
#include "TNamed.h"
#include "TParameter.h"

TADC::TADC()
{
	CreateTree();
}

TTree *TADC::CreateTree()
{
	tadc = new TTree("tadc", "Event with ADC counts and information");

	// Initialise metadata
	InitialiseMetadata();

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

	tadc->Branch("pps_id", &pps_id);
	tadc->Branch("fpga_temp", &fpga_temp);
	tadc->Branch("adc_temp", &adc_temp);

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
//	tadc->Branch("adc_input_channels", &adc_input_channels);
	tadc->Branch("adc_input_channels_ch", &adc_input_channels_ch);
//	tadc->Branch("adc_enabled_channels", &adc_enabled_channels);
	tadc->Branch("adc_enabled_channels_ch", &adc_enabled_channels_ch);
	tadc->Branch("adc_samples_count_total", &adc_samples_count_total);
//	tadc->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
//	tadc->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
//	tadc->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
//	tadc->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
	tadc->Branch("adc_samples_count_ch", &adc_samples_count_ch);

	//	tadc->Branch("trigger_pattern", &trigger_pattern);
	tadc->Branch("trigger_pattern_ch", &trigger_pattern_ch);
	tadc->Branch("trigger_pattern_ch0_ch1", &trigger_pattern_ch0_ch1);
	tadc->Branch("trigger_pattern_notch0_ch1", &trigger_pattern_notch0_ch1);
	tadc->Branch("trigger_pattern_redch0_ch1", &trigger_pattern_redch0_ch1);
	tadc->Branch("trigger_pattern_ch2_ch3", &trigger_pattern_ch2_ch3);
	tadc->Branch("trigger_pattern_calibration", &trigger_pattern_calibration);
	tadc->Branch("trigger_pattern_10s", &trigger_pattern_10s);
	tadc->Branch("trigger_pattern_external_test_pulse", &trigger_pattern_external_test_pulse);

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

//	tadc->Branch("digi_ctrl", &digi_ctrl);
	tadc->Branch("enable_auto_reset_timeout", &enable_auto_reset_timeout);
	tadc->Branch("force_firmware_reset", &force_firmware_reset);
	tadc->Branch("enable_filter_ch", &enable_filter_ch);
	tadc->Branch("enable_1PPS", &enable_1PPS);
	tadc->Branch("enable_DAQ", &enable_DAQ);
	tadc->Branch("enable_trigger_ch", &enable_trigger_ch);
	tadc->Branch("enable_trigger_ch0_ch1", &enable_trigger_ch0_ch1);
	tadc->Branch("enable_trigger_notch0_ch1", &enable_trigger_notch0_ch1);
	tadc->Branch("enable_trigger_redch0_ch1", &enable_trigger_redch0_ch1);
	tadc->Branch("enable_trigger_ch2_ch3", &enable_trigger_ch2_ch3);
	tadc->Branch("enable_trigger_calibration", &enable_trigger_calibration);
	tadc->Branch("enable_trigger_10s", &enable_trigger_10s);
	tadc->Branch("enable_trigger_external_test_pulse", &enable_trigger_external_test_pulse);
	tadc->Branch("enable_readout_ch", &enable_readout_ch);
	tadc->Branch("fire_single_test_pulse", &fire_single_test_pulse);
	tadc->Branch("test_pulse_rate_divider", &test_pulse_rate_divider);
	tadc->Branch("common_coincidence_time", &common_coincidence_time);
	tadc->Branch("selector_readout_ch", &selector_readout_ch);

	//tevent->Branch("clkb_event_gps", (gps_struct_spb2*)&Gps, "gps_time/F:gps_date:gps_lat:gps_lon:gps_hdp:gps_alt:gps_geoid_height:gps_update_time:gps_speed:gps_mag_variation:gps_course:gps_yaw:gps_tilt:gps_range_between_antennas:gps_pdop:gps_track_made_good:gps_track_made_good_magn:gps_station_id/S:gps_lat_hemisphere/B:gps_lon_hemisphere:gps_fix_quality:gps_alt_unit:gps_geoid_height_unit:gps_gngga_checksum:gps_gnzda_checksum:gps_pntl_checksum:gps_gnvtg_checksum:gps_mode:gps_mode_dim:gps_rec_warning:gps_mag_variation_hemisphere:gps_sat_mode:gps_sat_num/b:gps_ptnl_sat_num:gps_ptnl_fix_quality");

//	tadc->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
	tadc->Branch("pre_coincidence_window_ch", &pre_coincidence_window_ch);
	tadc->Branch("post_coincidence_window_ch", &post_coincidence_window_ch);

//	tadc->Branch("channel_properties0", &channel_properties0);
//	tadc->Branch("channel_properties1", &channel_properties1);
//	tadc->Branch("channel_properties2", &channel_properties2);
//	tadc->Branch("channel_properties3", &channel_properties3);
	tadc->Branch("gain_correction_ch", &gain_correction_ch);
	tadc->Branch("integration_time_ch", &integration_time_ch);
	tadc->Branch("offset_correction_ch", &offset_correction_ch);
	tadc->Branch("base_maximum_ch", &base_maximum_ch);
	tadc->Branch("base_minimum_ch", &base_minimum_ch);

//	tadc->Branch("channel_trig_settings0", &channel_trig_settings0);
//	tadc->Branch("channel_trig_settings1", &channel_trig_settings1);
//	tadc->Branch("channel_trig_settings2", &channel_trig_settings2);
//	tadc->Branch("channel_trig_settings3", &channel_trig_settings3);
	tadc->Branch("signal_threshold_ch", &signal_threshold_ch);
	tadc->Branch("noise_threshold_ch", &noise_threshold_ch);
	tadc->Branch("tprev_ch", &tprev_ch);
	tadc->Branch("tper_ch", &tper_ch);
	tadc->Branch("tcmax_ch", &tcmax_ch);
	tadc->Branch("ncmax_ch", &ncmax_ch);
	tadc->Branch("ncmin_ch", &ncmin_ch);
	tadc->Branch("qmax_ch", &qmax_ch);
	tadc->Branch("qmin_ch", &qmin_ch);

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
	// Use the binary blob addressing from firmware version 1
	using namespace fv1;

	std::ostream &vout = *pvout;

	// The allowed amount of DUs in the event. If it is exceeded, probably the reading of the file went wrong.
	int safe_du_amount = 10000;

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
		vout << "\nevent_size is " << event_size << endl;
		vout << "msg_type is " << *evptr++ << endl;
		du_id.push_back(*evptr++);
		vout << "duID is " << du_id[0] << endl;
		//memcpy(HitId, (char*)evptr++, 4);
		//printf("hit id is %lld\n",*evptr++);
		event_number = *evptr++;
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
		vout<<"Test "<<event_size/sizeof(uint16_t)<<endl;
	}

	int du_counter=0;

	while(idu<ev_end)
	{
		evdu = (uint16_t *)(&pevent[idu]);
		if(gp13v1) vout<<"EVT_LENGTH "<<evdu[file_shift + EVT_LENGTH]<<endl;

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
		battery_level.push_back(evdu[file_shift + EVT_BATTERY]);
		// ToDo: Is this the same as event_version for the whole event?
		firmware_version.push_back(evdu[file_shift + EVT_VERSION]);
		adc_sampling_frequency.push_back(evdu[file_shift + EVT_MSPS]);
		adc_sampling_resolution.push_back(evdu[file_shift + EVT_ADC_RES]);

//		adc_input_channels.push_back(evdu[file_shift + EVT_INP_SELECT]);
		ADCInputChannelsDecodeAndFill(evdu[file_shift + EVT_INP_SELECT]);

//		adc_enabled_channels.push_back(evdu[file_shift + EVT_CH_ENABLE]);
		ADCEnabledChannelsDecodeAndFill(evdu[file_shift + EVT_CH_ENABLE]);

		adc_samples_count_total.push_back(16*evdu[file_shift + EVT_TOT_SAMPLES]);
//		adc_samples_count_channel0.push_back(evdu[file_shift + EVT_TOT_SAMPLES+1]);
//		adc_samples_count_channel1.push_back(evdu[file_shift + EVT_TOT_SAMPLES+2]);
//		adc_samples_count_channel2.push_back(evdu[file_shift + EVT_TOT_SAMPLES+3]);
//		adc_samples_count_channel3.push_back(evdu[file_shift + EVT_TOT_SAMPLES+4]);
//		digi_ctrl.emplace_back();
//		for(int i=0;i<8;i++) digi_ctrl.back().push_back(evdu[file_shift + EVT_CTRL+i]);

		adc_samples_count_ch.emplace_back();
		for(int i=0;i<3;i++) adc_samples_count_ch.back().push_back(evdu[file_shift + EVT_TOT_SAMPLES+i]);

//		trigger_pattern.push_back(evdu[file_shift + EVT_TRIG_PAT]);
		TriggerPatternDecodeAndFill(evdu[file_shift + EVT_TRIG_PAT]);

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
		ts.Set(evdu[file_shift + EVT_YEAR], evdu[file_shift + EVT_DAYMONTH]&0xff, (evdu[file_shift + EVT_DAYMONTH]>>8)&0xff, evdu[file_shift + EVT_MINHOUR]&0xff,(evdu[file_shift + EVT_MINHOUR]>>8)&0xff,evdu[file_shift + EVT_STATSEC]&0xff, 0, true, 0);
		gps_time.push_back(ts.GetSec());

//		gps_long.push_back(57.3*(*(double *)&evdu[EVT_LONGITUDE]));
//		gps_lat.push_back(57.3*(*(double *)&evdu[EVT_LATITUDE]));
//		gps_alt.push_back(*(double *)&evdu[EVT_ALTITUDE]);
//		gps_temp.push_back(*(float *)&evdu[EVT_GPS_TEMP]);
		gps_long.push_back(*(unsigned long long*)&evdu[file_shift + EVT_LONGITUDE]);
		gps_lat.push_back(*(unsigned long long*)&evdu[file_shift + EVT_LATITUDE]);
		gps_alt.push_back(*(unsigned long long*)&evdu[file_shift + EVT_ALTITUDE]);
		// ToDo: I think this casting is a bug, and it should be unsigned int
		gps_temp.push_back(*(unsigned long long*)&evdu[file_shift + EVT_GPS_TEMP]);

//		digi_ctrl.push_back(vector<unsigned short>());
//		digi_ctrl.emplace_back();
//		for(int i=0;i<8;i++) digi_ctrl.back().push_back(evdu[file_shift + EVT_CTRL+i]);
		DigiCtrlDecodeAndFill(&evdu[file_shift + EVT_CTRL]);

//		digi_prepost_trig_windows.push_back(vector<unsigned short>());
//		digi_prepost_trig_windows.emplace_back();
//		for(int i=0;i<8;i++) digi_prepost_trig_windows.back().push_back(evdu[file_shift + EVT_WINDOWS+i]);
		DigiWindowDecodeAndFill(&evdu[file_shift + EVT_WINDOWS]);

//		channel_properties0.emplace_back();
//		for(int i=0;i<6;i++) channel_properties0.back().push_back(evdu[file_shift + EVT_CHANNEL+6*0+i]);
//		channel_properties1.emplace_back();
//		for(int i=0;i<6;i++) channel_properties1.back().push_back(evdu[file_shift + EVT_CHANNEL+6*1+i]);
//		channel_properties2.emplace_back();
//		for(int i=0;i<6;i++) channel_properties2.back().push_back(evdu[file_shift + EVT_CHANNEL+6*2+i]);
//		channel_properties3.emplace_back();
//		for(int i=0;i<6;i++) channel_properties3.back().push_back(evdu[file_shift + EVT_CHANNEL+6*3+i]);
		ChannelPropertyDecodeAndFill((unsigned short*)&evdu[file_shift + EVT_CHANNEL]);

//		channel_trig_settings0.emplace_back();
//		for(int i=0;i<6;i++) channel_trig_settings0.back().push_back(evdu[file_shift + EVT_TRIGGER+6*0+i]);
//		channel_trig_settings1.emplace_back();
//		for(int i=0;i<6;i++) channel_trig_settings1.back().push_back(evdu[file_shift + EVT_TRIGGER+6*1+i]);
//		channel_trig_settings2.emplace_back();
//		for(int i=0;i<6;i++) channel_trig_settings2.back().push_back(evdu[file_shift + EVT_TRIGGER+6*2+i]);
//		channel_trig_settings3.emplace_back();
//		for(int i=0;i<6;i++) channel_trig_settings3.back().push_back(evdu[file_shift + EVT_TRIGGER+6*3+i]);
		ChannelTriggerParameterDecodeAndFill((unsigned short*)&evdu[file_shift + EVT_TRIGGER]);

		// ToDo: What is it?
		ioff.push_back(evdu[file_shift + EVT_HDRLEN]);

		int start_addr = ioff.back();
		if(gp13v1) start_addr+=NewDataAdded;

		// Merge the traces
//		trace_ch.push_back(vector<vector<short>>());
		trace_ch.emplace_back();
		int end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+1];
//		trace_0.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
//		trace_ch.back().emplace_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		trace_ch.back().emplace_back(&evdu[start_addr], &evdu[end_addr]);
		transform(trace_ch.back().back().begin(), trace_ch.back().back().end(), trace_ch.back().back().begin(), &TADC::ADC2short);
//		trace_ch.back().push_back(trace_0.back());
		start_addr+=evdu[file_shift + EVT_TOT_SAMPLES+1];
		end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+2];
//		trace_1.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
//		trace_ch.back().push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		trace_ch.back().emplace_back(&evdu[start_addr], &evdu[end_addr]);
		transform(trace_ch.back().back().begin(), trace_ch.back().back().end(), trace_ch.back().back().begin(), &TADC::ADC2short);
		start_addr+=evdu[file_shift + EVT_TOT_SAMPLES+2];
		end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+3];
//		trace_2.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
//		trace_ch.back().push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		trace_ch.back().emplace_back(&evdu[start_addr], &evdu[end_addr]);
		transform(trace_ch.back().back().begin(), trace_ch.back().back().end(), trace_ch.back().back().begin(), &TADC::ADC2short);
		start_addr+=evdu[file_shift + EVT_TOT_SAMPLES+3];
		end_addr = start_addr+evdu[file_shift + EVT_TOT_SAMPLES+4];
//		trace_3.push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
//		trace_ch.back().push_back(vector<short>(&evdu[start_addr], &evdu[end_addr]));
		trace_ch.back().emplace_back(&evdu[start_addr], &evdu[end_addr]);
		transform(trace_ch.back().back().begin(), trace_ch.back().back().end(), trace_ch.back().back().begin(), &TADC::ADC2short);

		if(gp13v1) idu +=(evdu[file_shift + EVT_LENGTH] + NewDataAdded);
		else idu +=(evdu[file_shift + EVT_LENGTH]);

		du_counter++;
		// Safety check of the amount of DUs in the event. If too big, the file reading most likely went wrong.
		if(du_counter>=safe_du_amount)
		{
			cerr << "The amount of DUs in the event exceeded " << safe_du_amount << ". Too much!" << endl;
			cerr << "Make sure that the file format is correct and the file is not corrupted." << endl;
			cerr << "Exiting, to avoid a memory hog." << endl;
			return -1;
		}
	}

	return 0;
}

int TADC::SetValuesFromPointers_fv2(unsigned short *pevent, string file_format)
{
	using namespace fv2;

	std::ostream &vout = *pvout;

	// The allowed amount of DUs in the event. If it is exceeded, probably the reading of the file went wrong.
	int safe_du_amount = 10000;

	int file_shift = 0;

	// A bool means a lot of bools for more formats, but hopefully it won't come to that
	bool gp13v1 = false;

	// Clear the vectors
	ClearVectors();

	unsigned int *evptr = (unsigned int *)pevent;
	unsigned int *event = (unsigned int *)pevent;

	int idu = EVENT_DU; //parameter indicating start of LS
	int ev_end = event[EVENT_HDR_LENGTH]/4;

	unsigned int *evdu;
	char Event_ID[4];
	char TriggerDuNumber[4];
	char HitId[4];

	event_size = event[EVENT_HDR_LENGTH];
	run_number = event[EVENT_HDR_RUNNR];
	event_number = event[EVENT_HDR_EVENTNR];
	t3_number = event[EVENT_HDR_T3EVENTNR];
	first_du = event[EVENT_HDR_FIRST_DU];
	time_seconds = event[EVENT_HDR_EVENT_SEC];
	time_nanoseconds = event[EVENT_HDR_EVENT_NSEC];
//	evdu = (uint16_t *) evptr;
//	event_type = evdu[0];
//	event_version = *evdu++;
//	evptr += 2;
	du_count = event[EVENT_HDR_NDU];

	int NewDataAdded = 8;//Byte
	int du_counter=0;

	while(idu<ev_end)
	{
		evdu = (unsigned int *)(&event[idu]);

		event_id.push_back(evdu[EVT_EVT_ID]);
		du_id.push_back(evdu[EVT_STATION_ID]);
		// !ToDo: Add hardware_id
		hardware_id.push_back(evdu[EVT_HARDWARE_ID]);
		du_seconds.push_back(evdu[EVT_SECOND]);
		du_nanoseconds.push_back(evdu[EVT_NANOSEC]);
		trigger_position.push_back(evdu[EVT_TRIGGER_POS]>>16);
		trigger_flag.push_back(evdu[EVT_TRIGGER_STAT]>>16);
		// !ToDo: Add trigger status
		// ToDo: Ask Charles, as he says it is similar to trigger pattern in the old format, but this was being decoded
		trigger_status.push_back(evdu[EVT_TRIGGER_STAT]&0xffff);

		pps_id.push_back(evdu[EVT_PPS_ID]);
		fpga_temp.push_back(evdu[EVT_FPGA_TEMP]>>16);
		adc_temp.push_back(evdu[EVT_FPGA_TEMP]&0xffff);

		atm_temperature.push_back(evdu[EVT_ATM_TP]>>16);
		atm_pressure.push_back(evdu[EVT_ATM_TP]&0xffff);
		atm_humidity.push_back(evdu[EVT_HM_AX]>>16);
		acceleration_x.push_back(evdu[EVT_HM_AX]&0xffff);
		acceleration_y.push_back(evdu[EVT_AY_AZ]>>16);
		acceleration_z.push_back(evdu[EVT_AY_AZ]&0xffff);
		battery_level.push_back(evdu[EVT_BATTERY]>>16);
		// !ToDo: Is this the same as event_version for the whole event?
		firmware_version.push_back((evdu[EVT_VERSION]>>16)&0xff);
		// !ToDo: Add Data format version
		data_format_version.push_back((evdu[EVT_VERSION]>>24)&0xff);
		adaq_version.push_back((evdu[EVT_VERSION]>>8)&0xff);
		dudaq_version.push_back((evdu[EVT_VERSION]&0xff));

		adc_sampling_frequency.push_back(evdu[EVT_ADCINFO]>>16);
		adc_sampling_resolution.push_back(evdu[EVT_ADCINFO]&0xffff);

		// !ToDo: Add decoding from print_channel_info()
		ADCInputChannelsDecodeAndFill_fv2(evdu[EVT_INP_SELECT]);

		// ToDo: Add decoding from print_channel_info()
//		ADCEnabledChannelsDecodeAndFill(evdu[file_shift + EVT_CH_ENABLE]);

		// ToDo: Add decoding from print_channel_info()
//		adc_samples_count_total.push_back(16*evdu[file_shift + EVT_TOT_SAMPLES]);

		// ToDo: Add decoding from print_channel_info()
//		adc_samples_count_ch.emplace_back();
//		for(int i=0;i<3;i++) adc_samples_count_ch.back().push_back(evdu[file_shift + EVT_TOT_SAMPLES+i]);

		// !ToDo: Add decoding from print_channel_info()
		TriggerPatternDecodeAndFill_fv2(evdu[EVT_TRIG_SELECT]);

		trigger_rate.push_back(evdu[EVT_STATISTICS]>>16);
		// !ToDo: Add DDR Storage
		trigger_ddr_storage.push_back(evdu[EVT_STATISTICS]&0xffff);

		clock_tick.push_back(evdu[EVT_CTD]);
		clock_ticks_per_second.push_back(evdu[EVT_CTP]);
		gps_offset.push_back(*(float *)&evdu[EVT_OFFSET]);

		// ToDo: Check if there is leap second stored
//		gps_leap_second.push_back(evdu[file_shift + EVT_LEAP]);
		gps_status.push_back((evdu[EVT_SECMINHOUR]>>24)&0xff);

		// ToDo: Check which one in new data correspond to alarms and warning below
//		gps_alarms.push_back(evdu[file_shift + EVT_GPS_CRITICAL]);
//		gps_warnings.push_back(evdu[file_shift + EVT_GPS_WARNING]);

		// Convert the GPS times into unix time. This assumes we get UTC from the GPS
		TTimeStamp ts;
		ts.Set(evdu[EVT_YEAR], (evdu[EVT_DAYMONTH]>>16)&0xff, (evdu[EVT_DAYMONTH]>>24)&0xff, evdu[EVT_SECMINHOUR]&0xff, (evdu[EVT_SECMINHOUR]>>8)&0xff, (evdu[EVT_SECMINHOUR]>>16)&0xff, 0, true, 0);
		gps_time.push_back(ts.GetSec());

		gps_long.push_back(*(unsigned long long*)&evdu[EVT_LONGITUDE-1]);
		gps_lat.push_back(*(unsigned long long*)&evdu[EVT_LATITUDE-1]);
		gps_alt.push_back(*(unsigned long long*)&evdu[EVT_ALTITUDE-1]);
		gps_temp.push_back(*(unsigned int*)&evdu[EVT_TEMPERATURE]);

		// ToDo: Add seconds since sunday, week, utc offset, modes and work on alarms

		// ToDo: All 4 below from print_channel_info()
//		DigiCtrlDecodeAndFill(&evdu[file_shift + EVT_CTRL]);
//		DigiWindowDecodeAndFill(&evdu[file_shift + EVT_WINDOWS]);
//		ChannelPropertyDecodeAndFill((unsigned short*)&evdu[file_shift + EVT_CHANNEL]);
//		ChannelTriggerParameterDecodeAndFill((unsigned short*)&evdu[file_shift + EVT_TRIGGER]);

		// ToDo: Add notch filters

		// ToDo: check if there is anything like ioff now
//		ioff.push_back(evdu[file_shift + EVT_HDRLEN]);

		int start_addr = EVT_START_ADC;

		// Merge the traces
		trace_ch.emplace_back();
		int end_addr = start_addr+(evdu[EVT_TRACELENGTH]>>16);
		trace_ch.back().emplace_back();
		// Swapping the order of bytes in the traces values
		for(int i=0; i<(evdu[EVT_TRACELENGTH]>>16); ++i)
		{
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2+1]);
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2]);
		}
		start_addr=end_addr;
		end_addr = start_addr+(evdu[EVT_TRACELENGTH]>>16);
		trace_ch.back().emplace_back();
		for(int i=0; i<(evdu[EVT_TRACELENGTH]>>16); ++i)
		{
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2+1]);
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2]);
		}
		start_addr=end_addr;
		end_addr = start_addr+(evdu[EVT_TRACELENGTH]>>16);
		trace_ch.back().emplace_back();
		for(int i=0; i<(evdu[EVT_TRACELENGTH]>>16); ++i)
		{
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2+1]);
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2]);
		}
		start_addr=end_addr;
		end_addr = start_addr+(evdu[EVT_TRACELENGTH]>>16);
		trace_ch.back().emplace_back();
		for(int i=0; i<(evdu[EVT_TRACELENGTH]>>16); ++i)
		{
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2+1]);
			trace_ch.back().back().push_back(((short*)(&evdu[start_addr]))[i*2]);
		}

		idu += (evdu[EVT_LENGTH]>>16);

		du_counter++;
		// Safety check of the amount of DUs in the event. If too big, the file reading most likely went wrong.
		if(du_counter>=safe_du_amount)
		{
			cerr << "The amount of DUs in the event exceeded " << safe_du_amount << ". Too much!" << endl;
			cerr << "Make sure that the file format is correct and the file is not corrupted." << endl;
			cerr << "Exiting, to avoid a memory hog." << endl;
			return -1;
		}
	}

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

	pps_id.clear();
	fpga_temp.clear();
	adc_temp.clear();

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
//	adc_input_channels.clear();
	adc_input_channels_ch.clear();
//	adc_enabled_channels.clear();
	adc_enabled_channels_ch.clear();
	adc_samples_count_total.clear();
//	adc_samples_count_channel0.clear();
//	adc_samples_count_channel1.clear();
//	adc_samples_count_channel2.clear();
//	adc_samples_count_channel3.clear();
	adc_samples_count_ch.clear();

//	trigger_pattern.clear();
	trigger_pattern_ch.clear();
	trigger_pattern_ch0_ch1.clear();
	trigger_pattern_notch0_ch1.clear();
	trigger_pattern_redch0_ch1.clear();
	trigger_pattern_ch2_ch3.clear();
	trigger_pattern_calibration.clear();
	trigger_pattern_10s.clear();
	trigger_pattern_external_test_pulse.clear();


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

//	digi_ctrl.clear();
	enable_auto_reset_timeout.clear();
	force_firmware_reset.clear();
	enable_filter_ch.clear();
	enable_1PPS.clear();
	enable_DAQ.clear();
	enable_trigger_ch.clear();
	enable_trigger_ch0_ch1.clear();
	enable_trigger_notch0_ch1.clear();
	enable_trigger_redch0_ch1.clear();
	enable_trigger_ch2_ch3.clear();
	enable_trigger_calibration.clear();
	enable_trigger_10s.clear();
	enable_trigger_external_test_pulse.clear();
	enable_readout_ch.clear();
	fire_single_test_pulse.clear();
	test_pulse_rate_divider.clear();
	common_coincidence_time.clear();
	selector_readout_ch.clear();

//	digi_prepost_trig_windows.clear();
	pre_coincidence_window_ch.clear();
	post_coincidence_window_ch.clear();

//	channel_properties0.clear();
//	channel_properties1.clear();
//	channel_properties2.clear();
//	channel_properties3.clear();
	gain_correction_ch.clear();
	integration_time_ch.clear();
	offset_correction_ch.clear();
	base_maximum_ch.clear();
	base_minimum_ch.clear();

//	channel_trig_settings0.clear();
//	channel_trig_settings1.clear();
//	channel_trig_settings2.clear();
//	channel_trig_settings3.clear();
	signal_threshold_ch.clear();
	noise_threshold_ch.clear();
	tprev_ch.clear();
	tper_ch.clear();
	tcmax_ch.clear();
	ncmax_ch.clear();
	ncmin_ch.clear();
	qmax_ch.clear();
	qmin_ch.clear();

	// ToDo: What is it?
	ioff.clear();
//	trace_0.clear();
//	trace_1.clear();
//	trace_2.clear();
//	trace_3.clear();
	trace_ch.clear();
}

//! Initialises the TTree metadata fields
void TADC::InitialiseMetadata()
{
	this->creation_datetime = (new TDatime())->Convert(true);

	this->tadc->GetUserInfo()->Add(new TNamed("type", this->type));
	this->tadc->GetUserInfo()->Add(new TNamed("comment", this->comment));
	this->tadc->GetUserInfo()->Add(new TParameter<int>("creation_datetime", this->creation_datetime));
	this->tadc->GetUserInfo()->Add(new TNamed("modification_history", this->modification_history));
	this->tadc->GetUserInfo()->Add(new TParameter<int>("source_datetime", this->source_datetime));
	this->tadc->GetUserInfo()->Add(new TNamed("modification_software", this->modification_software));
	this->tadc->GetUserInfo()->Add(new TNamed("modification_software_version", this->modification_software_version));
	this->tadc->GetUserInfo()->Add(new TParameter<int>("analysis_level", this->analysis_level));
}

void TADC::TriggerPatternDecodeAndFill(unsigned short trigger_pattern)
{
	auto bits = bitset<16>{trigger_pattern};

	trigger_pattern_ch.push_back(vector<bool>{bits[8], bits[9], bits[10], bits[11]});

	trigger_pattern_ch0_ch1.push_back(bits[7]);
	trigger_pattern_notch0_ch1.push_back(bits[2]);
	trigger_pattern_redch0_ch1.push_back(bits[1]);
	trigger_pattern_ch2_ch3.push_back(bits[0]);
	trigger_pattern_calibration.push_back(bits[6]);
	trigger_pattern_10s.push_back(bits[5]);
	trigger_pattern_external_test_pulse.push_back(bits[4]);
}

void TADC::TriggerPatternDecodeAndFill_fv2(unsigned short trigger_pattern)
{
	auto bits = bitset<16>{trigger_pattern};

	trigger_pattern_ch.push_back(vector<bool>{bits[0], bits[1], bits[2], bits[3]});

	trigger_pattern_ch0_ch1.push_back(bits[4]);
	trigger_pattern_ch0_ch1_ch2.push_back(bits[5]);
	trigger_pattern_ch0_ch1_notch2.push_back(bits[6]);
	trigger_pattern_20Hz.push_back(bits[7]);
	trigger_pattern_10s.push_back(bits[8]);
	trigger_pattern_external_test_pulse.push_back(bits[9]);

	int period =((trigger_pattern>>13)&0xf)<<(2+((trigger_pattern>>9)&0xf));
	trigger_external_test_pulse_period.push_back(period);
}


void TADC::DigiCtrlDecodeAndFill(unsigned short digi_ctrl[8])
{

	// Digitizer control register
	auto bits = bitset<16>{digi_ctrl[0]};

	enable_auto_reset_timeout.push_back(bits[15]);
	force_firmware_reset.push_back(bits[14]);
	enable_filter_ch.push_back(vector<bool>{bits[8], bits[9], bits[10], bits[11]}); //
	enable_1PPS.push_back(bits[1]);
	enable_DAQ.push_back(bits[0]);

	// Trigger enable mask register
	bits = bitset<16>{digi_ctrl[1]};

	enable_trigger_ch.push_back(vector<bool>{bits[8], bits[9], bits[10], bits[11]}); //
	enable_trigger_ch0_ch1.push_back(bits[7]); //
	enable_trigger_notch0_ch1.push_back(bits[2]);
	enable_trigger_redch0_ch1.push_back(bits[1]);
	enable_trigger_ch2_ch3.push_back(bits[0]);
	enable_trigger_calibration.push_back(bits[6]);
	enable_trigger_10s.push_back(bits[5]); //
	enable_trigger_external_test_pulse.push_back(bits[4]);

	// Test pulse rate divider and channel readout enable
	bits = bitset<16>{digi_ctrl[2]};

	enable_readout_ch.push_back(vector<bool>{bits[0], bits[1], bits[2], bits[3]});
	fire_single_test_pulse.push_back(bits[7]);
	// Store second byte of digi_ctrl[2]
	test_pulse_rate_divider.push_back((digi_ctrl[2] >> (8*1)) & 0xff);

	// Common coincidence readout time window
	common_coincidence_time.push_back(digi_ctrl[3]);

	// Input selector for readout channel
	selector_readout_ch.push_back(vector<unsigned char>{(unsigned char)((digi_ctrl[4]>>0)&0b0111), (unsigned char)((digi_ctrl[4]>>4)&0b0111), (unsigned char)((digi_ctrl[4]>>8)&0b0111), (unsigned char)((digi_ctrl[4]>>12)&0b0111)});
}

void TADC::DigiWindowDecodeAndFill(unsigned short digi_window[8])
{
	pre_coincidence_window_ch.push_back(vector<unsigned short>{digi_window[0], digi_window[2], digi_window[4], digi_window[6]});
	post_coincidence_window_ch.push_back(vector<unsigned short>{digi_window[1], digi_window[3], digi_window[5], digi_window[7]});
}

void TADC::ChannelPropertyDecodeAndFill(unsigned short chprop[24])
{
	gain_correction_ch.push_back(vector<unsigned short>{chprop[0], chprop[6], chprop[12], chprop[18]});
	integration_time_ch.push_back(vector<unsigned char>{(unsigned char)((chprop[1]>>8)&0xff), (unsigned char)((chprop[7]>>8)&0xff), (unsigned char)((chprop[13]>>8)&0xff), (unsigned char)((chprop[19]>>8)&0xff)});
	offset_correction_ch.push_back(vector<unsigned char>{(unsigned char)((chprop[1]>>0)&0xff), (unsigned char)((chprop[7]>>0)&0xff), (unsigned char)((chprop[13]>>0)&0xff), (unsigned char)((chprop[19]>>0)&0xff)});;
	base_maximum_ch.push_back(vector<unsigned short>{chprop[2], chprop[8], chprop[14], chprop[20]});
	base_minimum_ch.push_back(vector<unsigned short>{chprop[3], chprop[9], chprop[15], chprop[21]});
}

void TADC::ChannelTriggerParameterDecodeAndFill(unsigned short chprop[24])
{
	signal_threshold_ch.push_back(vector<unsigned short>{chprop[0], chprop[6], chprop[12], chprop[18]});
	noise_threshold_ch.push_back(vector<unsigned short>{chprop[1], chprop[7], chprop[13], chprop[19]});

	// Cast the short array to char - easier to access from now on
	unsigned char *chpropchar = (unsigned char*)chprop;

	tprev_ch.push_back(vector<unsigned char>{chpropchar[4], chpropchar[16], chpropchar[28], chpropchar[40]});
	tper_ch.push_back(vector<unsigned char>{chpropchar[5], chpropchar[17], chpropchar[29], chpropchar[41]});
	tcmax_ch.push_back(vector<unsigned char>{chpropchar[6], chpropchar[18], chpropchar[30], chpropchar[42]});
	ncmax_ch.push_back(vector<unsigned char>{chpropchar[7], chpropchar[19], chpropchar[31], chpropchar[43]});
	ncmin_ch.push_back(vector<unsigned char>{chpropchar[8], chpropchar[20], chpropchar[32], chpropchar[44]});
	qmax_ch.push_back(vector<unsigned char>{chpropchar[9], chpropchar[21], chpropchar[33], chpropchar[45]});
	qmin_ch.push_back(vector<unsigned char>{chpropchar[10], chpropchar[22], chpropchar[34], chpropchar[46]});
}

void TADC::ADCInputChannelsDecodeAndFill(unsigned short val)
{
	adc_input_channels_ch.push_back(vector<unsigned char>{(unsigned char)((val>>0)&0b0111), (unsigned char)((val>>4)&0b0111), (unsigned char)((val>>8)&0b0111), (unsigned char)((val>>12)&0b0111)});
}

void TADC::ADCInputChannelsDecodeAndFill_fv2(unsigned short val)
{
	unsigned char adc_val[3];
	for(int ich=0;ich<3;ich++)
	{
		// Channel off
		if(((val>>5*ich)&0x1e) == 0) adc_val[ich]=15;
		else
		{
			for(int iadc=0;iadc<4;iadc++){
				if(val&(1<<(5*ich+iadc+1)))
				{
					// Filtered channel
					if (val & (1 << (5 * ich))) adc_val[ich] = iadc+4;
					// Unfiltered channel
					else adc_val[ich] = iadc;
				}
			}
		}
	}
	adc_input_channels_ch.push_back(vector<unsigned char>{adc_val[0], adc_val[1], adc_val[2]});
}

void TADC::ADCEnabledChannelsDecodeAndFill(unsigned short val)
{
	auto bits = bitset<16>{val};
	adc_enabled_channels_ch.push_back(vector<bool>{bits[0], bits[1], bits[2], bits[3]});
}

short TADC::ADC2short(short val)
{
	short value = val;
	short bit14 = (value & ( 1 << 13 )) >> 13;
	int mask = 1 << 14; // --- bit 15
	value = (value & (~mask)) | (bit14 << 14);
	mask = 1 << 15; // --- bit 16
	value = (value & (~mask)) | (bit14 << 15);
	return value;
}

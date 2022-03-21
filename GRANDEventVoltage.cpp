//
// Created by lewhoo on 17/02/2022.
//
// The main class for holding the Voltage traces and some other data relative to them

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include "ROOT/RDataFrame.hxx"
#include "GRANDEventVoltage.h"

using namespace std;
using namespace ROOT;

//! General constructor
GRANDEventVoltage::GRANDEventVoltage()
{
	// Initialize the TTree
	CreateTree();
}

//! Constructor computing values from teventadc
GRANDEventVoltage::GRANDEventVoltage(GRANDEventADC *adc) : GRANDEventVoltage()
{
	auto teventadc = adc->teventadc;

	teventvoltage->Print();

	// Exclude these branches from copying
	vector<string> excluded_branches = {"gps_long", "gps_lat", "gps_alt", "gps_temp"};

	// *** Transform the teventadc events into teventvoltage events ***

	// Loop through teventvoltage branches and set their addresses to teventadc branches
	for (auto br : TRangeDynCast<TBranch>( teventvoltage->GetListOfBranches() ))
	{
		if (!br) continue;

		// Attempt to get the branch from teventadc
		auto adc_br = teventadc->FindBranch(br->GetName());

		// If the branch exists in teventadc and not on excluded list
		if(adc_br && find(excluded_branches.begin(), excluded_branches.end(), adc_br->GetName())==end(excluded_branches))
		{
			// Set the teventvoltage branch address to the teventadc branch address
			br->SetAddress(adc_br->GetAddress());
		}
	}

	// Loop through the teventadc events and fill the teventvoltage with the corresponding values
	for(int entry_no=0; entry_no<teventadc->GetEntries(); ++entry_no)
	{
		teventadc->GetEntry(entry_no);

		// *** Calculate the values not existing in GRANDEventADC ***
		ADCs2Real(adc);
		teventvoltage->Fill();
	}
	teventvoltage->BuildIndex("run_number", "event_number");
	teventvoltage->AddFriend(teventadc);
}


void GRANDEventVoltage::ADCs2Real(GRANDEventADC *adc)
{
	// Clear the traces vectors
	trace_x.clear();
	trace_y.clear();
	trace_z.clear();

	// Clear the GPS vectors
	gps_long.clear();
	gps_lat.clear();
	gps_alt.clear();
	gps_temp.clear();

	// Loop through the DUs
	for (size_t i=0; i<adc->du_count; ++i)
	{
		// Create this DU's vectors
		trace_x.push_back(vector<float>());
		trace_y.push_back(vector<float>());
		trace_z.push_back(vector<float>());
		// Convert this specific DU's ADCs to Voltage
		TraceADC2Voltage(i, adc);
		// Convert GPS ADC to real values
		GPSADC2Real(i, adc);
	}
}

void GRANDEventVoltage::TraceADC2Voltage(int du_num, GRANDEventADC *adc)
{
	// Probably in the future adc2voltageconst will be replaced in the transform by some array_x/y/z[du_id], or corresponding function in some non-linear case
	// Also, at the moment I assume trace_0/1/2 are x/y/z - this may also change in the future
	// The conversion factor is just taken from the information for XiHu data, that "For currently ADC, the differential input voltage range is 1.8V (Vpp), that is -0.9V to 0.9V corresponding to ADC value -8192 to 8192"
	float adc2voltageconst=0.9/8192;
	trace_x[du_num].resize(adc->trace_0[du_num].size());
	transform(adc->trace_0[du_num].begin(), adc->trace_0[du_num].end(), trace_x[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });
	trace_y[du_num].resize(adc->trace_1[du_num].size());
	transform(adc->trace_1[du_num].begin(), adc->trace_1[du_num].end(), trace_y[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });
	trace_z[du_num].resize(adc->trace_2[du_num].size());
	transform(adc->trace_2[du_num].begin(), adc->trace_2[du_num].end(), trace_z[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });
}

void GRANDEventVoltage::GPSADC2Real(int du_num, GRANDEventADC *adc)
{
		gps_long.push_back(57.3*(double)(adc->gps_long[du_num]));
		gps_lat.push_back(57.3*(double)adc->gps_lat[du_num]);
		gps_alt.push_back(adc->gps_alt[du_num]);
		gps_temp.push_back(adc->gps_temp[du_num]);
}

TTree *GRANDEventVoltage::CreateTree()
{
	teventvoltage = new TTree("teventvoltage", "Event with Voltage traces and other information");

	// Create Branches for all stored data
	teventvoltage->Branch("event_size", &event_size, "event_size/i");
	teventvoltage->Branch("run_number", &run_number, "run_number/i");
	teventvoltage->Branch("event_number", &event_number, "event_number/i");
	teventvoltage->Branch("t3_number", &t3_number, "t3_number/i");
	teventvoltage->Branch("first_du", &first_du, "first_du/i");
	teventvoltage->Branch("time_seconds", &time_seconds, "time_seconds/i");
	teventvoltage->Branch("time_nanoseconds", &time_nanoseconds, "time_nanoseconds/i");
	teventvoltage->Branch("event_type", &event_type, "event_type/i");
	teventvoltage->Branch("event_version", &event_version, "event_version/i");
	teventvoltage->Branch("du_count", &du_count, "du_count/i");
	// Vector branches
	teventvoltage->Branch("event_id", &event_id);
	teventvoltage->Branch("du_id", &du_id);
	teventvoltage->Branch("du_seconds", &du_seconds);
	teventvoltage->Branch("du_nanoseconds", &du_nanoseconds);
	teventvoltage->Branch("trigger_position", &trigger_position);
	teventvoltage->Branch("trigger_flag", &trigger_flag);
	teventvoltage->Branch("atm_temperature", &atm_temperature);
	teventvoltage->Branch("atm_pressure", &atm_pressure);
	teventvoltage->Branch("atm_humidity", &atm_humidity);
	teventvoltage->Branch("acceleration_x", &acceleration_x);
	teventvoltage->Branch("acceleration_y", &acceleration_y);
	teventvoltage->Branch("acceleration_z", &acceleration_z);
	teventvoltage->Branch("battery_level", &battery_level);
	teventvoltage->Branch("firmware_version", &firmware_version);
	teventvoltage->Branch("adc_sampling_frequency", &adc_sampling_frequency);
	teventvoltage->Branch("adc_sampling_resolution", &adc_sampling_resolution);
	teventvoltage->Branch("adc_input_channels", &adc_input_channels);
	teventvoltage->Branch("adc_enabled_channels", &adc_enabled_channels);
	teventvoltage->Branch("adc_samples_count_total", &adc_samples_count_total);
	teventvoltage->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
	teventvoltage->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
	teventvoltage->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
	teventvoltage->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
	teventvoltage->Branch("trigger_pattern", &trigger_pattern);
	teventvoltage->Branch("trigger_rate", &trigger_rate);
	teventvoltage->Branch("clock_tick", &clock_tick);
	teventvoltage->Branch("clock_tics_per_second", &clock_tics_per_second);
	teventvoltage->Branch("gps_offset", &gps_offset);
	teventvoltage->Branch("gps_leap_second", &gps_leap_second);
	teventvoltage->Branch("gps_status", &gps_status);
	teventvoltage->Branch("gps_alarms", &gps_alarms);
	teventvoltage->Branch("gps_warnings", &gps_warnings);
	teventvoltage->Branch("gps_time", &gps_time);
	teventvoltage->Branch("gps_long", &gps_long);
	teventvoltage->Branch("gps_lat", &gps_lat);
	teventvoltage->Branch("gps_alt", &gps_alt);
	teventvoltage->Branch("gps_temp", &gps_temp);
	teventvoltage->Branch("digi_ctrl", &digi_ctrl);
	teventvoltage->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
	teventvoltage->Branch("channel_properties0", &channel_properties0);
	teventvoltage->Branch("channel_properties1", &channel_properties1);
	teventvoltage->Branch("channel_properties2", &channel_properties2);
	teventvoltage->Branch("channel_properties3", &channel_properties3);
	teventvoltage->Branch("channel_trig_settings0", &channel_trig_settings0);
	teventvoltage->Branch("channel_trig_settings1", &channel_trig_settings1);
	teventvoltage->Branch("channel_trig_settings2", &channel_trig_settings2);
	teventvoltage->Branch("channel_trig_settings3", &channel_trig_settings3);
	teventvoltage->Branch("ioff", &ioff);
	teventvoltage->Branch("trace_x", &trace_x);
	teventvoltage->Branch("trace_y", &trace_y);
	teventvoltage->Branch("trace_z", &trace_z);

	this->teventvoltage = teventvoltage;

	return teventvoltage;
}

void GRANDEventVoltage::ClearVectors()
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
	clock_tics_per_second.clear();
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
	trace_x.clear();
	trace_y.clear();
	trace_z.clear();
}

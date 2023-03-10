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
#include "TRawVoltage.h"

using namespace std;
using namespace ROOT;

//! General constructor
TRawVoltage::TRawVoltage()
{
	// Initialize the TTree
	CreateTree();
}

//! Constructor computing values from tadc
TRawVoltage::TRawVoltage(TADC *adc) : TRawVoltage()
{
	auto tadc = adc->tadc;

//	trawvoltage->Print();

	// Exclude these branches from copying
	vector<string> excluded_branches = {"gps_long", "gps_lat", "gps_alt", "gps_temp"};

	// *** Transform the tadc events into trawvoltage events ***

	// Loop through trawvoltage branches and set their addresses to tadc branches
	for (auto br : TRangeDynCast<TBranch>( trawvoltage->GetListOfBranches() ))
	{
		if (!br) continue;

		// Attempt to get the branch from tadc
		auto adc_br = tadc->FindBranch(br->GetName());

		// If the branch exists in tadc and not on excluded list
		if(adc_br && find(excluded_branches.begin(), excluded_branches.end(), adc_br->GetName())==end(excluded_branches))
		{
			// Set the trawvoltage branch address to the tadc branch address
			br->SetAddress(adc_br->GetAddress());
		}
	}

	// Loop through the tadc events and fill the trawvoltage with the corresponding values
	for(int entry_no=0; entry_no<tadc->GetEntries(); ++entry_no)
	{
		tadc->GetEntry(entry_no);

		// *** Calculate the values not existing in TADC ***
		// ToDo: Calling to separate functions results in two loops over traces. Definitely not optimal, but maybe fast enough for now.
		ADCs2Real(adc);
//		CalculateT0s(adc);
		trawvoltage->Fill();
	}
	trawvoltage->BuildIndex("run_number", "event_number");
	trawvoltage->AddFriend(tadc);
}


void TRawVoltage::ADCs2Real(TADC *adc)
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
//	for (size_t i=0; i<adc->du_count; ++i)
	for (size_t i=0; i<adc->trace_0.size(); ++i)
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

void TRawVoltage::TraceADC2Voltage(int du_num, TADC *adc)
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

void TRawVoltage::GPSADC2Real(int du_num, TADC *adc)
{
		gps_long.push_back(57.3*(double)(adc->gps_long[du_num]));
		gps_lat.push_back(57.3*(double)adc->gps_lat[du_num]);
		gps_alt.push_back(adc->gps_alt[du_num]);
		gps_temp.push_back(adc->gps_temp[du_num]);
}

void TRawVoltage::CalculateT0s(TADC *adc)
{
	// Clear the t0s vectors
	du_t0_seconds.clear();
	du_t0_nanoseconds.clear();

	// Resize the vectors, as the size is known
	du_t0_seconds.resize(adc->du_seconds.size());
	du_t0_nanoseconds.resize(adc->du_nanoseconds.size());

	// Loop through the DUs
//	for (size_t i=0; i<adc->du_count; ++i)
	for (size_t i=0; i<adc->trace_0.size(); ++i)
	{
		1000/adc->adc_sampling_frequency[i];

		// Trigger position in the trace in ns
		auto trigger_pos_ns = adc->trigger_position[i]*(1000/adc->adc_sampling_frequency[i]); // adc_sampling_frequency is in MHz, we want ns
		// Calculate this DU's t0s
		tie(du_t0_seconds[i], du_t0_nanoseconds[i]) = CalculateT0(adc->du_seconds[i], adc->du_nanoseconds[i], trigger_pos_ns);
//		du_t0_seconds.push_back(t0_seconds);
//		du_t0_nanoseconds.push_back(t0_nanoseconds);

	}
}

pair<unsigned int, unsigned int> TRawVoltage::CalculateT0(unsigned int seconds, unsigned int nanoseconds, unsigned int trigger_pos_ns)
{
	unsigned int t0_seconds=seconds, t0_nanoseconds=nanoseconds;
	// If nanoseconds of the trigger are greater/equal than trigger_pos_ns
	if(nanoseconds>=trigger_pos_ns)
	{
		// Just reduce the nanoseconds
		t0_nanoseconds-=trigger_pos_ns;
	}
		// Nanoseconds are lower than trigger_pos_ns
	else
	{
		// Reduce the seconds by 1 (assuming the trigger position in the trace is not >1 s)
		t0_seconds-=1;
		// Calculate the nanoseconds below 1
		t0_nanoseconds=1e9-(trigger_pos_ns-nanoseconds);
	}
	return pair<unsigned int, unsigned int>(t0_seconds, t0_nanoseconds);
}

TTree *TRawVoltage::CreateTree()
{
	trawvoltage = new TTree("trawvoltage", "Event with Voltage traces and other information");

	// Create Branches for all stored data
	trawvoltage->Branch("event_size", &event_size, "event_size/i");
	trawvoltage->Branch("run_number", &run_number, "run_number/i");
	trawvoltage->Branch("event_number", &event_number, "event_number/i");
	trawvoltage->Branch("t3_number", &t3_number, "t3_number/i");
	trawvoltage->Branch("first_du", &first_du, "first_du/i");
	trawvoltage->Branch("time_seconds", &time_seconds, "time_seconds/i");
	trawvoltage->Branch("time_nanoseconds", &time_nanoseconds, "time_nanoseconds/i");
	trawvoltage->Branch("event_type", &event_type, "event_type/i");
	trawvoltage->Branch("event_version", &event_version, "event_version/i");
	trawvoltage->Branch("du_count", &du_count, "du_count/i");
	// Vector branches
	trawvoltage->Branch("event_id", &event_id);
	trawvoltage->Branch("du_id", &du_id);
	trawvoltage->Branch("du_seconds", &du_seconds);
	trawvoltage->Branch("du_nanoseconds", &du_nanoseconds);
//	trawvoltage->Branch("du_t0_seconds", &du_t0_seconds);
//	trawvoltage->Branch("du_t0_nanoseconds", &du_t0_nanoseconds);
	trawvoltage->Branch("trigger_position", &trigger_position);
	trawvoltage->Branch("trigger_flag", &trigger_flag);
	trawvoltage->Branch("atm_temperature", &atm_temperature);
	trawvoltage->Branch("atm_pressure", &atm_pressure);
	trawvoltage->Branch("atm_humidity", &atm_humidity);
	trawvoltage->Branch("acceleration_x", &acceleration_x);
	trawvoltage->Branch("acceleration_y", &acceleration_y);
	trawvoltage->Branch("acceleration_z", &acceleration_z);
	trawvoltage->Branch("battery_level", &battery_level);
	trawvoltage->Branch("firmware_version", &firmware_version);
	trawvoltage->Branch("adc_sampling_frequency", &adc_sampling_frequency);
	trawvoltage->Branch("adc_sampling_resolution", &adc_sampling_resolution);
	trawvoltage->Branch("adc_input_channels", &adc_input_channels);
	trawvoltage->Branch("adc_enabled_channels", &adc_enabled_channels);
	trawvoltage->Branch("adc_samples_count_total", &adc_samples_count_total);
	trawvoltage->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
	trawvoltage->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
	trawvoltage->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
	trawvoltage->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
	trawvoltage->Branch("trigger_pattern", &trigger_pattern);
	trawvoltage->Branch("trigger_rate", &trigger_rate);
	trawvoltage->Branch("clock_tick", &clock_tick);
	trawvoltage->Branch("clock_ticks_per_second", &clock_ticks_per_second);
	trawvoltage->Branch("gps_offset", &gps_offset);
	trawvoltage->Branch("gps_leap_second", &gps_leap_second);
	trawvoltage->Branch("gps_status", &gps_status);
	trawvoltage->Branch("gps_alarms", &gps_alarms);
	trawvoltage->Branch("gps_warnings", &gps_warnings);
	trawvoltage->Branch("gps_time", &gps_time);
	trawvoltage->Branch("gps_long", &gps_long);
	trawvoltage->Branch("gps_lat", &gps_lat);
	trawvoltage->Branch("gps_alt", &gps_alt);
	trawvoltage->Branch("gps_temp", &gps_temp);
	trawvoltage->Branch("pos_x", &pos_x);
	trawvoltage->Branch("pos_y", &pos_y);
	trawvoltage->Branch("pos_z", &pos_z);
	trawvoltage->Branch("digi_ctrl", &digi_ctrl);
	trawvoltage->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
	trawvoltage->Branch("channel_properties0", &channel_properties0);
	trawvoltage->Branch("channel_properties1", &channel_properties1);
	trawvoltage->Branch("channel_properties2", &channel_properties2);
	trawvoltage->Branch("channel_properties3", &channel_properties3);
	trawvoltage->Branch("channel_trig_settings0", &channel_trig_settings0);
	trawvoltage->Branch("channel_trig_settings1", &channel_trig_settings1);
	trawvoltage->Branch("channel_trig_settings2", &channel_trig_settings2);
	trawvoltage->Branch("channel_trig_settings3", &channel_trig_settings3);
	trawvoltage->Branch("ioff", &ioff);
	trawvoltage->Branch("trace_x", &trace_x);
	trawvoltage->Branch("trace_y", &trace_y);
	trawvoltage->Branch("trace_z", &trace_z);

	this->trawvoltage = trawvoltage;

	return trawvoltage;
}

void TRawVoltage::ClearVectors()
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
	pos_x.clear();
	pos_y.clear();
	pos_z.clear();
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

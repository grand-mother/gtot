//
// Created by lewhoo on 17/02/2022.
//
// The main class for holding the voltage related information about the run

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include "TDatime.h"
#include "TNamed.h"
#include "TParameter.h"
#include "inc/TRunRawVoltage.h"
#include "TFile.h"

using namespace std;
using namespace ROOT;

//! General constructor
TRunRawVoltage::TRunRawVoltage(TFile *out_file)
{
	// Initialize the TTree
	CreateTree();

	// Set the output file before filling, if provided
	if (out_file != NULL)
	{
		this->trunvoltage->SetDirectory(out_file);
	}
}

//! Constructor computing values from tadc
TRunRawVoltage::TRunRawVoltage(TADC *adc, bool is_fv2, TFile *out_file) : TRunRawVoltage(out_file)
{
	// Initialise metadata
	InitialiseMetadata();

	ComputeFromADC(adc, is_fv2);
}

//! Compute values from tadc
void TRunRawVoltage::ComputeFromADC(TADC *adc, bool is_fv2)
{
	auto tadc = adc->tadc;

	// Exclude these branches from copying, for the values need to be modified
	vector<string> excluded_branches = {"gps_long", "gps_lat", "gps_alt", "gps_temp", "trace_0", "trace_1", "trace_2", "trace_3", "trace_ch", "battery_level", "fpga_temp", "adc_temp", "atm_temperature", "atm_pressure", "atm_humidity", "gain_correction_ch", "du_acceleration"};

	// *** Transform the tadc events into TRunRawVoltage events ***

	// Loop through TRunRawVoltage branches and set their addresses to tadc branches
	for (auto br : TRangeDynCast<TBranch>(TRunRawVoltage->GetListOfBranches() ))
	{
		if (!br) continue;

		// Attempt to get the branch from tadc
		auto adc_br = tadc->FindBranch(br->GetName());

		// If the branch exists in tadc and not on excluded list
		if(adc_br && find(excluded_branches.begin(), excluded_branches.end(), adc_br->GetName())==end(excluded_branches))
		{
			// Set the TRunRawVoltage branch address to the tadc branch address
			br->SetAddress(adc_br->GetAddress());
		}
	}

	// Loop through the tadc events and fill the TRunRawVoltage with the corresponding values
	for(int entry_no=0; entry_no<tadc->GetEntries(); ++entry_no)
	{
		tadc->GetEntry(entry_no);

		// *** Calculate the values not existing in TADC ***
		// ToDo: Calling to separate functions results in two loops over traces. Definitely not optimal, but maybe fast enough for now.
		ADCs2Real(adc, is_fv2);
//		CalculateT0s(adc);
		TRunRawVoltage->Fill();
	}
	TRunRawVoltage->BuildIndex("run_number", "event_number");
}


void TRunRawVoltage::ADCs2Real(TADC *adc, bool is_fv2)
{
	// Clear the traces vectors
	trace_ch.clear();

	// Clear the GPS vectors
	gps_long.clear();
	gps_lat.clear();
	gps_alt.clear();
	gps_temp.clear();

	// Clear the battery
	battery_level.clear();

	fpga_temp.clear();
	adc_temp.clear();

	atm_temperature.clear();
	atm_humidity.clear();
	atm_pressure.clear();

	gain_correction_ch.clear();

	du_acceleration.clear();

	// Loop through the DUs
//	for (size_t i=0; i<adc->du_count; ++i)
	for (size_t i=0; i<adc->trace_ch.size(); ++i)
	{
		// Create this DU's vectors
//		trace_ch.push_back(vector<vector<float>>());
//		trace_ch.back().push_back(vector<float>());
//		trace_ch.back().push_back(vector<float>());
//		trace_ch.back().push_back(vector<float>());
//		trace_ch.back().push_back(vector<float>());
		trace_ch.emplace_back();

		// Loop through traces dimensions
		for (int j = 0; j < adc->trace_ch[0].size(); ++j)
		{
			trace_ch.back().emplace_back();
		}
//		trace_ch.back().emplace_back();
//		trace_ch.back().emplace_back();
//		trace_ch.back().emplace_back();


		// Convert this specific DU's ADCs to Voltage
		TraceADC2Voltage(i, adc);
		// Convert GPS ADC to real values
		if(is_fv2)
		{
			// Firmware v2
			GPSADC2Real_fv2(i, adc);
			fpga_temp.push_back(adc->fpga_temp[i]*509.3140064/(1<<16)-280.23087870);
			adc_temp.push_back((adc->adc_temp[i]-819)/2.654+25);
		}
		else
			// Firmware v1
			GPSADC2Real(i, adc);
		// Convert battery level from ADCs to Voltage
		BatteryADC2Voltage(i, adc);

		atm_temperature.push_back(((adc->atm_temperature[i]*2500./4096)-400)/19.5);
		atm_humidity.push_back(((adc->atm_humidity[i]*2.5/4096/3.3)-0.1515)/0.00636);
		// ToDo: find the conversion
		atm_pressure.push_back(adc->atm_pressure[i]*1.);

		gain_correction_ch.push_back(vector<float>{(float)((adc->gain_correction_ch[i][0]-0.5)*2.5*37.5/4096-14), (float)((adc->gain_correction_ch[i][1]-0.5)*2.5*37.5/4096-14), (float)((adc->gain_correction_ch[i][2]-0.5)*2.5*37.5/4096-14), (float)((adc->gain_correction_ch[i][3]-0.5)*2.5*37.5/4096-14)});

		// ToDo: here should be some proper acceleration conversion when... we get some acceleration information
		du_acceleration.push_back(vector<float>{(float)adc->du_acceleration[i][0], (float)adc->du_acceleration[i][1], (float)adc->du_acceleration[i][2]});
	}
//	// Merge the traces
//	trace_ch.push_back(trace_0);
//	trace_ch.push_back(trace_1);
//	trace_ch.push_back(trace_2);
//	trace_ch.push_back(trace_3);
}

void TRunRawVoltage::TraceADC2Voltage(int du_num, TADC *adc)
{
	// Probably in the future adc2voltageconst will be replaced in the transform by some array_x/y/z[du_id], or corresponding function in some non-linear case
	// Also, at the moment I assume trace_0/1/2 are x/y/z - this may also change in the future
	// The conversion factor is just taken from the information for XiHu data, that "For currently ADC, the differential input voltage range is 1.8V (Vpp), that is -0.9V to 0.9V corresponding to ADC value -8192 to 8192"
	float adc2voltageconst = 0.9 / 8192 * 1e6;
//	trace_0[du_num].resize(adc->trace_0[du_num].size());
//	transform(adc->trace_0[du_num].begin(), adc->trace_0[du_num].end(), trace_0[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });
//	trace_1[du_num].resize(adc->trace_1[du_num].size());
//	transform(adc->trace_1[du_num].begin(), adc->trace_1[du_num].end(), trace_1[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });
//	trace_2[du_num].resize(adc->trace_2[du_num].size());
//	transform(adc->trace_2[du_num].begin(), adc->trace_2[du_num].end(), trace_2[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });
//	trace_3[du_num].resize(adc->trace_3[du_num].size());
//	transform(adc->trace_3[du_num].begin(), adc->trace_3[du_num].end(), trace_3[du_num].begin(), [adc2voltageconst](short &c){ return c*adc2voltageconst; });

	// Loop through traces dimensions
	for (int i = 0; i < adc->trace_ch[du_num].size(); ++i)
	{
		trace_ch[du_num][i].resize(adc->trace_ch[du_num][i].size());
		transform(adc->trace_ch[du_num][i].begin(), adc->trace_ch[du_num][i].end(), trace_ch[du_num][i].begin(), [adc2voltageconst](short &c) { return c * adc2voltageconst; });
//		trace_ch[du_num][1].resize(adc->trace_ch[du_num][1].size());
//		transform(adc->trace_ch[du_num][1].begin(), adc->trace_ch[du_num][1].end(), trace_ch[du_num][1].begin(), [adc2voltageconst](short &c) { return c * adc2voltageconst; });
//		trace_ch[du_num][2].resize(adc->trace_ch[du_num][2].size());
//		transform(adc->trace_ch[du_num][2].begin(), adc->trace_ch[du_num][2].end(), trace_ch[du_num][2].begin(), [adc2voltageconst](short &c) { return c * adc2voltageconst; });
//		trace_ch[du_num][3].resize(adc->trace_ch[du_num][3].size());
//		transform(adc->trace_ch[du_num][3].begin(), adc->trace_ch[du_num][3].end(), trace_ch[du_num][3].begin(), [adc2voltageconst](short &c) { return c * adc2voltageconst; });
	}
}

void TRunRawVoltage::GPSADC2Real(int du_num, TADC *adc)
{
		gps_long.push_back(57.3*(*(double*)&adc->gps_long[du_num]));
		gps_lat.push_back(57.3*(*(double*)&adc->gps_lat[du_num]));
		gps_alt.push_back(*(double*)&adc->gps_alt[du_num]);
		gps_temp.push_back(*(float*)&adc->gps_temp[du_num]);
}

void TRunRawVoltage::GPSADC2Real_fv2(int du_num, TADC *adc)
{
	double longitude,latitude,altitude;
	((int *)&longitude)[1] = ((int*)&adc->gps_long[du_num])[0];
	((int *)&longitude)[0] = ((int*)&adc->gps_long[du_num])[1];
	((int *)&latitude)[1] = ((int*)&adc->gps_lat[du_num])[0];
	((int *)&latitude)[0] = ((int*)&adc->gps_lat[du_num])[1];
	((int *)&altitude)[1] = ((int*)&adc->gps_alt[du_num])[0];
	((int *)&altitude)[0] = ((int*)&adc->gps_alt[du_num])[1];

	gps_long.push_back(57.3*longitude);
	gps_lat.push_back(57.3*latitude);
	gps_alt.push_back(altitude);
	gps_temp.push_back(*(float*)&adc->gps_temp[du_num]);
}

void TRunRawVoltage::BatteryADC2Voltage(int du_num, TADC *adc)
{
		battery_level.push_back(adc->battery_level[du_num] / ((18 * 4096) / (2.5*(18+91))));
}

//void TRunRawVoltage::CalculateT0s(TADC *adc)
//{
//	// Clear the t0s vectors
//	du_t0_seconds.clear();
//	du_t0_nanoseconds.clear();
//
//	// Resize the vectors, as the size is known
//	du_t0_seconds.resize(adc->du_seconds.size());
//	du_t0_nanoseconds.resize(adc->du_nanoseconds.size());
//
//	// Loop through the DUs
////	for (size_t i=0; i<adc->du_count; ++i)
//	for (size_t i=0; i<adc->trace_0.size(); ++i)
//	{
//		1000/adc->adc_sampling_frequency[i];
//
//		// Trigger position in the trace in ns
//		auto trigger_pos_ns = adc->trigger_position[i]*(1000/adc->adc_sampling_frequency[i]); // adc_sampling_frequency is in MHz, we want ns
//		// Calculate this DU's t0s
//		tie(du_t0_seconds[i], du_t0_nanoseconds[i]) = CalculateT0(adc->du_seconds[i], adc->du_nanoseconds[i], trigger_pos_ns);
////		du_t0_seconds.push_back(t0_seconds);
////		du_t0_nanoseconds.push_back(t0_nanoseconds);
//
//	}
//}
//
//pair<unsigned int, unsigned int> TRunRawVoltage::CalculateT0(unsigned int seconds, unsigned int nanoseconds, unsigned int trigger_pos_ns)
//{
//	unsigned int t0_seconds=seconds, t0_nanoseconds=nanoseconds;
//	// If nanoseconds of the trigger are greater/equal than trigger_pos_ns
//	if(nanoseconds>=trigger_pos_ns)
//	{
//		// Just reduce the nanoseconds
//		t0_nanoseconds-=trigger_pos_ns;
//	}
//		// Nanoseconds are lower than trigger_pos_ns
//	else
//	{
//		// Reduce the seconds by 1 (assuming the trigger position in the trace is not >1 s)
//		t0_seconds-=1;
//		// Calculate the nanoseconds below 1
//		t0_nanoseconds=1e9-(trigger_pos_ns-nanoseconds);
//	}
//	return pair<unsigned int, unsigned int>(t0_seconds, t0_nanoseconds);
//}

TTree *TRunRawVoltage::CreateTree()
{
	TRunRawVoltage = new TTree("TRunRawVoltage", "Event with Voltage traces and other information");

	// Create Branches for all stored data
	TRunRawVoltage->Branch("run_number", &run_number, "run_number/i");
	TRunRawVoltage->Branch("event_number", &event_number, "event_number/i");
	TRunRawVoltage->Branch("event_size", &event_size, "event_size/i");
//	TRunRawVoltage->Branch("t3_number", &t3_number, "t3_number/i");
	TRunRawVoltage->Branch("first_du", &first_du, "first_du/i");
	TRunRawVoltage->Branch("time_seconds", &time_seconds, "time_seconds/i");
	TRunRawVoltage->Branch("time_nanoseconds", &time_nanoseconds, "time_nanoseconds/i");
//	TRunRawVoltage->Branch("event_type", &event_type, "event_type/i");
//	TRunRawVoltage->Branch("event_version", &event_version, "event_version/i");
	TRunRawVoltage->Branch("du_count", &du_count, "du_count/i");
	// Vector branches
//	TRunRawVoltage->Branch("event_id", &event_id);
	TRunRawVoltage->Branch("du_id", &du_id);
	TRunRawVoltage->Branch("du_seconds", &du_seconds);
	TRunRawVoltage->Branch("du_nanoseconds", &du_nanoseconds);
//	TRunRawVoltage->Branch("du_t0_seconds", &du_t0_seconds);
//	TRunRawVoltage->Branch("du_t0_nanoseconds", &du_t0_nanoseconds);
	TRunRawVoltage->Branch("trigger_position", &trigger_position);
	TRunRawVoltage->Branch("trigger_flag", &trigger_flag);

	TRunRawVoltage->Branch("pps_id", &pps_id);
	TRunRawVoltage->Branch("fpga_temp", &fpga_temp);
	TRunRawVoltage->Branch("adc_temp", &adc_temp);

	TRunRawVoltage->Branch("atm_temperature", &atm_temperature);
	TRunRawVoltage->Branch("atm_pressure", &atm_pressure);
	TRunRawVoltage->Branch("atm_humidity", &atm_humidity);
	TRunRawVoltage->Branch("du_acceleration", &du_acceleration);
//	TRunRawVoltage->Branch("acceleration_x", &acceleration_x);
//	TRunRawVoltage->Branch("acceleration_y", &acceleration_y);
//	TRunRawVoltage->Branch("acceleration_z", &acceleration_z);
	TRunRawVoltage->Branch("battery_level", &battery_level);
	TRunRawVoltage->Branch("adc_samples_count_channel", &adc_samples_count_channel);
	TRunRawVoltage->Branch("gain_correction_ch", &gain_correction_ch);
//	TRunRawVoltage->Branch("firmware_version", &firmware_version);
//	TRunRawVoltage->Branch("adc_sampling_frequency", &adc_sampling_frequency);
//	TRunRawVoltage->Branch("adc_sampling_resolution", &adc_sampling_resolution);
//	TRunRawVoltage->Branch("adc_input_channels", &adc_input_channels);
//	TRunRawVoltage->Branch("adc_enabled_channels", &adc_enabled_channels);
//	TRunRawVoltage->Branch("adc_samples_count_total", &adc_samples_count_total);
//	TRunRawVoltage->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
//	TRunRawVoltage->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
//	TRunRawVoltage->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
//	TRunRawVoltage->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
	TRunRawVoltage->Branch("trigger_pattern", &trigger_pattern);
	TRunRawVoltage->Branch("trigger_rate", &trigger_rate);
	TRunRawVoltage->Branch("clock_tick", &clock_tick);
	TRunRawVoltage->Branch("clock_ticks_per_second", &clock_ticks_per_second);
	TRunRawVoltage->Branch("gps_offset", &gps_offset);
	TRunRawVoltage->Branch("gps_leap_second", &gps_leap_second);
	TRunRawVoltage->Branch("gps_status", &gps_status);
	TRunRawVoltage->Branch("gps_alarms", &gps_alarms);
	TRunRawVoltage->Branch("gps_warnings", &gps_warnings);
	TRunRawVoltage->Branch("gps_time", &gps_time);
	TRunRawVoltage->Branch("gps_long", &gps_long);
	TRunRawVoltage->Branch("gps_lat", &gps_lat);
	TRunRawVoltage->Branch("gps_alt", &gps_alt);
	TRunRawVoltage->Branch("gps_temp", &gps_temp);
//	TRunRawVoltage->Branch("pos_x", &pos_x);
//	TRunRawVoltage->Branch("pos_y", &pos_y);
//	TRunRawVoltage->Branch("pos_z", &pos_z);
//	TRunRawVoltage->Branch("digi_ctrl", &digi_ctrl);
//	TRunRawVoltage->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
//	TRunRawVoltage->Branch("channel_properties0", &channel_properties0);
//	TRunRawVoltage->Branch("channel_properties1", &channel_properties1);
//	TRunRawVoltage->Branch("channel_properties2", &channel_properties2);
//	TRunRawVoltage->Branch("channel_properties3", &channel_properties3);
//	TRunRawVoltage->Branch("channel_trig_settings0", &channel_trig_settings0);
//	TRunRawVoltage->Branch("channel_trig_settings1", &channel_trig_settings1);
//	TRunRawVoltage->Branch("channel_trig_settings2", &channel_trig_settings2);
//	TRunRawVoltage->Branch("channel_trig_settings3", &channel_trig_settings3);
	TRunRawVoltage->Branch("ioff", &ioff);
//	TRunRawVoltage->Branch("trace_0", &trace_0);
//	TRunRawVoltage->Branch("trace_1", &trace_1);
//	TRunRawVoltage->Branch("trace_2", &trace_2);
//	TRunRawVoltage->Branch("trace_3", &trace_3);
	TRunRawVoltage->Branch("trace_ch", &trace_ch);

	this->trunvoltage = TRunRawVoltage;

	return TRunRawVoltage;
}

//void TRunRawVoltage::ClearVectors()
//{
////	event_id.clear();
//	du_id.clear();
//	du_seconds.clear();
//	du_nanoseconds.clear();
////	trigger_position.clear();
//	trigger_flag.clear();
//	atm_temperature.clear();
//	atm_pressure.clear();
//	atm_humidity.clear();
//	du_acceleration.clear();
////	acceleration_x.clear();
////	acceleration_y.clear();
////	acceleration_z.clear();
//	battery_level.clear();
//	// ToDo: Is this the same as event_version for the whole event?
////	firmware_version.clear();
////	adc_sampling_frequency.clear();
////	adc_sampling_resolution.clear();
////	adc_input_channels.clear();
////	adc_enabled_channels.clear();
////	adc_samples_count_total.clear();
//	adc_samples_count_channel.clear();
////	adc_samples_count_channel0.clear();
////	adc_samples_count_channel1.clear();
////	adc_samples_count_channel2.clear();
////	adc_samples_count_channel3.clear();
//	trigger_pattern.clear();
//	trigger_rate.clear();
//	clock_tick.clear();
//	clock_ticks_per_second.clear();
//	gps_offset.clear();
//	gps_leap_second.clear();
//	gps_status.clear();
//	gps_alarms.clear();
//	gps_warnings.clear();
//	gps_time.clear();
//	gps_long.clear();
//	gps_lat.clear();
//	gps_alt.clear();
//	gps_temp.clear();
////	pos_x.clear();
////	pos_y.clear();
////	pos_z.clear();
////	digi_ctrl.clear();
////	digi_prepost_trig_windows.clear();
////	channel_properties0.clear();
////	channel_properties1.clear();
////	channel_properties2.clear();
////	channel_properties3.clear();
////	channel_trig_settings0.clear();
////	channel_trig_settings1.clear();
////	channel_trig_settings2.clear();
////	channel_trig_settings3.clear();
//	// ToDo: What is it?
//	ioff.clear();
//	trace_0.clear();
//	trace_1.clear();
//	trace_2.clear();
//	trace_3.clear();
//}

//! Initialises the TTree metadata fields
void TRunRawVoltage::InitialiseMetadata()
{
	this->creation_datetime = (new TDatime())->Convert(true);

	this->trunvoltage->GetUserInfo()->Add(new TNamed("type", this->type));
	this->trunvoltage->GetUserInfo()->Add(new TNamed("comment", this->comment));
	this->trunvoltage->GetUserInfo()->Add(new TParameter<int>("creation_datetime", this->creation_datetime));
	this->trunvoltage->GetUserInfo()->Add(new TNamed("modification_history", this->modification_history));
	this->trunvoltage->GetUserInfo()->Add(new TParameter<int>("source_datetime", this->source_datetime));
	this->trunvoltage->GetUserInfo()->Add(new TNamed("modification_software", this->modification_software));
	this->trunvoltage->GetUserInfo()->Add(new TNamed("modification_software_version", this->modification_software_version));
	this->trunvoltage->GetUserInfo()->Add(new TParameter<int>("analysis_level", this->analysis_level));
}

//! Change the name of the file in which the TTree is stored
void TRunRawVoltage::ChangeFileName(string new_file_name, bool write_tree)
{
	// Get the current tree name
	auto tree_name = string(TRunRawVoltage->GetName());

	// If writing requested, write the tree down in the file
	if(write_tree)
		TRunRawVoltage->Write("", TObject::kWriteDelete);

	// Close the old TFile
	auto old_file = TRunRawVoltage->GetCurrentFile();
	auto old_file_name = old_file->GetName();
	delete TRunRawVoltage;
	old_file->Close();

	// Rename the file in the filesystem
	filesystem::rename(filesystem::path(old_file_name), filesystem::path(new_file_name));

	// Open the new file
	auto new_file = new TFile(new_file_name.c_str(), "update");

	// Set the TTree to the one from the renamed file
	TRunRawVoltage = (TTree*)new_file->Get(tree_name.c_str());
}
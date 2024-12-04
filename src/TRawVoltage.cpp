//
// Created by lewhoo on 17/02/2022.
//
// The main class for holding the Voltage traces and some other data relative to them

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include "TDatime.h"
#include "TNamed.h"
#include "TParameter.h"
#include "TFile.h"
#include "TMath.h"
#include "inc/TRawVoltage.h"

using namespace std;
using namespace ROOT;

//! General constructor
TRawVoltage::TRawVoltage(TFile *out_file)
{
	// Initialize the TTree
	CreateTree();

	// Set the output file before filling, if provided
	if (out_file != NULL)
	{
		this->trawvoltage->SetDirectory(out_file);
	}

	// Initialise metadata
	InitialiseMetadata();
}

//! Constructor computing values from tadc
TRawVoltage::TRawVoltage(TADC *adc, bool is_fv2, TFile *out_file) : TRawVoltage(out_file)
{
	ComputeFromADC(adc, is_fv2);
}

//! Compute values from tadc
void TRawVoltage::ComputeFromADC(TADC *adc, bool is_fv2)
{
	auto tadc = adc->tadc;

	// Exclude these branches from copying, for the values need to be modified
	vector<string> excluded_branches = {"gps_long", "gps_lat", "gps_alt", "gps_temp", "trace_0", "trace_1", "trace_2", "trace_3", "trace_ch", "battery_level", "fpga_temp", "adc_temp", "atm_temperature", "atm_pressure", "atm_humidity", "gain_correction_ch", "du_acceleration"};

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
		ADCs2Real(adc, is_fv2);
//		CalculateT0s(adc);
		trawvoltage->Fill();
	}
	trawvoltage->BuildIndex("run_number", "event_number");
}


void TRawVoltage::ADCs2Real(TADC *adc, bool is_fv2)
{
	// Clear the traces vectors
//	trace_0.clear();
//	trace_1.clear();
//	trace_2.clear();
//	trace_3.clear();
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

void TRawVoltage::TraceADC2Voltage(int du_num, TADC *adc)
{
	// Probably in the future adc2voltageconst will be replaced in the transform by some array_x/y/z[du_id], or corresponding function in some non-linear case
	// Also, at the moment I assume trace_0/1/2 are x/y/z - this may also change in the future
	// The conversion factor is just taken from the information for XiHu data, that "For currently ADC, the differential input voltage range is 1.8V (Vpp), that is -0.9V to 0.9V corresponding to ADC value -8192 to 8192"
	auto adc2voltageconst = glob_adc2voltageconst;
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

void TRawVoltage::GPSADC2Real(int du_num, TADC *adc)
{
		gps_long.push_back(TMath::RadToDeg()*(*(double*)&adc->gps_long[du_num]));
		gps_lat.push_back(TMath::RadToDeg()*(*(double*)&adc->gps_lat[du_num]));
		gps_alt.push_back(*(double*)&adc->gps_alt[du_num]);
		gps_temp.push_back(*(float*)&adc->gps_temp[du_num]);
}

void TRawVoltage::GPSADC2Real_fv2(int du_num, TADC *adc)
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

void TRawVoltage::BatteryADC2Voltage(int du_num, TADC *adc)
{
		battery_level.push_back(adc->battery_level[du_num] / ((18 * 4096) / (2.5*(18+91))));
}

//void TRawVoltage::CalculateT0s(TADC *adc)
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
//pair<unsigned int, unsigned int> TRawVoltage::CalculateT0(unsigned int seconds, unsigned int nanoseconds, unsigned int trigger_pos_ns)
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

TTree *TRawVoltage::CreateTree()
{
	trawvoltage = new TTree("trawvoltage", "Event with Voltage traces and other information");

	// Create Branches for all stored data
	trawvoltage->Branch("run_number", &run_number, "run_number/i");
	trawvoltage->Branch("event_number", &event_number, "event_number/i");
	trawvoltage->Branch("event_size", &event_size, "event_size/i");
//	trawvoltage->Branch("t3_number", &t3_number, "t3_number/i");
	trawvoltage->Branch("first_du", &first_du, "first_du/i");
	trawvoltage->Branch("time_seconds", &time_seconds, "time_seconds/i");
	trawvoltage->Branch("time_nanoseconds", &time_nanoseconds, "time_nanoseconds/i");
//	trawvoltage->Branch("event_type", &event_type, "event_type/i");
//	trawvoltage->Branch("event_version", &event_version, "event_version/i");
	trawvoltage->Branch("du_count", &du_count, "du_count/i");
	// Vector branches
//	trawvoltage->Branch("event_id", &event_id);
	trawvoltage->Branch("du_id", &du_id);
	trawvoltage->Branch("du_seconds", &du_seconds);
	trawvoltage->Branch("du_nanoseconds", &du_nanoseconds);
//	trawvoltage->Branch("du_t0_seconds", &du_t0_seconds);
//	trawvoltage->Branch("du_t0_nanoseconds", &du_t0_nanoseconds);
	trawvoltage->Branch("trigger_position", &trigger_position);
	trawvoltage->Branch("trigger_flag", &trigger_flag);

	trawvoltage->Branch("pps_id", &pps_id);
	trawvoltage->Branch("fpga_temp", &fpga_temp);
	trawvoltage->Branch("adc_temp", &adc_temp);

	trawvoltage->Branch("atm_temperature", &atm_temperature);
	trawvoltage->Branch("atm_pressure", &atm_pressure);
	trawvoltage->Branch("atm_humidity", &atm_humidity);
	trawvoltage->Branch("du_acceleration", &du_acceleration);
//	trawvoltage->Branch("acceleration_x", &acceleration_x);
//	trawvoltage->Branch("acceleration_y", &acceleration_y);
//	trawvoltage->Branch("acceleration_z", &acceleration_z);
	trawvoltage->Branch("battery_level", &battery_level);
	trawvoltage->Branch("adc_samples_count_channel", &adc_samples_count_channel);
	trawvoltage->Branch("gain_correction_ch", &gain_correction_ch);
//	trawvoltage->Branch("firmware_version", &firmware_version);
//	trawvoltage->Branch("adc_sampling_frequency", &adc_sampling_frequency);
//	trawvoltage->Branch("adc_sampling_resolution", &adc_sampling_resolution);
//	trawvoltage->Branch("adc_input_channels", &adc_input_channels);
//	trawvoltage->Branch("adc_enabled_channels", &adc_enabled_channels);
//	trawvoltage->Branch("adc_samples_count_total", &adc_samples_count_total);
//	trawvoltage->Branch("adc_samples_count_channel0", &adc_samples_count_channel0);
//	trawvoltage->Branch("adc_samples_count_channel1", &adc_samples_count_channel1);
//	trawvoltage->Branch("adc_samples_count_channel2", &adc_samples_count_channel2);
//	trawvoltage->Branch("adc_samples_count_channel3", &adc_samples_count_channel3);
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
//	trawvoltage->Branch("pos_x", &pos_x);
//	trawvoltage->Branch("pos_y", &pos_y);
//	trawvoltage->Branch("pos_z", &pos_z);
//	trawvoltage->Branch("digi_ctrl", &digi_ctrl);
//	trawvoltage->Branch("digi_prepost_trig_windows", &digi_prepost_trig_windows);
//	trawvoltage->Branch("channel_properties0", &channel_properties0);
//	trawvoltage->Branch("channel_properties1", &channel_properties1);
//	trawvoltage->Branch("channel_properties2", &channel_properties2);
//	trawvoltage->Branch("channel_properties3", &channel_properties3);
//	trawvoltage->Branch("channel_trig_settings0", &channel_trig_settings0);
//	trawvoltage->Branch("channel_trig_settings1", &channel_trig_settings1);
//	trawvoltage->Branch("channel_trig_settings2", &channel_trig_settings2);
//	trawvoltage->Branch("channel_trig_settings3", &channel_trig_settings3);
	trawvoltage->Branch("ioff", &ioff);
//	trawvoltage->Branch("trace_0", &trace_0);
//	trawvoltage->Branch("trace_1", &trace_1);
//	trawvoltage->Branch("trace_2", &trace_2);
//	trawvoltage->Branch("trace_3", &trace_3);
	trawvoltage->Branch("trace_ch", &trace_ch);

	this->trawvoltage = trawvoltage;

	return trawvoltage;
}

//void TRawVoltage::ClearVectors()
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
void TRawVoltage::InitialiseMetadata()
{
	this->creation_datetime = (new TDatime())->Convert(true);

	this->trawvoltage->GetUserInfo()->Add(new TNamed("type", this->type));
	this->trawvoltage->GetUserInfo()->Add(new TNamed("comment", this->comment));
	this->trawvoltage->GetUserInfo()->Add(new TParameter<int>("creation_datetime", this->creation_datetime));
	this->trawvoltage->GetUserInfo()->Add(new TNamed("modification_history", this->modification_history));
	this->trawvoltage->GetUserInfo()->Add(new TParameter<int>("source_datetime", this->source_datetime));
	this->trawvoltage->GetUserInfo()->Add(new TNamed("modification_software", this->modification_software));
	this->trawvoltage->GetUserInfo()->Add(new TNamed("modification_software_version", this->modification_software_version));
	this->trawvoltage->GetUserInfo()->Add(new TParameter<int>("analysis_level", this->analysis_level));
}

//! Change the name of the file in which the TTree is stored
void TRawVoltage::ChangeFileName(string new_file_name, bool write_tree)
{
	// Get the current tree name
	auto tree_name = string(trawvoltage->GetName());

	// If writing requested, write the tree down in the file
	if(write_tree)
		trawvoltage->Write("", TObject::kWriteDelete);

	// Close the old TFile
	auto old_file = trawvoltage->GetCurrentFile();
	auto old_file_name = old_file->GetName();
	delete trawvoltage;
	old_file->Close();

	// Rename the file in the filesystem
	filesystem::rename(filesystem::path(old_file_name), filesystem::path(new_file_name));

	// Open the new file
	auto new_file = new TFile(new_file_name.c_str(), "update");

	// Set the TTree to the one from the renamed file
	trawvoltage = (TTree*)new_file->Get(tree_name.c_str());
}
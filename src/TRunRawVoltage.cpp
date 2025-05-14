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

#include <TLeaf.h>
#include <TLeafD.h>

#include "TFile.h"

using namespace std;
using namespace ROOT;

float glob_adc2voltageconst = 0.9 / 8192 * 1e6;

//! General constructor
TRunRawVoltage::TRunRawVoltage(TFile *out_file)
{
	// Initialize the TTree
	CreateTree();

	// Set the output file before filling, if provided
	if (out_file != NULL)
	{
		this->trunrawvoltage->SetDirectory(out_file);
	}
}

//! Constructor computing values from tadc
TRunRawVoltage::TRunRawVoltage(TADC &adc, bool is_fv2, TFile *out_file) : TRunRawVoltage(out_file)
{
	// Initialise metadata
	InitialiseMetadata();

	ComputeFromADC(adc, is_fv2);
}

//! Compute values from tadc
void TRunRawVoltage::ComputeFromADC(TADC &adc, bool is_fv2=false)
{
	auto tadc = adc.tadc;

	// Exclude these branches from copying, for the values need to be modified or copied by value
	vector<string> excluded_branches = {"integration_time_ch", "tper_ch", "tprev_ch", "ncmax_ch", "tcmax_ch", "ncmin_ch"};

	// *** Transform the tadc events into TRunRawVoltage events ***

	// Loop through TRunRawVoltage branches and set their addresses to tadc branches
	for (auto br : TRangeDynCast<TBranch>(trunrawvoltage->GetListOfBranches() ))
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

	// Copy from the first event
	tadc->GetEntry(0);
	// Copy the variables that may differ by type
	for (size_t i=0; i<adc.trace_ch.size(); ++i)
	{
		if (is_fv2)
		{
			integration_time_ch.emplace_back(adc.integration_time_ch_fv2[i].begin(), adc.integration_time_ch_fv2[i].end());
			tper_ch.emplace_back(adc.tper_ch_fv2[i].begin(), adc.tper_ch_fv2[i].end());
			tprev_ch.emplace_back(adc.tprev_ch_fv2[i].begin(), adc.tprev_ch_fv2[i].end());
			ncmax_ch.emplace_back(adc.ncmax_ch_fv2[i].begin(), adc.ncmax_ch_fv2[i].end());
			tcmax_ch.emplace_back(adc.tcmax_ch_fv2[i].begin(), adc.tcmax_ch_fv2[i].end());
			ncmin_ch.emplace_back(adc.ncmin_ch_fv2[i].begin(), adc.ncmin_ch_fv2[i].end());
		}
		else
		{
			integration_time_ch.emplace_back(adc.integration_time_ch[i].begin(), adc.integration_time_ch[i].end());
			tper_ch.emplace_back(adc.tper_ch[i].begin(), adc.tper_ch[i].end());
			tprev_ch.emplace_back(adc.tprev_ch[i].begin(), adc.tprev_ch[i].end());
			ncmax_ch.emplace_back(adc.ncmax_ch[i].begin(), adc.ncmax_ch[i].end());
			tcmax_ch.emplace_back(adc.tcmax_ch[i].begin(), adc.tcmax_ch[i].end());
			ncmin_ch.emplace_back(adc.ncmin_ch[i].begin(), adc.ncmin_ch[i].end());
		}

		// The same ADC conversion for each unit, now hardcoded in GtoT
		adc_conversion.push_back(glob_adc2voltageconst);

		trace_length.push_back(adc.trace_ch[i][0].size());
	}

	trunrawvoltage->Fill();

	trunrawvoltage->BuildIndex("run_number");
}

TTree *TRunRawVoltage::CreateTree()
{
	trunrawvoltage = new TTree("trunrawvoltage", "Event with Raw Voltage run information");

	// Create Branches for all stored data
	trunrawvoltage->Branch("run_number", &run_number, "run_number/i");
	trunrawvoltage->Branch("trigger_position", &trigger_position);
	trunrawvoltage->Branch("firmware_version", &firmware_version);
	trunrawvoltage->Branch("adc_sampling_frequency", &adc_sampling_frequency);
	trunrawvoltage->Branch("adc_sampling_resolution", &adc_sampling_resolution);
	trunrawvoltage->Branch("adc_input_channels_ch", &adc_input_channels_ch);
	trunrawvoltage->Branch("adc_enabled_channels_ch", &adc_enabled_channels_ch);
	trunrawvoltage->Branch("pre_coincidence_window_ch", &pre_coincidence_window_ch);
	trunrawvoltage->Branch("post_coincidence_window_ch", &post_coincidence_window_ch);
	trunrawvoltage->Branch("gain_correction_ch", &gain_correction_ch);
	trunrawvoltage->Branch("integration_time_ch", &integration_time_ch);
	trunrawvoltage->Branch("offset_correction_ch", &offset_correction_ch);
	trunrawvoltage->Branch("base_maximum_ch", &base_maximum_ch);
	trunrawvoltage->Branch("base_minimum_ch", &base_minimum_ch);
	trunrawvoltage->Branch("signal_threshold_ch", &signal_threshold_ch);
	trunrawvoltage->Branch("noise_threshold_ch", &noise_threshold_ch);
	trunrawvoltage->Branch("tper_ch", &tper_ch);
	trunrawvoltage->Branch("tprev_ch", &tprev_ch);
	trunrawvoltage->Branch("ncmax_ch", &ncmax_ch);
	trunrawvoltage->Branch("tcmax_ch", &tcmax_ch);
	trunrawvoltage->Branch("qmax_ch", &qmax_ch);
	trunrawvoltage->Branch("ncmin_ch", &ncmin_ch);
	trunrawvoltage->Branch("qmin_ch", &qmin_ch);
	trunrawvoltage->Branch("notch_filters_no_ch", &notch_filters_no_ch);
	trunrawvoltage->Branch("trace_length", &trace_length);
	trunrawvoltage->Branch("adc_conversion", &adc_conversion);

	this->trunrawvoltage = trunrawvoltage;

	return trunrawvoltage;
}

//! Initialises the TTree metadata fields
void TRunRawVoltage::InitialiseMetadata()
{
	this->creation_datetime = TDatime().Convert(true);

	this->trunrawvoltage->GetUserInfo()->Add(new TNamed("type", this->type));
	this->trunrawvoltage->GetUserInfo()->Add(new TNamed("comment", this->comment));
	this->trunrawvoltage->GetUserInfo()->Add(new TParameter<int>("creation_datetime", this->creation_datetime));
	this->trunrawvoltage->GetUserInfo()->Add(new TNamed("modification_history", this->modification_history));
	this->trunrawvoltage->GetUserInfo()->Add(new TParameter<int>("source_datetime", this->source_datetime));
	this->trunrawvoltage->GetUserInfo()->Add(new TNamed("modification_software", this->modification_software));
	this->trunrawvoltage->GetUserInfo()->Add(new TNamed("modification_software_version", this->modification_software_version));
	this->trunrawvoltage->GetUserInfo()->Add(new TParameter<int>("analysis_level", this->analysis_level));
}

//! Change the name of the file in which the TTree is stored
void TRunRawVoltage::ChangeFileName(string new_file_name, bool write_tree)
{
	// Get the current tree name
	auto tree_name = string(trunrawvoltage->GetName());

	// If writing requested, write the tree down in the file
	if(write_tree)
		trunrawvoltage->Write("", TObject::kWriteDelete);

	// Close the old TFile
	auto old_file = trunrawvoltage->GetCurrentFile();
	auto old_file_name = old_file->GetName();
	delete trunrawvoltage;
	old_file->Close();

	// Rename the file in the filesystem
	filesystem::rename(filesystem::path(old_file_name), filesystem::path(new_file_name));

	// Open the new file
	auto new_file = new TFile(new_file_name.c_str(), "update");

	// Set the TTree to the one from the renamed file
	trunrawvoltage = (TTree*)new_file->Get(tree_name.c_str());
}
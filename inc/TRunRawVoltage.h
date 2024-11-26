//
// Created by lewhoo on 19/01/2022.
//
// The main class for holding the voltage related information about the run

#ifndef GTOT_TRUNRAWVOLTAGE_H
#define GTOT_TRUNRAWVOLTAGE_H

#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <filesystem>
#include "scope.h"
#include "Traces.h"
#include "Traces_fv2.h"
#include "TTree.h"
#include "TTimeStamp.h"
#include "TFile.h"
#include "gtot.h"
#include "TADC.h"

using namespace std;

class TRunRawVoltage
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:

	// *** Metadata ***

	//! Tree type
	string type = "TRunRawVoltage";
	//! Comment - if needed, added by user
	string comment = "";
	//! TTree creation date/time in UTC - a naive time, without timezone set
	int creation_datetime;
	//! Modification history - JSON
	string modification_history = "";

	//! Unix creation datetime of the source tree; 0 s means no source
	int source_datetime = 0;
	//! The tool used to generate this tree's values from another tree
	string modification_software = "";
	//! The version of the tool used to generate this tree's values from another tree
	string modification_software_version = "";
	//! The analysis level of this tree
	int analysis_level = 0;

	// *** Entry values ***

	// Better have local storage for all the variables if one day we don't init from the structure
	// Common for the whole event
	//! Run number
	unsigned int run_number;
	//! Trigger position in the trace (trigger start = nanoseconds - 2*sample number)
	vector<unsigned short> trigger_position;
	//! Firmware version
	vector<unsigned short> firmware_version;
	//! ADC sampling frequency in MHz
	vector<unsigned short> adc_sampling_frequency;
	//! ADC sampling resolution in bits
	vector<unsigned short> adc_sampling_resolution;

	//! ADC input channels for fv2: 0: ADC1, 1: ADC2, 2:ADC3, 3:ADC4, 4:FILTERED ADC1, 5:FILTERED ADC2, 6:FILTERED ADC3, 7:FILTERED ADC4, 15: off
	vector<vector<unsigned char>> adc_input_channels_ch;

	//! ADC enabled channels - LOWEST 4 BITS STATE WHICH CHANNEL IS READ OUT
	vector<vector<bool>> adc_enabled_channels_ch;

	//! Digitizer window parameters - describe Pre Coincidence, Coincidence and Post Coincidence readout windows (Digitizer window parameters in the manual). The unit is samples (to get time, multiply by 2).
	vector<vector<unsigned short>> pre_coincidence_window_ch;
	vector<vector<unsigned short>> post_coincidence_window_ch;

	// Channel property parameters
	vector<vector<unsigned short>> gain_correction_ch;
	vector<vector<unsigned short>> integration_time_ch;
	vector<vector<unsigned char>> offset_correction_ch;
	vector<vector<unsigned short>> base_maximum_ch;
	vector<vector<unsigned short>> base_minimum_ch;

	// Channel trigger parameters
	vector<vector<unsigned short>> signal_threshold_ch;
	vector<vector<unsigned short>> noise_threshold_ch;
	// The unit is samples (to get time, multiply by 2)
	vector<vector<unsigned short>> tper_ch;
	vector<vector<unsigned short>> tprev_ch;
	vector<vector<unsigned short>> ncmax_ch;
	vector<vector<unsigned short>> tcmax_ch;
	vector<vector<unsigned short>> qmax_ch;
	vector<vector<unsigned short>> ncmin_ch;
	vector<vector<unsigned short>> qmin_ch;

	// Added on 29.02.2024

	//! Samples in baseline subtraction - same as integration time in fv1, but now a short not a char
	// vector<vector<unsigned short>> integration_time_ch_fv2;

//	vector<vector<unsigned short>> tper_ch_fv2;
//	vector<vector<unsigned short>> tprev_ch_fv2;
//	vector<vector<unsigned short>> ncmax_ch_fv2;
//	vector<vector<unsigned short>> tcmax_ch_fv2;
//	vector<vector<unsigned short>> ncmin_ch_fv2;

	vector<vector<unsigned char>> notch_filters_no_ch;

	//! Trace length
	vector<unsigned int> trace_length;

	//! ADC to voltage conversion factor
	vector<float> adc_conversion;

	//! The TTree for holding the data
	TTree *trunrawvoltage;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! The default constructor
	TRunRawVoltage();

	//! Constructor with outfile
	TRunRawVoltage(TFile *out_file=NULL);

	//! Constructor computing values from TADC
	TRunRawVoltage(TADC *adc, bool is_fv2=false, TFile *out_file=NULL);

	//! Compute values from tadc
	void ComputeFromADC(TADC *adc, bool is_fv2);

	//! Change the name of the file in which the TTree is stored
	void ChangeFileName(string new_file_name, bool write=true);


private:
	//! Clear the vectors for another fill
	void ClearVectors();

	//! Initialises the TTree metadata fields
	void InitialiseMetadata();

};


#endif //GTOT_TRUNRAWVOLTAGE_H

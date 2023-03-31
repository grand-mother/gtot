//
// Created by lewhoo on 10/03/2022.
//
// The main class for storing the Run information

#ifndef GTOT_TRUN_H
#define GTOT_TRUN_H

#include <string>
#include <vector>
#include <iostream>
#include "TTree.h"
#include "gp13.h"
#include "TADC.h"

using namespace std;

class TRun
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:

	// *** Metadata ***

	//! Tree type
	string type = "TRun";
	//! Comment - if needed, added by user
	string comment = "";
	//! TTree creation date/time in UTC - a naive time, without timezone set
	int creation_datetime;
	//! Modification history - JSON
	string modification_history = "";

	// *** Entry values ***

	// Better have local storage for all the variables if one day we don't init from the structure

	// These are coming from the hardware
	//! Run number
	unsigned int run_number;
	//! Run mode - calibration/test/physics. ToDo: should get enum description for that, but I don't think it exists at the moment
	unsigned int run_mode;
	//! File serial number for this specific Run in hardware generated binary files -- makes no sense in "Run" TTree, as it is not run specific, but file specific. Could move into an event, but I guess we can ignore
//	unsigned int file_serial_number;
	//! Run's first event
	unsigned int first_event;
	//! First event time
	unsigned int first_event_time;
	//! Run's last event
	unsigned int last_event;
	//! Last event time
	unsigned int last_event_time;

	// These are not from the hardware
	//! Data source: detector, simulation, other
	string data_source="detector";
	//! Data generator: gtot (in this case)
	string data_generator="gtot";
	//! Generator version: gtot version (in this case)
	string data_generator_version="0.24";
	//! Trigger type 0x1000 10 s trigger and 0x8000 random trigger, else shower
	unsigned int event_type=0;
	//! Event format version of the DAQ
	unsigned int event_version=0;
	//! Site name
	string site="not specified";
	//! Site layout
	string site_layout="";
	//! Origin of the coordinate system used for the array
	float origin_geoid[3]={0,0,0};
	//! Detector unit (antenna) ID
	vector<int> du_id;
	//! Detector unit (antenna) (lat,lon,alt) position
	vector<vector<float>> du_geoid;
	//! Detector unit (antenna) (x,y,z) position in site's referential
	vector<vector<float>> du_xyz;
	//! Detector unit type
	vector<string> du_type;
	//! Detector unit (antenna) angular tilt
	vector<vector<float>> du_tilt;
	//! Angular tilt of the ground at the antenna
	vector<vector<float>> du_ground_tilt;
	//! Detector unit (antenna) nut ID
	vector<int> du_nut;
	//! Detector unit (antenna) FrontEnd Board ID
	vector<int> du_feb;
	//! Time bin size in ns (computed as 1/adc_sampling_frequency)
	vector<float> t_bin_size;


	//! The TTree for holding the data
	TTree *trun;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! Set the object variables from pointer intialised by Charles' functions from Traces.c
	int SetValuesFromPointers(int *pheader, string file_format="");

	//! Set the values of t_bin_size vector from adc_sampling_frequency vector in the TADC tree
	void SetTBinSizeFromADCSamplingFrequency(TADC *t);

	//! The default constructor
	TRun();

private:
	//! Initialises the TTree metadata fields
	void InitialiseMetadata();

};


#endif //GTOT_TRUN_H

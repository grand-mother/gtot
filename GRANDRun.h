//
// Created by lewhoo on 10/03/2022.
//
// The main class for storing the Run information

#ifndef GTOT_GRANDRUN_H
#define GTOT_GRANDRUN_H

#include <string>
#include "TTree.h"

using namespace std;

class GRANDRun
{
	// I assume we don't need getters and setters for these properties, because they are simple values
public:
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
	string data_generator_version="0.1.0";
	//! Site name
	string site="not specified";
	//! Site longitude
	float site_long=0;
	//! Site latitude
	float site_lat=0;
	//! Origin of the coordinate system used for the array
	float origin_geoid[3]={0,0,0};

	//! The TTree for holding the data
	TTree *trun;

	//! Create the TTree and its branches
	TTree *CreateTree();

	//! Set the object variables from pointer intialised by Charles' functions from Traces.c
	int SetValuesFromPointers(int *pheader);

	//! The default constructor
	GRANDRun();

};


#endif //GTOT_GRANDRUN_H

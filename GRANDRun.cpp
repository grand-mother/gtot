//
// Created by lewhoo on 10/03/2022.
//
// The main class for storing the Run information

#include "GRANDRun.h"
#include "Traces.h"

GRANDRun::GRANDRun()
{
	CreateTree();
}

TTree *GRANDRun::CreateTree()
{
	trun = new TTree("trun", "Run information");

	// Create Branches for all stored data
	// Int branches
	trun->Branch("run_number", &run_number, "run_number/i");
	trun->Branch("run_mode", &run_mode, "run_mode/i");
//	trun->Branch("file_serial_number", &file_serial_number, "file_serial_number/i");
	trun->Branch("first_event", &first_event, "first_event/i");
	trun->Branch("first_event_time", &first_event_time, "first_event_time/i");
	trun->Branch("last_event", &last_event, "last_event/i");
	trun->Branch("last_event_time", &last_event_time, "last_event_time/i");

	trun->Branch("data_source", &data_source);
	trun->Branch("data_generator", &data_generator);
	trun->Branch("data_generator_version", &data_generator_version);
	trun->Branch("site", &site);
	trun->Branch("site_long", &site_long, "site_long/F");
	trun->Branch("site_lat", &site_lat, "site_lat/F");
	trun->Branch("origin_geoid", &origin_geoid, "origin_geoid[3]/D");

	return trun;
}

int GRANDRun::SetValuesFromPointers(int *pheader)
{
	run_number = pheader[FILE_HDR_RUNNR];
	run_mode = pheader[FILE_HDR_RUN_MODE];
//	file_serial_number = pheader[FILE_HDR_SERIAL];
	first_event = pheader[FILE_HDR_FIRST_EVENT];
	first_event_time = pheader[FILE_HDR_FIRST_EVENT_SEC];
	last_event = pheader[FILE_HDR_LAST_EVENT];
	last_event_time = pheader[FILE_HDR_LAST_EVENT_SEC];

	return 0;
}
//
// Created by lewhoo on 10/03/2022.
//
// The main class for storing the Run information

#include "inc/TRun.h"
#include "inc/Traces.h"
#include "TDatime.h"
#include "TNamed.h"
#include "TParameter.h"

TRun::TRun()
{
	CreateTree();
}

TTree *TRun::CreateTree()
{
	trun = new TTree("trun", "Run information");

	// Initialise metadata
	InitialiseMetadata();

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
	trun->Branch("event_type", &event_type, "event_type/i");
	trun->Branch("event_version", &event_version, "event_version/i");
	trun->Branch("site", &site);
	trun->Branch("site_layout", &site_layout);
//	trun->Branch("site_long", &site_long, "site_long/F");
//	trun->Branch("site_lat", &site_lat, "site_lat/F");
	trun->Branch("origin_geoid", &origin_geoid, "origin_geoid[3]/F");
	trun->Branch("du_id", &du_id);
	trun->Branch("du_geoid", &du_geoid);
	trun->Branch("du_xyz", &du_xyz);
	trun->Branch("du_type", &du_type);
	trun->Branch("du_tilt", &du_tilt);
	trun->Branch("du_ground_tilt", &du_ground_tilt);
	trun->Branch("du_nut", &du_nut);
	trun->Branch("du_feb", &du_feb);
	trun->Branch("t_bin_size", &t_bin_size);

	return trun;
}

int TRun::SetValuesFromPointers(int *pheader, string file_format)
{
	run_number = pheader[FILE_HDR_RUNNR];
	run_mode = pheader[FILE_HDR_RUN_MODE];
//	file_serial_number = pheader[FILE_HDR_SERIAL];
	first_event = pheader[FILE_HDR_FIRST_EVENT];
	first_event_time = pheader[FILE_HDR_FIRST_EVENT_SEC];
	last_event = pheader[FILE_HDR_LAST_EVENT];
	last_event_time = pheader[FILE_HDR_LAST_EVENT_SEC];

	// GP13 case
	if(strstr(file_format.c_str(), "GP13") || strstr(file_format.c_str(), "gp13") || strstr(file_format.c_str(), "Gp13"))
	{
		site = "GP13";
		site_layout = "GP13";

		this->origin_geoid[0] = gp13::origin_geoid[0];
		this->origin_geoid[1] = gp13::origin_geoid[1];
		this->origin_geoid[2] = gp13::origin_geoid[2];
		this->du_id = gp13::du_id;
		this->du_geoid = gp13::du_geoid;
		this->du_xyz = gp13::du_xyz;
		this->du_type = gp13::du_type;
		this->du_tilt = gp13::du_tilt;
		this->du_ground_tilt = gp13::du_ground_tilt;
		this->du_nut = gp13::du_nut;
		this->du_feb = gp13::du_feb;
	}

	return 0;
}

// ToDo: BUG!!! It fills only the first event DUs, while it should all the DUs, like in TRun::SetValuesFromPointers
void TRun::SetTBinSizeFromADCSamplingFrequency(TADC *t)
{
	for(int i=0; i<t->adc_sampling_frequency.size(); ++i)
	{
		this->t_bin_size.push_back(1./t->adc_sampling_frequency[i]*1000);
	}
}

//! Initialises the TTree metadata fields
void TRun::InitialiseMetadata()
{
	this->creation_datetime = (new TDatime())->Convert(true);

	this->trun->GetUserInfo()->Add(new TNamed("type", this->type));
	this->trun->GetUserInfo()->Add(new TNamed("comment", this->comment));
	this->trun->GetUserInfo()->Add(new TParameter<int>("creation_datetime", this->creation_datetime));
	this->trun->GetUserInfo()->Add(new TNamed("modification_history", this->modification_history));
}

//
// Created by lewhoo on 10/03/2022.
//
// The main class for storing the Run information

#include "inc/TRun.h"
#include "inc/Traces.h"
#include "TDatime.h"
#include "TNamed.h"
#include "TParameter.h"
#include "TFile.h"
#include "TSystem.h"

TRun::TRun()
{
	CreateTree();

	first_event = numeric_limits<unsigned int>::max();
	last_event = 0;
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

int TRun::SetValuesFromPointers(int *pheader, string file_format, bool init_first_last_event, string site_name)
{
	using namespace fv1;
	run_number = pheader[FILE_HDR_RUNNR];
	run_mode = pheader[FILE_HDR_RUN_MODE];
//	file_serial_number = pheader[FILE_HDR_SERIAL];
	// Init first last event from the header if requested
	// (not the case for GP13 now, where these values are rubbish)
	if(init_first_last_event)
	{
		first_event = pheader[FILE_HDR_FIRST_EVENT];
		first_event_time = pheader[FILE_HDR_FIRST_EVENT_SEC];
		last_event = pheader[FILE_HDR_LAST_EVENT];
		last_event_time = pheader[FILE_HDR_LAST_EVENT_SEC];
	}

	// GP13 case
	auto tfile_format = TString(file_format);
	tfile_format.ToLower();
	if (tfile_format.Index("gp13")>=0)
	// if(strstr(.ToLower().Data(), "GP13") || strstr(file_format.c_str(), "gp13") || strstr(file_format.c_str(), "Gp13"))
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

	// If site name was provided and conforms to the standards, set site and site_layout to it (regardless of the file format)
	auto tsite_name = TString(site_name);
	tsite_name.ToLower();
	if (tsite_name=="gaa" || (tsite_name.Length()>3 && tsite_name(0,2)=="gp"))
	{
		site = site_name;
		site_layout = site_name;
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
	this->creation_datetime = TDatime().Convert(true);

	this->trun->GetUserInfo()->Add(new TNamed("type", this->type));
	this->trun->GetUserInfo()->Add(new TNamed("comment", this->comment));
	this->trun->GetUserInfo()->Add(new TParameter<int>("creation_datetime", this->creation_datetime));
	this->trun->GetUserInfo()->Add(new TNamed("modification_history", this->modification_history));
	this->trun->GetUserInfo()->Add(new TParameter<int>("source_datetime", this->source_datetime));
	this->trun->GetUserInfo()->Add(new TNamed("modification_software", this->modification_software));
	this->trun->GetUserInfo()->Add(new TNamed("modification_software_version", this->modification_software_version));
	this->trun->GetUserInfo()->Add(new TParameter<int>("analysis_level", this->analysis_level));
}

//! Update the first/last event info, fill, write and close
void TRun::UpdateAndWrite(unsigned int first_event, unsigned int first_event_time, unsigned int last_event, unsigned int last_event_time, TTree *old_tree)
{
	// If the given first event is smaller than the one in the tree, update the one in the tree
	if(first_event<this->first_event)
	{
		this->first_event = first_event;
		this->first_event_time = first_event_time;
	}

	// If the given last event is greater than the one in the tree, update the one in the tree
	if(last_event>this->last_event)
	{
		this->last_event = last_event;
		this->last_event_time = last_event_time;
	}

	// Fill the tree and write it
	this->trun->Fill();
	this->trun->BuildIndex("run_number");

	this->trun->Write("", TObject::kWriteDelete);

	// If there was an old TTree from which this one was cloned, delete it before writing this one down
	if(old_tree!=NULL)
	{
		auto old_name = old_tree->GetCurrentFile()->GetName();
		gSystem->Unlink(old_name);
		gSystem->Rename(this->trun->GetCurrentFile()->GetName(), old_name);
	}
}
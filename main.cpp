#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <filesystem>
#include "inc/Traces1.h"
#include "inc/Traces1_fv2.h"
#include "TFile.h"
#include "TInterpreter.h"
#include "TNamed.h"

#include "inc/TRun.h"
#include "inc/TADC.h"
#include "inc/TRawVoltage.h"
#include "inc/gtot.h"

bool overbose = false;

// Prints help in the command line
void print_help()
{
	cout << "GRAND TO TTrees (gtot), program for converting GRAND experiment binary files in raw hardware format to CERN ROOT TTrees.\nMade by Lech Wiktor Piotrowski <lech.piotrowski@fuw.edu.pl>, University of Warsaw, Poland." << endl << endl;
	cout << "Usage: gtot <options> <hardware_file_names>" << endl;
	cout << "Options:" << endl;
	cout << "\t-h, --help\t\t\t\tdisplay this help" << endl;
	cout << "\t-g1, --gp13v1\t\t\t\tthe input file is a GP13 v1 file" << endl;
	cout << "\t-e, --event_number_assignment\t\t\t\tdon't read event numbers from raw files, assign them consecutively" << endl;
	cout << "\t-f2, --firmware_v2\t\t\tthe input file is a firmware v2 file" << endl;
	cout << "\t-os, --old_style_output\t\t\tall trees will be in the same file, no directory will be created" << endl;
	cout << "\t-o, --output_filename <filename>\tname of the file to which store the TTrees" << endl;
	cout << "\t-i, --input_filename <filename>\t\tname of the single file to analyse, regardless of extension. No other files accepted." << endl;
	cout << "\t-v, --verbose\t\t\t\tswitch on verbose output" << endl;
}

// Array holding filenames to analyse
TObjArray filenames;
string output_filename="";
bool gp13v1 = false;
bool is_fv2 = false;
string file_format = "";
bool infile_forced = false;
bool old_style_output = false;
bool cons_ev_num = false;

// Analyse the command line parameters
void analyse_command_line_params(int argc, char **argv)
{
	if(argc<2)
	{
		cout << "Please provide the name of the binary file in raw hardware format!\nAdd -h to display help." << endl;
		exit(EXIT_FAILURE);
	}

	for(int i=1; i<argc; ++i)
	{
		// Help requested
		if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-h")) || strstr(argv[i], "--help"))
		{
			print_help();
			exit(0);
		}
			// Forced input file
		else if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-i")) || strstr(argv[i], "--input_filename"))
		{
			infile_forced = true;
			filenames.Clear();
			filenames.Add((TObject *) (new TString(argv[i + 1])));
		} else if ((strlen(argv[i]) == 2 && strstr(argv[i], "-o")) || strstr(argv[i], "--output_filename"))
		{
			output_filename = argv[i + 1];
			++i;
		} else if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-g1")) || strstr(argv[i], "--gp13v1"))
		{
			cout << "Switching to GP13 v1 mode" << endl;
			gp13v1 = true;
			file_format = "gp13v1";
		} else if ((strlen(argv[i]) == 2 && strstr(argv[i], "-e")) || strstr(argv[i], "--event_number_assignment"))
		{
			cout << "Assigning event numbers consecutively" << endl;
			cons_ev_num = true;
		} else if ((strlen(argv[i]) == 2 && strstr(argv[i], "-v")) || strstr(argv[i], "--verbose"))
		{
			cout << "Enabled verbose output" << endl;
			overbose = true;
		} else if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-f2")) || strstr(argv[i], "--firmware_v2"))
		{
			cout << "Switching to firmware v2 mode" << endl;
			is_fv2 = true;
//			file_format = "gp13v1";
		}
		else if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-os")) || strstr(argv[i], "--old_style_output"))
		{
			cout << "Storing trees in the old way" << endl;
			old_style_output = true;
		}

			// File to analyse
		else
		{
			auto fn_ext = filesystem::path(argv[i]).extension();
			if ((fn_ext == ".dat" || strstr(fn_ext.c_str(), ".f0") || fn_ext == ".bin" || count(argv[i], argv[i] + strlen(argv[i]), '_') >= 5) && !infile_forced)
//		else if((strstr(argv[i],".dat") || strstr(argv[i],".f0")) && !infile_forced)
			{
				filenames.Add((TObject *) (new TString(argv[i])));
				cout << "Added " << ((TString *) (filenames.Last()))->Data() << endl;
			}
		}
		/*
		else
		{
			cout << argv[i] << " count " << count(argv[i], argv[i]+sizeof(argv[i]), '_') << " " << strlen(argv[i]) << endl;
		}
		 */

	}
};

vector<string> parse_file_name(string &filename)
{
	string rest;
	// Operate on just the file name without extension
	istringstream stream(filesystem::path(filename).stem());
	string chunk;
	vector<string> parts;

	// Splitting the filename by "_"
	while (std::getline(stream, chunk, '_'))
	{
		parts.push_back(chunk);
	}

	// The last part should contain all the remaining filename ("extra" comment) with _ replaced by -
	for (size_t i = 5; i < parts.size(); ++i)
	{
		if (!rest.empty())
		{
			rest += "-";
		}
		rest += parts[i];
	}

	// In GP13 the "extra" may contain serial number, which needs to be removed
	if(parts[0]=="gp13" || parts[0]=="GP13")
	{
		auto last_part = rest.substr(rest.find_last_of('-')+1);
		// If the last part after "-" is a number, remove it
		if(last_part.find_first_not_of("0123456789") == std::string::npos)
			rest = rest.substr(0, rest.find_last_of('-'));
	}

	parts[5] = rest;

	return parts;
};

void rename_event_files(string out_dir, string date, string time, int first_event, int last_event)
{
	string tadc_name = string("adc_")+date+string("_")+time+string("_")+to_string(first_event)+"-"+to_string(last_event)+"_L1_0000.root";
	cout << "Renaming adc.root to " << tadc_name << endl;
	filesystem::rename(filesystem::path(out_dir+"/adc.root"), filesystem::path(out_dir+"/"+tadc_name));

		string trawvoltage_name = string("rawvoltage_")+date+string("_")+time+string("_")+to_string(first_event)+"-"+to_string(last_event)+"_L1_0000.root";
	cout << "Renaming rawvoltage.root to " << trawvoltage_name << endl;

	filesystem::rename(filesystem::path(out_dir+"/rawvoltage.root"), filesystem::path(out_dir+"/"+trawvoltage_name));
}

std::ostream *pvout;

int main(int argc, char **argv)
{
	analyse_command_line_params(argc, argv);

	// Create verbose output stream if requested
	std::ofstream dev_null("/dev/null");
	pvout = (std::ostream*)&dev_null;
	if(overbose)
		pvout = &cout;
	std::ostream &vout = *pvout;

	// No proper files given
	if(filenames.GetEntries()==0)
	{
		cout << "No files with .dat/.bin extension provided! If you want to use other extension for a single file, use option -i." << endl;
		return -1;
	}
	// No sense to use output filename if more than one input file was provided
	if(!(output_filename=="") && (filenames.GetEntries()>1 || !old_style_output))
	{
		cout << "Specifying output file name makes no sense in case of multiply input files and/or without --old_style_output flag" << endl;
		return -1;
	}

	// Initialise pointers to functions depending on the input file type
	int (*grand_read_file_header_ptr)(FILE *fp, int **pfilehdr);
	int (*grand_read_event_ptr)(FILE *fp, unsigned short **pevent, const char *file_format);
	if(is_fv2)
	{
		grand_read_file_header_ptr = fv2::grand_read_file_header;
		grand_read_event_ptr = fv2::grand_read_event;
	}
	else
	{
		grand_read_file_header_ptr = fv1::grand_read_file_header;
		grand_read_event_ptr = fv1::grand_read_event;
	}

	TRun *run = NULL;
	TFile *trun_file = NULL;

	// Shows if a run tree file was already created in this directory before this run of gtot
	bool run_file_exists = false;

	// First and last events to be stored in the run file
	unsigned int first_event, last_event=0, first_event_time, last_event_time;
	if(cons_ev_num) first_event=0;
	else first_event=numeric_limits<unsigned int>::max();

	vector<string> fn_tokens;
	string dir_name;

	// Event counter
	int event_counter = 0;

	// Loop through the input files
	for(int j=0; j<filenames.GetEntries(); ++j)
	{
		string filename  = ((TString *) (filenames.At(j)))->Data();

		fn_tokens = parse_file_name(filename);

		// Check if the directory for the analysed files already exists
		vector<filesystem::path> directories;
		// Get all directories with proper names
		for (const auto& entry : filesystem::directory_iterator("."))
		{
			auto dn = entry.path().string();
			// Append if it is a directory and its name contain proper parts
			if (entry.is_directory() && dn.find(string("exp_")+fn_tokens.at(0))!=string::npos && dn.find(fn_tokens.at(3)+string("_")+fn_tokens.at(4)+string("_")+fn_tokens.at(5)+string("_0000"))!=string::npos)
			{
				directories.push_back(entry.path().filename());
			}
		}

		// If the directory or analysed files already exists, use it
		if(!directories.empty())
		{
			// Sort the directories alphabetically
			sort(directories.begin(), directories.end());
			// Use the first directory
			dir_name = directories[0];
		}
		// Build directory name
		else
					dir_name = string("exp_")+fn_tokens.at(0)+string("_")+fn_tokens.at(1)+string("_")+fn_tokens.at(2)+string("_")+fn_tokens.at(3)+string("_")+fn_tokens.at(4)+string("_")+fn_tokens.at(5)+string("_0000");

		// Assume the file to analyse is the last parameter
		cout << "\n*** Analysing " << filename << endl;

		//! File reading
		FILE *fp;
		int i, ich, ib;

		fp = fopen(filename.c_str(), "r");
		if (fp == NULL) printf("Error opening file %s\n", filename.c_str());

		int *filehdr = NULL;
		unsigned short *event = NULL;

		int ret_val = 0;

		// If directory does not exist, create it
		if(!filesystem::is_directory(filesystem::path(dir_name)))
		{
			cout << "\n*** Creating directory " << dir_name << endl;
			filesystem::create_directory(filesystem::path(dir_name));
		}
		filesystem::current_path(dir_name);

		// If no output was provided, replace the file extension with ".root"
		if (output_filename == "" || filenames.GetEntries()>1)
		{
			output_filename = filename;
			// Leaving the old extension inside the new filename, as it contains an ordinal number
			auto fn_ext = filesystem::path(filename).extension();
			if( fn_ext == ".dat" || fn_ext.string().find(".f0") != string::npos)
				output_filename = output_filename.replace(output_filename.find_last_of("."), 1, "_");
//			output_filename = output_filename.replace(output_filename.find_last_of("."), output_filename.size() - output_filename.find_last_of("."), ".root");

			// The standard case
			if( fn_ext == ".bin")
			{
				output_filename = filesystem::path(filename).replace_extension(".root");
			}
				// For others, add .root
			else
				output_filename+=".root";

			cout << "Storing output in " << output_filename << endl;
		}
		// Created on the disk after the first successfull event read
		TFile *tadc_file, *trawvoltage_file;

		// Just for debugging for now
//	tree_file->SetCompressionLevel(0);

		// Need to define a dictionary so that ROOT can handle vector<vector<unsigned short> >
//	gInterpreter->GenerateDictionary("vector<vector<unsigned short> >", "vector");
//	gInterpreter->GenerateDictionary("vector<vector<short> >", "vector");
//	gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");

		TFile *old_trun_file = NULL;
		TTree *old_trun = NULL;

		// For event numbers read from the raw file, zero the event_counter
		if(!cons_ev_num) event_counter=0;

		if(!old_style_output)
		{
			auto run_num = fn_tokens.at(3).substr(3, 10);
			string trun_name = string("run_") + run_num + "_L0_0000.root";

			// If the run file already exist and we are reading the first file, clone it for updating later
			if (filesystem::is_regular_file(filesystem::path(trun_name)) && j==0)
			{
				run_file_exists = true;
				cerr << "reading old tree" << endl;
				old_trun_file = new TFile(string(trun_name).c_str(), "read");
				old_trun = (TTree *) old_trun_file->Get("trun");
				old_trun->GetEntry(0);

				// Update first/last events from the previous run
				first_event = old_trun->GetLeaf("first_event")->GetValue();
				first_event_time = old_trun->GetLeaf("first_event_time")->GetValue();
				last_event = old_trun->GetLeaf("last_event")->GetValue();
				last_event_time = old_trun->GetLeaf("last_event_time")->GetValue();
			}

			// If it is not the first file, and consecutive event numbering requested, update event numbers from the last file
			if(j!=0)
			{
				first_event = event_counter;
			}
		}

		// The whole Run class, init only for the first file or for the old output style
		if(j==0 || old_style_output)
		{
			run = new TRun();
		}

		// The ADC event class
		auto ADC = new TADC(is_fv2);


		// Read the file from the detector and fill in the TTrees with the read-out data
//		if (grand_read_file_header(fp, &filehdr))
		if (grand_read_file_header_ptr(fp, &filehdr))
		{
			// Read the file header and fill the Run TTree
			// For every file in the old style output, and only for the first file in the new style output
			if(old_style_output || j==0)
				run->SetValuesFromPointers(filehdr, file_format);

			// For GP13 move in the file
			if (gp13v1) fseek(fp, 256, 0);
			// Loop-read the events
			while (grand_read_event_ptr(fp, &event, file_format.c_str()) > 0)
			{
				if(is_fv2)
					ret_val = ADC->SetValuesFromPointers_fv2(event, file_format);
				else
					ret_val = ADC->SetValuesFromPointers(event, file_format);

				if (ret_val < 0) break;

				// For GP13v1 overwrite some values
				if (gp13v1 || cons_ev_num)
				{
					// The run number is not specified in GPv13 event, only in header of the file
					if(gp13v1) ADC->run_number = run->run_number;
					// The event number - in raw data it is separate for each DU and thus not unique, while it needs to be unique in the TTree
					ADC->event_number = event_counter;
				}
				ADC->tadc->Fill();

				// In principle these event numbers/times could be taken from the file header, but the thing below is work around potential bugs in firmware

				// If the event_number lower than the first event, update the first event
				if(ADC->event_number<first_event && !cons_ev_num)
				{
					first_event = ADC->event_number;
					first_event_time = ADC->time_seconds;
				}

				// If the event_number higher than the last event, update the last event
				if(ADC->event_number>last_event && !cons_ev_num)
				{
					last_event = ADC->event_number;
					last_event_time = ADC->time_seconds;
				}

				// For the first event, create the TFile, fill some trun values read by tadc
				if (event_counter == first_event)
				{
					// Create the TFiles in the output directory
					if(!old_style_output)
					{
						// Create the run file only when analysing the first file
						if(j==0)
						{
							auto run_num = fn_tokens[3].substr(3, 10);
							string trun_name = string("run_") + run_num + "_L0_0000.root";
							trun_file = new TFile(trun_name.c_str(), "recreate");
						}

						tadc_file = new TFile(string("adc.root").c_str(), "recreate");
						trawvoltage_file = new TFile(string("rawvoltage.root").c_str(), "recreate");
					}
					// For old style output, store trees in just one file in the current directory
					else
					{
						trun_file = new TFile(output_filename.c_str(), "recreate");
						tadc_file = trun_file;
						trawvoltage_file = trun_file;
					}

					// Move TTrees to it
					run->trun->SetDirectory(trun_file);
					ADC->tadc->SetDirectory(tadc_file);

					// Fill run either for first event and each file in the old case, or first event, first file in the new case
					if(old_style_output || j==0)
					{

						// Event type and version
						run->event_version = ADC->event_version;
						run->event_type = ADC->event_type;
						// The time bin size
						// ToDo: BUG!!! It fills only the first event DUs, while it should all the DUs, like in TRun::SetValuesFromPointers
						run->SetTBinSizeFromADCSamplingFrequency(ADC);

						// Add a comment
						TString comment =
								TString("Generated by GtoT ") + run->data_generator_version + TString(" from file ") +
								filename;
						((TNamed *) run->trun->GetUserInfo()->FindObject("comment"))->SetTitle(comment);

						// Clone the old run tree if exists
						// Basically, I should not clone only, but reinit the whole run object from the old tree, but that requires adding capability of initialising classes from trees in gtot, just for that..
						if (!old_style_output && run_file_exists)
						{
							run->trun = old_trun->CloneTree(0);
						}
					}

					// Fill and write the Run TTree
					// For each file in the old case only (for the new case, fill & write only for the last file)
					if(old_style_output)
					{
						run->trun->Fill();
						run->trun->BuildIndex("run_number");
						run->trun->Write("", TObject::kWriteDelete);
					}
				}

				cout << "\rFilled event " << event_counter;
				event_counter++;
			}

			// In case of no events, just exit
			if (event_counter == 0)
			{
				cout << "No events found in the hardware file. The ROOT file will not be written." << endl;
				exit(-1);
			}

		}
		if (fp != NULL) fclose(fp); // close the file


		// In case of a bad return above, just close the file and exit
		if (ret_val < 0)
		{
			if(!old_style_output)
			{
				trun_file->cd();
				last_event=event_counter-1;
				run->UpdateAndWrite(first_event, first_event_time, last_event, last_event_time);
				trun_file->Close();
				tadc_file->Close();
				trawvoltage_file->Close();
				rename_event_files(".", fn_tokens[1], fn_tokens[2], first_event, last_event);
				if(cons_ev_num) first_event = event_counter;
			}
			else
				trun_file->Close();
//			tree_file->Close();
			return ret_val;
		}

		// Build the run_number/event_number index for ADC TTree
		ADC->tadc->BuildIndex("run_number", "event_number");
		// Add the Run TTree as a friend
		ADC->tadc->AddFriend(run->trun);

		// Write out the ADC TTree to the file
		cout << "Writing TADC tree" << endl;
		tadc_file->cd();
		ADC->tadc->Write("", TObject::kWriteDelete);

		// Create the TRawVoltage TTree
		auto voltage = new TRawVoltage(ADC, is_fv2, trawvoltage_file);
		// Add the Run TTree as a friend
		voltage->trawvoltage->AddFriend(run->trun);
		// Write out the Voltage TTree to the file
		cout << "Writing TRawVoltage tree" << endl;
		trawvoltage_file->cd();
		voltage->trawvoltage->Write("", TObject::kWriteDelete);

		// Close TFiles with the TTrees for the old style output
		// the trun_file contains also the event trees
		if(old_style_output)
			trun_file->Close();
		else
		{
			tadc_file->Close();
			trawvoltage_file->Close();
			last_event = event_counter-1;
			rename_event_files(".", fn_tokens[1], fn_tokens[2], first_event, last_event);
		}

		filesystem::current_path("../");
	}

	// For the new style output fill, write and close the run after all the files
	if(!old_style_output)
	{
		run->trun->SetDirectory(trun_file);
		trun_file->cd();
		last_event=event_counter-1;
		run->UpdateAndWrite(first_event, first_event_time, last_event, last_event_time);
	}

	cout << "\nFinished, quitting" << endl;

    return 0;
}

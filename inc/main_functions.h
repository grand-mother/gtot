//
// Created by lewhoo on 15/10/24.
//

#ifndef GTOT_MAIN_FUNCTIONS_H
#define GTOT_MAIN_FUNCTIONS_H

#include <iostream>
#include <string.h>
#include "TRun.h"
#include "TADC.h"
#include "TRawVoltage.h"
#include "inc/Traces.h"
#include "Traces1.h"
#include "inc/scope.h"

// Prints help in the command line
void print_help()
{
	cout << "GRAND TO TTrees (gtot), program for converting GRAND experiment binary files in raw hardware format to CERN ROOT TTrees.\nMade by Lech Wiktor Piotrowski <lech.piotrowski@fuw.edu.pl>, University of Warsaw, Poland." << endl << endl;
	cout << "Usage: gtot <options> <hardware_file_names>" << endl;
	cout << "Options:" << endl;
	cout << "\t-h, --help\t\t\t\tdisplay this help" << endl;
	cout << "\t-g1, --gp13v1\t\t\t\tthe input file is a GP13 v1 file UD and/or MD" << endl;
	cout << "\t-gc, --gp13v1cd\t\t\t\tthe input file is a GP13 v1 file CD" << endl;
	cout << "\t-e, --event_number_assignment\t\tdon't read event numbers from raw files, assign them consecutively" << endl;
	cout << "\t-rn, --run_number_assignment\t\tdon't read raw numbers from raw files, but from the file name" << endl;
	cout << "\t-f2, --firmware_v2\t\t\tthe input file is a firmware v2 file" << endl;
	cout << "\t-os, --old_style_output\t\t\tall trees will be in the same file, no directory will be created" << endl;
	cout << "\t-o, --output_filename <filename>\tname of the file to which store the TTrees" << endl;
	cout << "\t-od, --output_directory <directory>\tname of the directory in which to store the files" << endl;
	cout << "\t-i, --input_filename <filename>\t\tname of the single file to analyse, regardless of extension. No other files accepted." << endl;
	cout << "\t-ow, --overwrite \toverwrites ROOT files if they exist (by default will quit if they do)" << endl;
	cout << "\t-v, --verbose\t\t\t\tswitch on verbose output" << endl;
}

// Analyse the command line parameters
//void analyse_command_line_params(int argc, char **argv, TObjArray &filenames, string &output_filename, string &file_format, bool &infile_forced, bool &gp13v1, bool &cons_ev_num, bool &overbose, bool &is_fv2, bool &old_style_output, bool &file_run_num)
void analyse_command_line_params(int argc, char **argv, vector<string> &filenames, string &output_filename, string &file_format, bool &infile_forced, bool &gp13v1, bool &cons_ev_num, bool &overbose, bool &is_fv2, bool &old_style_output, bool &file_run_num, bool &gp13v1cd, string &output_directory, bool &overwrite_files)
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
			filenames.clear();
//			filenames.Add((TObject *) (new TString(argv[i + 1])));
			filenames.push_back(string(argv[i + 1]));
		} else if ((strlen(argv[i]) == 2 && strstr(argv[i], "-o")) || strstr(argv[i], "--output_filename"))
		{
			output_filename = argv[i + 1];
			++i;
		} else if ((strlen(argv[i]) == 3 && strstr(argv[i], "-od")) || strstr(argv[i], "--output_directory"))
		{
			// The output filename and output directory are mutually exclusive (because I'm lazy - please give a file's full path with -o)
			if(output_filename!="")
			{
				cout << "Output directory and output filename options are mutually exclusive. Please provide the output file with a requested path.";
				exit(0);
			}
			output_directory = argv[i + 1];
            // Create the output directory
            filesystem::create_directory(filesystem::path(output_directory));
			++i;
		} else if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-g1")) || strstr(argv[i], "--gp13v1"))
		{
			cout << "Switching to GP13 v1 UD/MD mode" << endl;
			gp13v1 = true;
			file_format = "gp13v1";
		}
		else if ((strlen(argv[i]) >= 2 && strstr(argv[i], "-gc")) || strstr(argv[i], "--gp13v1cd"))
		{
			cout << "Switching to GP13 v1 CD mode" << endl;
			gp13v1 = true;
			gp13v1cd = true;
			file_format = "gp13v1cd";
		} else if ((strlen(argv[i]) == 2 && strstr(argv[i], "-e")) || strstr(argv[i], "--event_number_assignment"))
		{
			cout << "Assigning event numbers consecutively" << endl;
			cons_ev_num = true;
		} else if ((strlen(argv[i]) == 3 && strstr(argv[i], "-rn")) || strstr(argv[i], "--run_number_assignment"))
		{
			cout << "Reading run numbers from files names" << endl;
			file_run_num = true;
		}
		else if ((strlen(argv[i]) == 2 && strstr(argv[i], "-v")) || strstr(argv[i], "--verbose"))
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
		else if ((strlen(argv[i]) == 3 && strstr(argv[i], "-ow")) || strstr(argv[i], "--overwrite"))
		{
			cout << "Overwriting ROOT files if they exist" << endl;
			overwrite_files = true;
		}

		// File to analyse
		else
		{
			auto file_path = filesystem::path(argv[i]);
			auto fn_ext = file_path.extension();
			if ((fn_ext == ".dat" || strstr(fn_ext.c_str(), ".f0") || fn_ext == ".bin" || count(argv[i], argv[i] + strlen(argv[i]), '_') >= 5) && !infile_forced)
			{
				// Check if file exists
				if(!filesystem::exists(file_path))
				{
					cout << "File " << file_path.string() << " does not exist. Exiting." << endl;
					exit(-1);
				}
				filenames.push_back(argv[i]);
				cout << "Added " << filenames.back() << endl;
			}
		}
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

	// If less than 6 parts, it is an old file or has a malformed name. Setting artificial tokens
	if (parts.size()<6)
	{
		cout << "The file name " << filename << " is not in the expected format. Setting artificial parts of the file name." << endl;

		parts.resize(6);
		// If the first part is not a date, set a dummy date
		if(parts[1].size()!=8 || parts[1].find_first_not_of("0123456789") != std::string::npos)
			parts[1] = "19700101";

		// If the second part is not a time, set a dummy date
		if(parts[2].size()!=8 || parts[2].find_first_not_of("0123456789") != std::string::npos)
			parts[2] = "000000";

		// If the third part does not contain a run number, set a dummy run number
		if (parts[3].size()>3)
		{
			if (parts[3].substr(0,3)=="RUN" && parts[3].substr(4).find_first_not_of("0123456789") != std::string::npos)
				parts[3] = "RUN000";
		}
		else parts[3] = "RUN000";

		// If the fourth part does not contain a valid acquisition type, make it "XD"
		if(parts[4]!="CD" && parts[4]!="MD" && parts[4]!="UD")
			parts[4] = "XD";

		return parts;
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

	// In GP13/GP80 the "extra" may contain serial number, which needs to be removed
	if(parts[0]=="gp13" || parts[0]=="GP13" || parts[0]=="GP80")
	{
		auto last_part = rest.substr(rest.find_last_of('-')+1);
		// If the last part after "-" is a number, remove it
		if(last_part.find_first_not_of("0123456789") == std::string::npos)
			rest = rest.substr(0, rest.find_last_of('-'));
	}

	parts[5] = rest;

	return parts;
};


//! Computes content of TRawVoltage from TADC, renames files for both, writes and closes
void finalise_and_close_event_trees(TADC &ADC, TRawVoltage *voltage, TRun *run, vector<string> fn_tokens, unsigned int first_event, unsigned int last_event, bool is_fv2=false, bool old_style_output=false)
{
	if(!old_style_output)
	{
		// Build the run_number/event_number index for ADC TTree
		ADC.tadc->BuildIndex("run_number", "event_number");

		// Write out the ADC TTree to the file
		cout << "Writing TADC tree" << endl;
		ADC.tadc->GetCurrentFile()->cd();
		ADC.tadc->Write("", TObject::kWriteDelete);

		// Create the TRawVoltage TTree and copying the valid TADC contents (need to do that before the change of TADC file name)
		voltage->ComputeFromADC(ADC, is_fv2);

		string tadc_name =
				string("adc_") + fn_tokens[1] + string("_") + fn_tokens[2] + string("_") + to_string(first_event) +
				"-" + to_string(last_event) + "_L1_0000.root";
		cout << "Renaming tadc.root to " << tadc_name << endl;
		ADC.ChangeFileName(tadc_name);
		// Add the Run TTree as a friend
		ADC.tadc->AddFriend(run->trun);
		ADC.tadc->Write("", TObject::kWriteDelete);

		// Build the run_number/event_number index for RawVoltage TTree
		voltage->trawvoltage->BuildIndex("run_number", "event_number");
		// Write out the Voltage TTree to the file
		cout << "Writing TRawVoltage tree" << endl;
		voltage->trawvoltage->GetCurrentFile()->cd();
		voltage->trawvoltage->Write("", TObject::kWriteDelete);
		string trawvoltage_name = string("rawvoltage_") + fn_tokens[1] + string("_") + fn_tokens[2] + string("_") +
								  to_string(first_event) + "-" + to_string(last_event) + "_L1_0000.root";
		cout << "Renaming rawvoltage.root to " << trawvoltage_name << endl;
		voltage->ChangeFileName(trawvoltage_name);
		// Add the Run TTree as a friend
//		voltage->trawvoltage->AddFriend(run->trun);
		voltage->trawvoltage->Write("", TObject::kWriteDelete);

		delete ADC.tadc->GetCurrentFile();
		delete voltage->trawvoltage->GetCurrentFile();

		// ADC->tadc->GetCurrentFile()->Close();
		// voltage->trawvoltage->GetCurrentFile()->Close();
	}
	else
	{
		// Build the run_number/event_number index for ADC TTree
		ADC.tadc->BuildIndex("run_number", "event_number");

		// Write out the ADC TTree to the file
		cout << "Writing TADC tree" << endl;
		ADC.tadc->GetCurrentFile()->cd();
		ADC.tadc->Write("", TObject::kWriteDelete);

		// Create the TRawVoltage TTree and copying the valid TADC contents (need to do that before the change of TADC file name)
		voltage->ComputeFromADC(ADC, is_fv2);
		// Build the run_number/event_number index for RawVoltage TTree
		voltage->trawvoltage->BuildIndex("run_number", "event_number");
		// Write out the Voltage TTree to the file
		cout << "Writing TRawVoltage tree" << endl;
		voltage->trawvoltage->GetCurrentFile()->cd();
		voltage->trawvoltage->Write("", TObject::kWriteDelete);

		// Close TFiles with the TTrees for the old style output
		// the trun_file contains also the event trees
		delete run->trun->GetCurrentFile();
		// run->trun->GetCurrentFile()->Close();
	}

}

// Group filenames that would go in the same directory together as vector<string> and add these groups to file_groups
void group_files_and_directories(vector<string> filenames, vector<vector<string>> &file_groups, string output_directory=".")
{
	vector<string> directories;

	// Sort the filenames
	sort(filenames.begin(), filenames.end());

	// Add existing directories to the directories list
	for (const auto &entry: filesystem::directory_iterator(output_directory))
	{
		auto dn = entry.path().filename().string();
		// If the entry is a directory and starts with "exp_" add it to the list of directories
		if (entry.is_directory() && dn.find(string("exp_")) == 0)
			directories.push_back(dn);
	}

	for (auto filename : filenames)
	{
		auto fn_tokens = parse_file_name(filename);

		int i=0;

		bool dir_found = false;

		for (auto directory : directories)
		{
			// If file matches an existing directory in the list and a file group for this directory has been created
			if (directory.find(string("exp_") + fn_tokens.at(0)) == 0 &&
					directory.find(fn_tokens.at(3) + string("_") + fn_tokens.at(4) + string("_") + fn_tokens.at(5) +
						string("_0000")) != string::npos && file_groups.size()>i)
			{
				file_groups[i].push_back(filename);
				dir_found = true;
				break;
			}
			++i;
		}

		// The directory for the file does not exist yet
		if(!dir_found)
		{
			auto dir_name = string("exp_") + fn_tokens.at(0) + string("_") + fn_tokens.at(1) + string("_") +
							fn_tokens.at(2) + string("_") + fn_tokens.at(3) + string("_") + fn_tokens.at(4) +
							string("_") + fn_tokens.at(5) + string("_0000");

			directories.push_back(dir_name);
			file_groups.emplace_back();
			file_groups.back().push_back(filename);
		}
	}
}

// Reorders GPX CD files in memory, so that one event is continuation of DUs from this event, and reassigns file descriptor fp with the in-memory file descriptor pointing to the reordered buffer
char *read_order_file_in_memory(FILE **fp)
{
	map<unsigned int, vector<unsigned short*>> fmap;

	unsigned short *event = nullptr;
	long last_good_pos = 0;

	// Read the file event by event (which means DU by DU)
	while(fv1::grand_read_event(*fp, &event, "gp13v1")>0)
	{
		unsigned int ev_num = *(unsigned int*)(event+6);

		// Push the DU into the vector of this event
		fmap[ev_num].push_back(event);
		last_good_pos = ftell(*fp);
	}

	// Allocate the buffer for the whole file - header
//	unsigned int whole_size = ftell(*fp)-256;
	unsigned int whole_size = last_good_pos-256;
	auto out_buf = (char*)malloc(whole_size);

	long last_pos = 0;

	// Loop through the map of events - the map is ordered by events
	for (auto & [key, val] : fmap)
	{
		int du_num = 0;
		// Loop through the vector of DUs in the event
		for (auto const ev : val)
		{
			// Copy the DU into the output buffer
			unsigned int size = *(unsigned int*)(ev);
			memcpy((void *) (out_buf + last_pos), (void *) ev, size);

			// For the first DU copy event with a header and modify it
			if(du_num==0)
			{
				// Change the event size
				*((unsigned int*)(out_buf + last_pos))*=val.size();
				// Change message type to a number of DUs in the event
				// ToDo: this is replacing a valid value is just a temporary solution!
				*((unsigned int*)(out_buf + last_pos+4))=val.size();
			}

			// Change atmospheric humidity to du_id
			// ToDo: this is replacing a valid value is just a temporary solution!
			auto du_id = *((unsigned int*)(out_buf + last_pos + 8));
			*(unsigned int*)(out_buf + last_pos+(fv1::EVT_ATM_HUM+12)*2)=du_id;

			last_pos+=size;
			du_num++;

			free(ev);
		}
	}

	fclose(*fp);

	// Use fmemopen to create a FILE stream from memory
	*fp = fmemopen((void *)out_buf, whole_size, "r");
	if (!*fp)
	{
		perror("Failed to open memory stream");
		exit(EXIT_FAILURE);
	}

	return out_buf;

}

void is_file_opened(TFile *f)
{
	if (!f->IsOpen())
	{
		cout << "To overwrite files automatically, use the --overwrite option. Exiting." << endl;
		exit(1);
	}
}

#endif //GTOT_MAIN_FUNCTIONS_H

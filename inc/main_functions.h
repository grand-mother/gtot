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
	cout << "\t-i, --input_filename <filename>\t\tname of the single file to analyse, regardless of extension. No other files accepted." << endl;
	cout << "\t-v, --verbose\t\t\t\tswitch on verbose output" << endl;
}

// Analyse the command line parameters
//void analyse_command_line_params(int argc, char **argv, TObjArray &filenames, string &output_filename, string &file_format, bool &infile_forced, bool &gp13v1, bool &cons_ev_num, bool &overbose, bool &is_fv2, bool &old_style_output, bool &file_run_num)
void analyse_command_line_params(int argc, char **argv, vector<string> &filenames, string &output_filename, string &file_format, bool &infile_forced, bool &gp13v1, bool &cons_ev_num, bool &overbose, bool &is_fv2, bool &old_style_output, bool &file_run_num, bool &gp13v1cd)
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

			// File to analyse
		else
		{
			auto fn_ext = filesystem::path(argv[i]).extension();
			if ((fn_ext == ".dat" || strstr(fn_ext.c_str(), ".f0") || fn_ext == ".bin" || count(argv[i], argv[i] + strlen(argv[i]), '_') >= 5) && !infile_forced)
//		else if((strstr(argv[i],".dat") || strstr(argv[i],".f0")) && !infile_forced)
			{
//				filenames.Add((TObject *) (new TString(argv[i])));
				filenames.push_back(argv[i]);
//				cout << "Added " << ((TString *) (filenames.Last()))->Data() << endl;
				cout << "Added " << filenames.back() << endl;
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


//! Computes content of TRawVoltage from TADC, renames files for both, writes and closes
void finalise_and_close_event_trees(TADC *ADC, TRawVoltage *voltage, TRun *run, vector<string> fn_tokens, unsigned int first_event, unsigned int last_event, bool is_fv2=false, bool old_style_output=false)
{
	if(!old_style_output)
	{
		// Build the run_number/event_number index for ADC TTree
		ADC->tadc->BuildIndex("run_number", "event_number");

		// Write out the ADC TTree to the file
		cout << "Writing TADC tree" << endl;
		ADC->tadc->GetCurrentFile()->cd();
		ADC->tadc->Write("", TObject::kWriteDelete);

		// Create the TRawVoltage TTree and copying the valid TADC contents (need to do that before the change of TADC file name)
		voltage->ComputeFromADC(ADC, is_fv2);

		string tadc_name =
				string("adc_") + fn_tokens[1] + string("_") + fn_tokens[2] + string("_") + to_string(first_event) +
				"-" + to_string(last_event) + "_L1_0000.root";
		cout << "Renaming tadc.root to " << tadc_name << endl;
		ADC->ChangeFileName(tadc_name);
		// Add the Run TTree as a friend
		ADC->tadc->AddFriend(run->trun);
		ADC->tadc->Write("", TObject::kWriteDelete);

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

		ADC->tadc->GetCurrentFile()->Close();
		voltage->trawvoltage->GetCurrentFile()->Close();
	}
	else
	{
		// Build the run_number/event_number index for ADC TTree
		ADC->tadc->BuildIndex("run_number", "event_number");

		// Write out the ADC TTree to the file
		cout << "Writing TADC tree" << endl;
		ADC->tadc->GetCurrentFile()->cd();
		ADC->tadc->Write("", TObject::kWriteDelete);

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
		run->trun->GetCurrentFile()->Close();
	}

}

// Group filenames that would go in the same directory together as vector<string> and add these groups to file_groups
void group_files_and_directories(vector<string> filenames, vector<vector<string>> &file_groups)
{
	vector<string> directories;

	// Sort the filenames
	sort(filenames.begin(), filenames.end());

	// Add existing directories to the directories list
	for (const auto &entry: filesystem::directory_iterator("."))
	{
		auto dn = entry.path().string();
		// If the entry is a directory and starts with "exp_" add it to the list of directories
		if (entry.is_directory() && dn.find(string("exp_")) == 2)
			directories.push_back(dn);
	}

	for (auto filename : filenames)
	{
		auto fn_tokens = parse_file_name(filename);

		int i=0;

		bool dir_found = false;

		for (auto directory : directories)
		{
			// If file matches an existing directory in the list
			if (directory.find(string("exp_") + fn_tokens.at(0)) == 0 &&
					directory.find(fn_tokens.at(3) + string("_") + fn_tokens.at(4) + string("_") + fn_tokens.at(5) +
						string("_0000")) != string::npos)
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

	unsigned short *event = NULL;
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
	for (auto const& [key, val] : fmap)
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

			last_pos+=size;
			du_num++;
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

#endif //GTOT_MAIN_FUNCTIONS_H

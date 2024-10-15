//
// Created by lewhoo on 15/10/24.
//

#ifndef GTOT_MAIN_FUNCTIONS_H
#define GTOT_MAIN_FUNCTIONS_H

#include <iostream>
#include "TRun.h"
#include "TADC.h"
#include "TRawVoltage.h"

// Prints help in the command line
void print_help()
{
	cout << "GRAND TO TTrees (gtot), program for converting GRAND experiment binary files in raw hardware format to CERN ROOT TTrees.\nMade by Lech Wiktor Piotrowski <lech.piotrowski@fuw.edu.pl>, University of Warsaw, Poland." << endl << endl;
	cout << "Usage: gtot <options> <hardware_file_names>" << endl;
	cout << "Options:" << endl;
	cout << "\t-h, --help\t\t\t\tdisplay this help" << endl;
	cout << "\t-g1, --gp13v1\t\t\t\tthe input file is a GP13 v1 file" << endl;
	cout << "\t-e, --event_number_assignment\t\t\t\tdon't read event numbers from raw files, assign them consecutively" << endl;
	cout << "\t-rn, --run_number_assignment\t\t\t\tdon't read raw numbers from raw files, but from the file name" << endl;
	cout << "\t-f2, --firmware_v2\t\t\tthe input file is a firmware v2 file" << endl;
	cout << "\t-os, --old_style_output\t\t\tall trees will be in the same file, no directory will be created" << endl;
	cout << "\t-o, --output_filename <filename>\tname of the file to which store the TTrees" << endl;
	cout << "\t-i, --input_filename <filename>\t\tname of the single file to analyse, regardless of extension. No other files accepted." << endl;
	cout << "\t-v, --verbose\t\t\t\tswitch on verbose output" << endl;
}

// Analyse the command line parameters
void analyse_command_line_params(int argc, char **argv, TObjArray &filenames, string &output_filename, string &file_format, bool &infile_forced, bool &gp13v1, bool &cons_ev_num, bool &overbose, bool &is_fv2, bool &old_style_output, bool &file_run_num)
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
		voltage->trawvoltage->AddFriend(run->trun);
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

#endif //GTOT_MAIN_FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <filesystem>
#include "inc/TRunRawVoltage.h"
#include "inc/main_functions.h"
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

// Array holding filenames to analyse
vector<string> all_filenames;
string output_filename="";
string output_directory=".";
bool gp13v1 = false;
bool gp13v1cd = false;
bool is_fv2 = false;
string file_format = "";
bool infile_forced = false;
bool old_style_output = false;
bool cons_ev_num = false;
bool file_run_num = false;
bool overwrite_files = false;

std::ostream *pvout;

int main(int argc, char **argv)
{
	analyse_command_line_params(argc, argv, all_filenames, output_filename, file_format, infile_forced, gp13v1, cons_ev_num, overbose, is_fv2, old_style_output, file_run_num, gp13v1cd, output_directory, overwrite_files);

	// Create verbose output stream if requested
	std::ofstream dev_null("/dev/null");
	pvout = (std::ostream*)&dev_null;
	if(overbose)
		pvout = &cout;
	std::ostream &vout = *pvout;

	// Files write flag depending on overwrite_files command line option
	auto write_flag = "NEW";
	if(overwrite_files)
		write_flag = "RECREATE";

	// No proper files given
	if(all_filenames.size()==0)
	{
		cout << "No files with .dat/.bin extension provided! If you want to use other extension for a single file, use option -i." << endl;
		return -1;
	}
	// No sense to use output filename if more than one input file was provided
	if(!(output_filename=="") && (all_filenames.size()>1 || !old_style_output))
	{
		cout << "Specifying output file name makes no sense in case of multiply input files and/or without --old_style_output flag" << endl;
		return -1;
	}
	// Get the current directory
	auto input_files_dir = filesystem::current_path();

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

	// Group filenames that would go in the same directory together
	vector<vector<string>> file_groups;
	group_files_and_directories(all_filenames, file_groups, output_directory);

	// Loop through groups of files that go into the same directory
	for(auto filenames : file_groups)
	{
		unique_ptr<TRun> run;
		// TRun *run = nullptr;
		unique_ptr<TRunRawVoltage> runrawvoltage;
		// TRunRawVoltage *runrawvoltage = nullptr;
		unique_ptr<TFile> trun_file_up;
		TFile *trun_file = nullptr;
		std::unique_ptr<TFile> trunrawvoltage_file_up;
		TFile *trunrawvoltage_file = nullptr;
		unique_ptr<TADC> ADC;
		// TFile *old_trun_file = NULL;
		std::unique_ptr<TFile> old_trun_file;
		// TFile* old_trun_file = nullptr;
		TTree *old_trun = nullptr;

		// Shows if a run tree file was already created in this directory before this run of gtot
		bool run_file_exists = false;

		// First and last events to be stored in the run file
		unsigned int first_event, last_event=0, first_event_time, last_event_time, first_first_event_time;
		if(cons_ev_num) first_event = 0;
		else first_event = numeric_limits<unsigned int>::max();

		// The first event among all the analysed files
		unsigned int first_first_event = first_event;

		vector<string> fn_tokens;
		string dir_name;

		// Event counter
		int event_counter = 0;

		// Loop through the input files
		for (int j = 0; j < filenames.size(); ++j)
		{
			string filename = filenames[j];

			fn_tokens = parse_file_name(filename);

			if (!old_style_output)
			{
				// Check if the directory for the analysed files already exists
				vector<filesystem::path> directories;
				// Get all directories with proper names
				for (const auto &entry: filesystem::directory_iterator(output_directory))
				{
					auto dn = entry.path().filename().string();
					// Append if it is a directory and its name contain proper parts
					if (entry.is_directory() && dn.find(string("exp_") + fn_tokens.at(0)) == 0 &&
						dn.find(fn_tokens.at(3) + string("_") + fn_tokens.at(4) + string("_") + fn_tokens.at(5) +
								string("_0000")) != string::npos)
					{
						directories.push_back(entry.path().filename());
					}
				}

				// If the directory or analysed files already exists, use it
				if (!directories.empty())
				{
					// Sort the directories alphabetically
					sort(directories.begin(), directories.end());
					// Use the first directory
					dir_name = directories[0];
				}
				// Build directory name
				else
					dir_name = string("exp_") + fn_tokens.at(0) + string("_") + fn_tokens.at(1) + string("_") +
							   fn_tokens.at(2) + string("_") + fn_tokens.at(3) + string("_") + fn_tokens.at(4) +
							   string("_") + fn_tokens.at(5) + string("_0000");

				// Add the requested output directory to dir_name
				dir_name = output_directory+"/"+dir_name;
			}

			// Assume the file to analyse is the last parameter
			cout << "\n*** Analysing " << filename << endl;

			//! File reading
			FILE *fp;
			int i, ich, ib;

			fp = fopen(filename.c_str(), "r");
			if (fp == nullptr) printf("Error opening file %s\n", filename.c_str());

			int *filehdr = nullptr;
			unsigned short *event = nullptr;

			int ret_val = 0;

			if (!old_style_output)
			{
             	// If the directory exists, but overwrite was requested, delete it
                if (filesystem::is_directory(filesystem::path(dir_name)) && overwrite_files)
                {
                    cout << "\n*** Deleting existing directory " << dir_name << endl;
                	filesystem::remove_all(filesystem::path(dir_name));
                }

				// If directory does not exist, create it
				if (!filesystem::is_directory(filesystem::path(dir_name)))
				{
					cout << "\n*** Creating directory " << dir_name << endl;
					filesystem::create_directory(filesystem::path(dir_name));
				}
				filesystem::current_path(dir_name);
			}

			// If no output was provided, replace the file extension with ".root"
			if (output_filename == "" || filenames.size() > 1)
			{
				output_filename = filename;
				// Leaving the old extension inside the new filename, as it contains an ordinal number
				auto fn_ext = filesystem::path(filename).extension();
				if (fn_ext == ".dat" || fn_ext.string().find(".f0") != string::npos)
					output_filename = output_filename.replace(output_filename.find_last_of("."), 1, "_");
//			output_filename = output_filename.replace(output_filename.find_last_of("."), output_filename.size() - output_filename.find_last_of("."), ".root");

				// The standard case
				if (fn_ext == ".bin")
				{
					output_filename = filesystem::path(filename).replace_extension(".root");
				}
					// For others, add .root
				else
					output_filename += ".root";

				cout << "Storing output in " << output_filename << endl;
			}
			// Created on the disk after the first successfull event read
			unique_ptr<TFile> tadc_file_up, trawvoltage_file_up;
			TFile *tadc_file=nullptr, *trawvoltage_file=nullptr;

			// Just for debugging for now
//	tree_file->SetCompressionLevel(0);

			// Need to define a dictionary so that ROOT can handle vector<vector<unsigned short> >
//	gInterpreter->GenerateDictionary("vector<vector<unsigned short> >", "vector");
//	gInterpreter->GenerateDictionary("vector<vector<short> >", "vector");
//	gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");

			// For event numbers read from the raw file, zero the event_counter
			if (!cons_ev_num) event_counter = 0;

			if (!old_style_output)
			{
				auto run_num = fn_tokens.at(3).substr(3, 10);
				string trun_name = string("run_") + run_num + "_L0_0000.root";
				string trunrawvoltage_name = string("runrawvoltage_") + run_num + "_L0_0000.root";

				// If the run file already exist and we are reading the first file, clone it for updating later
				if (filesystem::is_regular_file(filesystem::path(trun_name)) && j == 0 && !overwrite_files)
				{
					run_file_exists = true;
					// old_trun_file = new TFile(string(trun_name).c_str(), "update");
					old_trun_file = make_unique<TFile>(string(trun_name).c_str(), "update");
					old_trun = static_cast<TTree*>(old_trun_file->Get("trun"));
					old_trun->SetName("old_trun");
					old_trun->GetEntry(0);

					// Update first/last events from the previous run
					first_event = old_trun->GetLeaf("first_event")->GetValue();
					first_event_time = old_trun->GetLeaf("first_event_time")->GetValue();
					first_first_event = first_event;
					last_event = old_trun->GetLeaf("last_event")->GetValue();
					last_event_time = old_trun->GetLeaf("last_event_time")->GetValue();
					first_event = last_event + 1;
					event_counter = first_event;
				}

				// If it is not the first file, and consecutive event numbering requested, update event numbers from the last file
				if (j != 0 && cons_ev_num)
				{
					first_event = event_counter;
				}
			}

			// The whole Run class, init only for the first file or for the old output style
			if (j == 0 || old_style_output)
			{
				// run = new TRun();
				run = make_unique<TRun>();
				// run = run_up.get();
				if (run_file_exists)
				{
					run->trun->SetDirectory(old_trun_file.get());
				}
			}

			// The ADC event class
			// ADC = new TADC(is_fv2);
			ADC = make_unique<TADC>(is_fv2);

			// Read the file from the detector and fill in the TTrees with the read-out data
//		if (grand_read_file_header(fp, &filehdr))
			if (grand_read_file_header_ptr(fp, &filehdr))
			{
				// Read the file header and fill the Run TTree
				// For every file in the old style output, and only for the first file in the new style output
				if (old_style_output || (j == 0 && !run_file_exists))
				{
					run->SetValuesFromPointers(filehdr, file_format, !(gp13v1 || cons_ev_num) || old_style_output, fn_tokens[0]);
					if (!cons_ev_num)
					{
						first_event = run->first_event;
						last_event = run->last_event;
					}
					first_event_time = run->first_event_time;
					last_event_time = run->last_event_time;
					first_first_event = first_event;
					first_first_event_time = first_event_time;
				}

				// For GP13 move in the file
				if (gp13v1) fseek(fp, 256, 0);

				char *out_buf = nullptr;
				if(gp13v1cd) out_buf = read_order_file_in_memory(&fp);

				// Loop-read the events
				while (grand_read_event_ptr(fp, &event, file_format.c_str()) > 0)
				{
					if (is_fv2)
						ret_val = ADC->SetValuesFromPointers_fv2(event, file_format);
					else
						ret_val = ADC->SetValuesFromPointers(event, file_format);

					// If unable to read the event, break the loop and continue to the next file
					if (ret_val < 0)
					{
						free(event);
						event = nullptr;
						break;
					}

					// For GP13v1 overwrite some values
					if (gp13v1 || cons_ev_num || file_run_num)
					{
						// The run number is not specified in GPv13 event, only in header of the file
						if (file_run_num)
						{
							auto run_num = fn_tokens.at(3).substr(3, 10);
							run->run_number = stoul(run_num);
							ADC->run_number = run->run_number;
						} else if (gp13v1) ADC->run_number = run->run_number;
						// The event number - in raw data it is separate for each DU and thus not unique, while it needs to be unique in the TTree
						ADC->event_number = event_counter;
					}
					ADC->tadc->Fill();

					// In principle these event numbers/times could be taken from the file header, but the thing below is work around potential bugs in firmware

					// If the event_number lower than the first event, update the first event
					if (ADC->event_number < first_event && !cons_ev_num)
					{
						first_event = ADC->event_number;
						first_event_time = ADC->time_seconds;
					}

					// If the event_number is lower than the first first event, update the first first event
					if (ADC->event_number < first_first_event && !cons_ev_num)
					{
						first_first_event = ADC->event_number;
						if (gp13v1)
							first_first_event_time = *min_element(ADC->du_seconds.begin(), ADC->du_seconds.end());
						else first_first_event_time = ADC->time_seconds;
					}


					// If the event_number higher than the last event, update the last event
					if (ADC->event_number > last_event && !cons_ev_num)
					{
						last_event = ADC->event_number;
						if (gp13v1) last_event_time = *min_element(ADC->du_seconds.begin(), ADC->du_seconds.end());
						else last_event_time = ADC->time_seconds;
					}

					// For the first event, create the TFile, fill some trun values read by tadc
					if ((cons_ev_num && event_counter == first_event) || (!cons_ev_num && event_counter == 0) ||
						(run_file_exists && event_counter == last_event + 1))
					{
						// Create the TFiles in the output directory
						if (!old_style_output)
						{
							// Create the run file only when analysing the first file and not cloning the previous run tree
							if (j == 0)
							{
								auto run_num = fn_tokens[3].substr(3, 10);
								string trun_name = string("run_") + run_num + "_L0_0000.root";
								string trunrawvoltage_name = string("runrawvoltage_") + run_num + "_L0_0000.root";

								// If the run file already exists, give this one a temporary name
								if(run_file_exists)
									trun_name = "new_"+trun_name;

								trun_file_up = make_unique<TFile>(trun_name.c_str(), write_flag);
								trun_file = trun_file_up.get();
								// trun_file = new TFile(trun_name.c_str(), write_flag);
								is_file_opened(trun_file);
								trunrawvoltage_file_up = make_unique<TFile>(trunrawvoltage_name.c_str(), "recreate");
								trunrawvoltage_file = trunrawvoltage_file_up.get();
								// trunrawvoltage_file = new TFile(trunrawvoltage_name.c_str(), "recreate");
								is_file_opened(trunrawvoltage_file);

								// Set the time of the first event
								if (gp13v1)
									first_first_event_time = *min_element(ADC->du_seconds.begin(),
																		  ADC->du_seconds.end());
								else first_first_event_time = ADC->time_seconds;
							}

							// tadc_file = new TFile(string("adc.root").c_str(), write_flag);
							tadc_file_up = make_unique<TFile>(string("adc.root").c_str(), write_flag);
							tadc_file = tadc_file_up.get();
							is_file_opened(tadc_file);
							// trawvoltage_file = new TFile(string("rawvoltage.root").c_str(), write_flag);
							trawvoltage_file_up = make_unique<TFile>(string("rawvoltage.root").c_str(), write_flag);
							trawvoltage_file = trawvoltage_file_up.get();
							is_file_opened(trawvoltage_file);
						}
						// For old style output, store trees in just one file in the current directory
						else
						{
							// trun_file = new TFile(output_filename.c_str(), write_flag);
							trun_file_up = make_unique<TFile>(output_filename.c_str(), write_flag);
							trun_file = trun_file_up.get();
							is_file_opened(trun_file);
							tadc_file = trun_file;
							trawvoltage_file = trun_file;
							trunrawvoltage_file = trun_file;
						}

						// Move TTrees to it
						run->trun->SetDirectory(trun_file);
						ADC->tadc->SetDirectory(tadc_file);

						// Fill run either for first event and each file in the old case, or first event, first file in the new case
						if (old_style_output || j == 0)
						{

							// Event type and version
							run->event_version = ADC->event_version;
							run->event_type = ADC->event_type;
							// The time bin size
							// ToDo: BUG!!! It fills only the first event DUs, while it should all the DUs, like in TRun::SetValuesFromPointers
							run->SetTBinSizeFromADCSamplingFrequency(*ADC);

							// Add a comment
							TString comment =
									TString("Generated by GtoT ") + run->data_generator_version +
									TString(" from file ") +
									filename;
							(static_cast<TNamed *>(run->trun->GetUserInfo()->FindObject("comment"))->SetTitle(comment));

							// Clone the old run tree if exists
							// Basically, I should not clone only, but reinit the whole run object from the old tree, but that requires adding capability of initialising classes from trees in gtot, just for that..
							if (!old_style_output && run_file_exists)
							{
								old_trun->SetBranchStatus("last_event", 0);
								old_trun->SetBranchStatus("last_event_time", 0);
								run->trun = old_trun->CloneTree(0);
								run->trun->Branch("last_event", &run->last_event, "last_event/i");
								run->trun->Branch("last_event_time", &run->last_event_time, "last_event_time/i");
								run->trun->SetDirectory(trun_file);
								run->trun->SetName("trun");
							}

							// Fill the trunrawvoltage
							trunrawvoltage_file->cd();
							// runrawvoltage = new TRunRawVoltage(ADC, is_fv2, trunrawvoltage_file);
							runrawvoltage = make_unique<TRunRawVoltage>(*ADC, is_fv2, trunrawvoltage_file);
							// Write the trunrawvoltage
							runrawvoltage->trunrawvoltage->Write("", TObject::kWriteDelete);
							// delete runrawvoltage;
						}

						// Fill and write the Run TTree
						// For each file in the old case only (for the new case, fill & write only for the last file)
						if (old_style_output)
						{
							run->trun->SetDirectory(trun_file);
							run->trun->Fill();
							run->trun->BuildIndex("run_number");
							run->trun->Write("", TObject::kWriteDelete);
						}
					}

					free(event);
					event = nullptr;

					cout << "\rFilled event " << event_counter;
					event_counter++;
				}
				free(out_buf);

				// In case of no events, continue
				if (event_counter == 0)
				{
					cout << "No events found in the hardware file. The ROOT file will not be written." << endl;
					if (fp != nullptr)
					{
						fclose(fp); // close the file
						free(filehdr);
						fp = nullptr;
					}
					continue;
				}

			}
			if (fp != nullptr)
			{
				fclose(fp); // close the file
				free(filehdr);
				fp = nullptr;
			}

			// In case of a bad return above, just close the file and continue to the next file
			if (ret_val < 0)
			{
				if (!old_style_output)
				{
					trawvoltage_file->cd();
					// auto voltage = new TRawVoltage(trawvoltage_file);
					auto voltage = make_unique<TRawVoltage>(trawvoltage_file);
					finalise_and_close_event_trees(*ADC, *voltage, *run, fn_tokens, first_event, last_event, is_fv2,
												   old_style_output);
					// delete ADC;
					// delete voltage;

					trun_file->cd();
					if (cons_ev_num)
					{
						last_event = event_counter - 1;
						if (gp13v1) last_event_time = *min_element(ADC->du_seconds.begin(), ADC->du_seconds.end());
						else last_event_time = ADC->time_seconds;
					}
					run->UpdateAndWrite(first_first_event, first_first_event_time, last_event, last_event_time,
										old_trun);
					if (cons_ev_num)
					{
						first_event = event_counter;
					}
				}

				// delete run;
				// delete trun_file;
				// delete tadc_file;
				// delete trawvoltage_file;
				// delete trunrawvoltage_file;
				// return ret_val;
				continue;
			}

			if (cons_ev_num) last_event = event_counter - 1;

			trawvoltage_file->cd();
			// auto voltage = new TRawVoltage(trawvoltage_file);
			auto voltage = make_unique<TRawVoltage>(trawvoltage_file);

			finalise_and_close_event_trees(*ADC, *voltage, *run, fn_tokens, first_event, last_event, is_fv2,
										   old_style_output);

			// delete ADC;
			// delete voltage;
			// delete tadc_file;
			// delete trawvoltage_file;

			if (!old_style_output) filesystem::current_path(input_files_dir);
		}

		// For the new style output fill, write and close the run after all the files
		if (!old_style_output)
		{
			filesystem::current_path(dir_name);
			run->trun->SetDirectory(trun_file);
			trun_file->cd();
			if (cons_ev_num)
			{
				last_event = event_counter - 1;
				if (gp13v1) last_event_time = *min_element(ADC->du_seconds.begin(), ADC->du_seconds.end());
				else last_event_time = ADC->time_seconds;
			}
			run->UpdateAndWrite(first_first_event, first_first_event_time, last_event, last_event_time, old_trun);
			trun_file->Close();
			filesystem::current_path(input_files_dir);

			// delete run;
			// delete trunrawvoltage_file;
			// delete trun_file;
		}
	}

	cout << "\nFinished, quitting" << endl;

    return 0;
}

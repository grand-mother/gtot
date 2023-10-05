#include <iostream>
#include <fstream>
#include "inc/Traces1.h"
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
	cout << "Usage: gtot <options> <hardware_file_names>.dat/.f0*" << endl;
	cout << "Options:" << endl;
	cout << "\t-h, --help\t\t\t\tdisplay this help" << endl;
	cout << "\t-g1, --gp13v1\t\t\t\tthe input file is a GP13 v1 file" << endl;
	cout << "\t-o, --output_filename <filename>\tname of the file to which store the TTrees" << endl;
	cout << "\t-i, --input_filename <filename>\t\tname of the single file to analyse, regardless of extension. No other files accepted." << endl;
	cout << "\t-v, --verbose\t\t\t\tswitch on verbose output" << endl;
}

// Array holding filenames to analyse
TObjArray filenames;
string output_filename="";
bool gp13v1 = false;
string file_format = "";
bool infile_forced = false;

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
		if((strlen(argv[i])>=2 && strstr(argv[i],"-h")) || strstr(argv[i],"--help"))
		{
			print_help();
			exit(0);
		}
		// File to analyse
		else if((strstr(argv[i],".dat") || strstr(argv[i],".f0")) && !infile_forced)
		{
			filenames.Add((TObject*)(new TString(argv[i])));
			cout << "Added " << ((TString*)(filenames.Last()))->Data() << endl;
		}
		// Forced input file
		else if((strlen(argv[i])>=2 && strstr(argv[i],"-i")) || strstr(argv[i],"--input_filename"))
		{
			infile_forced=true;
			filenames.Clear();
			filenames.Add((TObject*)(new TString(argv[i+1])));
		}
		else if((strlen(argv[i])>=2 && strstr(argv[i],"-o")) || strstr(argv[i],"--output_filename"))
		{
			output_filename = argv[i+1];
			++i;
		}
		else if((strlen(argv[i])>=2 && strstr(argv[i],"-g1")) || strstr(argv[i],"--gp13v1"))
		{
			cout << "Switching to GP13 v1 mode" << endl;
			gp13v1 = true;
			file_format = "gp13v1";
		}
		else if((strlen(argv[i])==2 && strstr(argv[i],"-v")) || strstr(argv[i],"--verbose"))
		{
			cout << "Enabled verbose output" << endl;
			overbose = true;
		}
	}
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
		cout << "No files with .dat extension provided! If you want to use other extension for a single file, use option -i." << endl;
		return -1;
	}
	// No sense to use output filename if more than one input file was provided
	if(!(output_filename=="") && filenames.GetEntries()>1)
	{
		cout << "Specifying output file name makes no sense in case of multiply input files" << endl;
		return -1;
	}

	// Loop through the input files
	for(int j=0; j<filenames.GetEntries(); ++j)
	{
		string filename  = ((TString *) (filenames.At(j)))->Data();


		// Assume the file to analyse is the last parameter
		cout << "\n*** Analysing " << filename << endl;

		// If no output was provided, replace the file extension with ".root"
		if (output_filename == "" || filenames.GetEntries()>1)
		{
			output_filename = filename;
			// Leaving the old extension inside the new filename, as it contains an ordinal number
			output_filename = output_filename.replace(output_filename.find_last_of("."), 1, "_");
//			output_filename = output_filename.replace(output_filename.find_last_of("."), output_filename.size() - output_filename.find_last_of("."), ".root");
			output_filename+=".root";
			cout << "Storing output in " << output_filename << endl;
		}
		auto tree_file = new TFile(output_filename.c_str(), "recreate");

		// Just for debugging for now
//	tree_file->SetCompressionLevel(0);

		// Need to define a dictionary so that ROOT can handle vector<vector<unsigned short> >
//	gInterpreter->GenerateDictionary("vector<vector<unsigned short> >", "vector");
//	gInterpreter->GenerateDictionary("vector<vector<short> >", "vector");
//	gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");

		// The whole Run class
		auto run = new TRun();
		// The ADC event class
		auto ADC = new TADC();

		//! File reading
		FILE *fp;
		int i, ich, ib;

		fp = fopen(filename.c_str(), "r");
		if (fp == NULL) printf("Error opening file %s\n", argv[1]);

		int *filehdr = NULL;
		unsigned short *event = NULL;

		int ret_val = 0;

		// Read the file from the detector and fill in the TTrees with the read-out data
		if (grand_read_file_header(fp, &filehdr))
		{
			// Read the file header and fill the Run TTree
			run->SetValuesFromPointers(filehdr, file_format);

			// Event counter
			int event_counter = 0;

			// For GP13 move in the file
			if (gp13v1) fseek(fp, 256, 0);
			// Loop-read the events
			while (grand_read_event(fp, &event, file_format.c_str()) > 0)
			{
				ret_val = ADC->SetValuesFromPointers(event, file_format);
				if (ret_val < 0) break;
				// For GP13v1 overwrite some values
				if (gp13v1)
				{
					// The run number is not specified in GPv13 event, only in header of the file
					ADC->run_number = run->run_number;
					// The event number - in raw data it is separate for each DU and thus not unique, while it needs to be unique in the TTree
					ADC->event_number = event_counter;
				}
				ADC->tadc->Fill();

				// For the first event, fill some trun values read by tadc
				if (event_counter == 0)
				{
					// Event type and version
					run->event_version = ADC->event_version;
					run->event_type = ADC->event_type;
					// The time bin size
					// ToDo: BUG!!! It fills only the first event DUs, while it should all the DUs, like in TRun::SetValuesFromPointers
					run->SetTBinSizeFromADCSamplingFrequency(ADC);

					// Add a comment
					TString comment = TString("Generated by GtoT ") + run->data_generator_version + TString(" from file ")+filename;
					((TNamed*)run->trun->GetUserInfo()->FindObject("comment"))->SetTitle(comment);

					// Write the Run TTree
					run->trun->Fill();
					run->trun->BuildIndex("run_number");
					run->trun->Write("", TObject::kWriteDelete);
				}

				cout << "\rFilled event " << event_counter;
				event_counter++;
			}
		}
		if (fp != NULL) fclose(fp); // close the file

		// In case of a bad return above, just close the file and exit
		if (ret_val < 0)
		{
			tree_file->Close();
			return ret_val;
		}

		// Build the run_number/event_number index for ADC TTree
		ADC->tadc->BuildIndex("run_number", "event_number");
		// Add the Run TTree as a friend
		ADC->tadc->AddFriend(run->trun);
		// Write out the ADC TTree to the file
		cout << "Writing TADC tree" << endl;
		ADC->tadc->Write("", TObject::kWriteDelete);

		// Create the TRawVoltage TTree
		auto voltage = new TRawVoltage(ADC);
		// Add the Run TTree as a friend
		voltage->trawvoltage->AddFriend(run->trun);
		// Write out the Voltage TTree to the file
		cout << "Writing TRawVoltage tree" << endl;
		voltage->trawvoltage->Write("", TObject::kWriteDelete);

		// Close the TFile with the TTrees
		tree_file->Close();
	}

	cout << "\nFinished, quitting" << endl;

    return 0;
}

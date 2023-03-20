#include <iostream>
#include "Traces1.h"
#include "TFile.h"
#include "TInterpreter.h"

#include "TRun.h"
#include "TADC.h"
#include "TRawVoltage.h"

// Prints help in the command line
void print_help()
{
	cout << "GRAND TO TTrees (gtot), program for converting GRAND experiment binary files in raw hardware format to CERN ROOT TTrees.\nMade by Lech Wiktor Piotrowski <lech.piotrowski@fuw.edu.pl>, University of Warsaw, Poland." << endl << endl;
	cout << "Usage: gtot <options> hardware_file_name" << endl;
	cout << "Options:" << endl;
	cout << "\t-h, --help\t\t\tdisplay this help" << endl;
	cout << "\t-g1, --gp13v1\t\t\tthe input file is a GP13 v1 file" << endl;
	cout << "\t-o, --output_filename <filename>\tname of the file to which store the TTrees" << endl;
}

string filename;
string output_filename="";
bool gp13v1 = false;
string file_format = "";

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
		else if((strlen(argv[i])>=2 && strstr(argv[i],"-o")) || strstr(argv[i],"--output_filename"))
		{
			output_filename = argv[i+1];
			++i;
		}
		else if((strlen(argv[i])>=2 && strstr(argv[i],"-g1")) || strstr(argv[i],"--gp13v1"))
		{
			gp13v1 = true;
			file_format = "gp13v1";
		}
	}
}


int main(int argc, char **argv)
{
	analyse_command_line_params(argc, argv);

	// Assume the file to analyse is the last parameter
	filename = argv[argc-1];
	cout << "Analysing " << filename << endl;

	// If no output was provided, replace the file extension with ".root"
	if(output_filename=="")
	{
		output_filename = filename;
		output_filename = output_filename.replace(output_filename.find_last_of("."), output_filename.size()-output_filename.find_last_of("."), ".root");
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
	int i,ich,ib;

	fp = fopen(filename.c_str(),"r");
	if(fp == NULL) printf("Error opening file %s\n",argv[1]);

	int *filehdr=NULL;
	unsigned short *event=NULL;

	// Read the file from the detector and fill in the TTrees with the read-out data
	if(grand_read_file_header(fp, &filehdr))
	{
		// Read the file header and fill the Run TTree
		run->SetValuesFromPointers(filehdr);
		// Write the Run TTree
		run->trun->Fill();
		run->trun->BuildIndex("run_number");
		run->trun->Write();

		// For GP13 move in the file
		if(gp13v1) fseek(fp, 256, 0);
		// Loop-read the events
		while(grand_read_event(fp, &event, file_format.c_str())>0)
		{
			cout << "New event" << endl;
			ADC->SetValuesFromPointers(event, file_format);
			ADC->tadc->Fill();
		}
	}
	if (fp != NULL) fclose(fp); // close the file

	// Build the run_number/event_number index for ADC TTree
	ADC->tadc->BuildIndex("run_number", "event_number");
	// Add the Run TTree as a friend
	ADC->tadc->AddFriend(run->trun);
	// Write out the ADC TTree to the file
	cout << "Writing TADC tree" << endl;
	ADC->tadc->Write();

	// Create the TRawVoltage TTree
	auto voltage = new TRawVoltage(ADC);
	// Add the Run TTree as a friend
	voltage->trawvoltage->AddFriend(run->trun);
	// Write out the Voltage TTree to the file
	cout << "Writing TRawVoltage tree" << endl;
	voltage->trawvoltage->Write();

	// Close the TFile with the TTrees
	tree_file->Close();

	cout << "Finished, quitting" << endl;

    return 0;
}

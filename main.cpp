#include <iostream>
#include "Traces1.h"
#include "TFile.h"
#include "TInterpreter.h"

#include "TRun.h"
#include "TADC.h"
#include "TRawVoltage.h"

int main(int argc, char **argv)
{
    auto tree_file = new TFile("test.root", "recreate");
	// Just for debugging for now
	tree_file->SetCompressionLevel(0);

    // Need to define a dictionary so that ROOT can handle vector<vector<unsigned short> >
	gInterpreter->GenerateDictionary("vector<vector<unsigned short> >", "vector");
	gInterpreter->GenerateDictionary("vector<vector<short> >", "vector");
	gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");

	// The whole Run class
	auto run = new TRun();
	// The ADC event class
    auto ADC = new TADC();

    //! File reading
	FILE *fp;
	int i,ich,ib;

	fp = fopen(argv[1],"r");
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

		// Loop-read the events
		while(grand_read_event(fp, &event)>0)
		{
			cout << "New event" << endl;
			ADC->SetValuesFromPointers(event);
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

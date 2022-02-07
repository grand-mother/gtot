#include <iostream>
#include "Traces1.h"
#include "TFile.h"
#include "TInterpreter.h"

#include "GRANDEventADC.h"

int main(int argc, char **argv)
{
    auto tree_file = new TFile("test.root", "recreate");

    // Need to define a dictionary so that ROOT can handle vector<vector<unsigned short> >
	gInterpreter->GenerateDictionary("vector<vector<unsigned short> >", "vector");
	gInterpreter->GenerateDictionary("vector<vector<short> >", "vector");

    auto g = new GRANDEventADC();
    g->CreateTree();

    //! File reading
	FILE *fp;
	int i,ich,ib;
	char fname[100],hname[100];

	fp = fopen(argv[1],"r");
	if(fp == NULL) printf("Error opening file %s\n",argv[1]);

	int *filehdr=NULL;
	unsigned short *event=NULL;

	if(grand_read_file_header(fp, &filehdr) ){ //lets read events
		//print_file_header();
		while (grand_read_event(fp, &event) >0 ) {
			g->SetValuesFromPointers(event);
			cout << "set" << endl;
			g->teventadc->Fill();
		}
	}
	if (fp != NULL) fclose(fp); // close the file

	//g->teventadc->Scan("run_number:event_number:event_type:adc_track0[0]:adc_track1[0]:adc_track2[0]:adc_track3[0]");

	g->teventadc->Write();
	tree_file->Close();

    return 0;
}

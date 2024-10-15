//
// Created by lewhoo on 15/10/24.
//

#ifndef GTOT_MAIN_FUNCTIONS_H
#define GTOT_MAIN_FUNCTIONS_H

#include <iostream>
#include "TRun.h"
#include "TADC.h"
#include "TRawVoltage.h"

void finalise_and_close_event_trees(TADC *ADC, TRawVoltage *voltage, TRun *run, vector<string> fn_tokens, unsigned int first_event, unsigned int last_event, bool is_fv2=false, bool old_style_output=false)
{
	// Build the run_number/event_number index for ADC TTree
	ADC->tadc->BuildIndex("run_number", "event_number");

	// Write out the ADC TTree to the file
	cout << "Writing TADC tree" << endl;
	ADC->tadc->GetCurrentFile()->cd();
	ADC->tadc->Write("", TObject::kWriteDelete);

	// Create the TRawVoltage TTree and copying the valid TADC contents (need to do that before the change of TADC file name)
//		auto voltage = new TRawVoltage(ADC, is_fv2, trawvoltage_file);
	voltage->ComputeFromADC(ADC, is_fv2);

	string tadc_name = string("adc_")+fn_tokens[1]+string("_")+fn_tokens[2]+string("_")+to_string(first_event)+"-"+to_string(last_event)+"_L1_0000.root";
	cout << "Renaming tadc.root to " << tadc_name << endl;
	ADC->ChangeFileName(tadc_name);
	// Add the Run TTree as a friend
	ADC->tadc->AddFriend(run->trun);
	ADC->tadc->Write("", TObject::kWriteDelete);

	// Write out the Voltage TTree to the file
	cout << "Writing TRawVoltage tree" << endl;
	voltage->trawvoltage->GetCurrentFile()->cd();
	voltage->trawvoltage->Write("", TObject::kWriteDelete);
	string trawvoltage_name = string("rawvoltage_")+fn_tokens[1]+string("_")+fn_tokens[2]+string("_")+to_string(first_event)+"-"+to_string(last_event)+"_L1_0000.root";
	cout << "Renaming rawvoltage.root to " << trawvoltage_name << endl;
	voltage->ChangeFileName(trawvoltage_name);
	// Add the Run TTree as a friend
	voltage->trawvoltage->AddFriend(run->trun);
	voltage->trawvoltage->Write("", TObject::kWriteDelete);

	// Close TFiles with the TTrees for the old style output
	// the trun_file contains also the event trees
	if(old_style_output)
		run->trun->GetCurrentFile()->Close();
	else
	{
		ADC->tadc->GetCurrentFile()->Close();
		voltage->trawvoltage->GetCurrentFile()->Close();
//			last_event = event_counter-1;
//			rename_event_files(".", fn_tokens[1], fn_tokens[2], first_event, last_event);
	}

}

#endif //GTOT_MAIN_FUNCTIONS_H

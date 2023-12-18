#ifndef GTOT_TRACES_FV2_H
#define GTOT_TRACES_FV2_H

namespace fv2
{
/* First define the header words */
	const int FILE_HDR_LENGTH = 0;
	const int FILE_HDR_RUNNR = 1;
	const int FILE_HDR_RUN_MODE = 2;
	const int FILE_HDR_SERIAL = 3;
	const int FILE_HDR_FIRST_EVENT = 4;
	const int FILE_HDR_FIRST_EVENT_SEC = 5;
	const int FILE_HDR_LAST_EVENT = 6;
	const int FILE_HDR_LAST_EVENT_SEC = 7;
	const int FILE_HDR_ADDITIONAL = 8; //start of additional info to be defined

	const int EVENT_HDR_LENGTH = 0;
	const int EVENT_HDR_RUNNR = 1;
	const int EVENT_HDR_EVENTNR = 2;
	const int EVENT_HDR_T3EVENTNR = 3;
	const int EVENT_HDR_FIRST_DU = 4;
	const int EVENT_HDR_EVENT_SEC = 5;
	const int EVENT_HDR_EVENT_NSEC = 6;
	const int EVENT_HDR_EVENT_TYPE = 7;
	const int EVENT_HDR_EVENT_VERS = 8;
	const int EVENT_HDR_NDU = 9; //start of additional info to be defined
	const int EVENT_HDR_AD1 = 10; //                    info to be defined
	const int EVENT_DU = 11;

	const int EVT_HDR_LENGTH = 146;
	const int EVT_LENGTH = 0; // nr of int32 words
	const int EVT_VERSION = 1;
	const int EVT_STATION_ID = 2;
	const int EVT_HARDWARE_ID = 3;
	const int EVT_EVT_ID = 4;
	const int EVT_CTP = 5;
	const int EVT_CTD = 6;
	const int EVT_ADCINFO = 7;
	const int EVT_SECOND = 8;
	const int EVT_NANOSEC = 9;
	const int EVT_TRIGGER_POS = 10;
	const int EVT_TRIGGER_STAT = 11;
	const int EVT_STATISTICS = 12;
	const int EVT_PPS_ID = 13;
	const int EVT_SPARE1 = 14;
	const int EVT_SPARE2 = 15;
	const int EVT_SPARE3 = 16;
	const int EVT_ATM_TP = 17;
	const int EVT_HM_AX = 18;
	const int EVT_AY_AZ = 19;
	const int EVT_BATTERY = 20;
	const int EVT_WEEKTIME = 21;
	const int EVT_WEEKOFFSET = 22;
	const int EVT_SECMINHOUR = 23;
	const int EVT_DAYMONTH = 24;
	const int EVT_YEAR = 25;
	const int EVT_GPSMODE = 26;
	const int EVT_GPSSTATUS = 27;
	const int EVT_OFFSET = 28;
	const int EVT_TEMPERATURE = 29;
	const int EVT_LATITUDE = 31;
	const int EVT_LONGITUDE = 33;
	const int EVT_ALTITUDE = 35;
	const int EVT_TRACELENGTH = 36;
	const int EVT_INP_SELECT = 37;
	const int EVT_TRIG_SELECT = 38;
	const int EVT_THRES_C1 = 39;
	const int EVT_THRES_C2 = 40;
	const int EVT_THRES_C3 = 41;
	const int EVT_TRIG_C1 = 43;
	const int EVT_TRIG_C2 = 45;
	const int EVT_TRIG_C3 = 47;
	const int EVT_GAIN_AB = 51;
	const int EVT_GAIN_CD = 52;
	const int EVT_BASELINE_12 = 53;
	const int EVT_BASELINE_3 = 54;
	const int EVT_NOTCH_C1_F1 = 63;
	const int EVT_NOTCH_C1_F2 = 68;
	const int EVT_NOTCH_C1_F3 = 73;
	const int EVT_NOTCH_C1_F4 = 78;
	const int EVT_NOTCH_C2_F1 = 83;
	const int EVT_NOTCH_C2_F2 = 88;
	const int EVT_NOTCH_C2_F3 = 93;
	const int EVT_NOTCH_C2_F4 = 98;
	const int EVT_NOTCH_C3_F1 = 103;
	const int EVT_NOTCH_C3_F2 = 108;
	const int EVT_NOTCH_C3_F3 = 113;
	const int EVT_NOTCH_C3_F4 = 118;
	const int EVT_TOT_SAMPLEP = 143;
	const int EVT_CH12_SAMPLEP = 144;
	const int EVT_CH3_SAMPLEP = 145;
	const int EVT_START_ADC = 146;

	const int INTSIZE = 4;
	const int SHORTSIZE = 2;
}

#endif //GTOT_TRACES_FV2_H
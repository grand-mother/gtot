// Traces.c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Traces_fv2.h"
//const int INTSIZE=4;
//const int SHORTSIZE=2;

//int *filehdr=NULL;
//unsigned int *event=NULL;

// Use the binary blob addressing from firmware version 2
using namespace fv2;
namespace fv2
{
//	int grand_read_file_header(FILE *fp)
	int grand_read_file_header(FILE *fp, int **pfilehdr)
	{
		int *filehdr = NULL;
		int i, return_code;
		int isize;

		if (!fread(&isize, INTSIZE, 1, fp))
		{
			printf("Cannot read the header length\n");
			return (0);                                                       //cannot read the header length
		}
		printf("The header length is %d bytes \n", isize);
		if (isize < FILE_HDR_ADDITIONAL)
		{
			printf("The file header is too short, only %d integers\n", isize);
			return (0);                                                       //file header too short
		}
		if (filehdr != NULL) free((void *) filehdr);                         //in case we run several files
		filehdr = (int *) malloc(isize + INTSIZE);                            //allocate memory for the file header
		if (filehdr == NULL)
		{
			printf("Cannot allocate enough memory to save the file header!\n");
			return (0);                                                       //cannot allocate memory for file header
		}
		filehdr[0] = isize;                                                //put the size into the header
		if ((return_code = fread(&(filehdr[1]), 1, isize, fp)) != (isize))
		{
			printf("Cannot read the full header (%d)\n", return_code);
			return (0);                                                       //cannot read the full header
		}
		*pfilehdr = filehdr;
		return (1);
	}

//	int grand_read_event(FILE *fp)
	int grand_read_event(FILE *fp, unsigned short **pevent, const char *file_format)
	{
		int isize, return_code;

		std::ostream &vout = *pvout;

		unsigned int *event=NULL;

		if (!fread(&isize, INTSIZE, 1, fp))
		{
			printf("\nCannot read the Event length\n");
			return (0);                                                       //cannot read the header length
		}
		vout << "The event length is " << isize << " bytes \n";
		if (event != NULL)
		{
			if (event[0] != isize)
			{
				free((void *) event);                                           //free and alloc only if needed
				event = (unsigned int *) malloc(isize + INTSIZE);                          //allocate memory for the event
			}
		} else
		{
			event = (unsigned int *) malloc(isize + INTSIZE);                          //allocate memory for the event
		}
		if (event == NULL)
		{
			printf("Cannot allocate enough memory to save the event!\n");
			return (0);                                                       //cannot allocate memory for event
		}
		event[0] = isize;
		if ((return_code = fread(&(event[1]), 1, isize, fp)) != (isize))
		{
			printf("Cannot read the full event (%d)\n", return_code);
			return (0);                                                       //cannot read the full event
		}

		*pevent = (unsigned short*)event;

		return (1);
	}

	void print_du_general(uint32_t *du)
	{
		printf("\t Total Length= %d, Header Length = %d\n", du[EVT_LENGTH] >> 16, du[EVT_LENGTH] & 0xffff);
		printf("\t Versions\n\t\t Data Format %d\n\t\t Firmware %d\n",
			   (du[EVT_VERSION] >> 24) & 0xff, (du[EVT_VERSION] >> 16) & 0xff);
		printf("\t\t Adaq %d\n\t\t Dudaq %d\n",
			   (du[EVT_VERSION] >> 8) & 0xff, du[EVT_VERSION] & 0xff);
		printf("\t Station id %d Hardware id %d\n",
			   du[EVT_STATION_ID], du[EVT_HARDWARE_ID]);
		printf("\t Event id %d\n", du[EVT_EVT_ID]);
		printf("\t CTP %d\n", du[EVT_CTP]);
		printf("\t CTD %d\n", du[EVT_CTD]);
		printf("\t ADC MSPS %d Bits %d\n", du[EVT_ADCINFO] >> 16, du[EVT_ADCINFO] & 0xffff);
		printf("\t Dudaq Time %u.%09u\n", du[EVT_SECOND], du[EVT_NANOSEC]);
		printf("\t Trigger\n\t\t Position %d\n\t\t T3 Flag 0x%x\n\t\t Status 0x%x\n", du[EVT_TRIGGER_POS] >> 16, du[EVT_TRIGGER_STAT] >> 16, du[EVT_TRIGGER_STAT] & 0xffff);
		printf("\t Statistics\n\t\t Trigger rate %d\n\t\t DDR Storage %d\n", du[EVT_STATISTICS] >> 16, du[EVT_STATISTICS] & 0xffff);
	}

	void print_sensor(uint32_t *du)
	{
		float val;
		short ADC;
		ADC = du[EVT_ATM_TP] >> 16;
		val = ADC * 2500. / 4096; //millivolt
		val = (val - 400) / 19.5; //degrees
		printf("\t Sensor Temperature = %f degrees\n", val);
		printf("\t Sensor Pressure (ADC) = %d\n", du[EVT_ATM_TP] & 0xffff);
		ADC = du[EVT_HM_AX] >> 16;
		val = ADC * 2.5 / 4096; //volt
		val = (val / 3.3 - 0.1515) / 0.00636;
		printf("\t Sensor Humidity = %f\n", val);
		printf("\t Sensor Gravity(ADC) (%d,%d,%d)\n", du[EVT_HM_AX] & 0xffff, du[EVT_AY_AZ] >> 16, du[EVT_AY_AZ] & 0xffff);
		ADC = du[EVT_BATTERY] >> 16;
		val = ADC * 2.5 / 4096; //volt
		val = val * (18 + 91) / 18; //voltage divider
		printf("\t Battery = %f V\n", val);

	}

	void print_gps(uint32_t *du)
	{
		double longitude, latitude, altitude;
		uint32_t *tt;
		printf("\t Seconds since Sunday 00:00 %d\n", du[EVT_WEEKTIME]);
		printf("\t Week nr %d UTC offset %d\n", du[EVT_WEEKOFFSET] >> 16, du[EVT_WEEKOFFSET] & 0xffff);
		printf("\t GPS Time %02d:%02d:%02d %02d-%02d-%04d Status 0x%02x\n",
			   du[EVT_SECMINHOUR] & 0xff, (du[EVT_SECMINHOUR] >> 8) & 0xff,
			   (du[EVT_SECMINHOUR] >> 16) & 0xff, (du[EVT_DAYMONTH] >> 24) & 0xff,
			   (du[EVT_DAYMONTH] >> 16) & 0xff, du[EVT_YEAR],
			   (du[EVT_SECMINHOUR] >> 24) & 0xff);
		printf("\t GPS Temperature = %f\n", *(float *) &du[EVT_TEMPERATURE]);
		printf("\t PPS Offset= %f\n", *(float *) &du[EVT_OFFSET]);
		// Next: swap words to get the right GPS coordinates
		tt = (uint32_t *) &latitude;
		tt[1] = du[EVT_LATITUDE - 1];
		tt[0] = du[EVT_LATITUDE];
		tt = (uint32_t *) &longitude;
		tt[1] = du[EVT_LONGITUDE - 1];
		tt[0] = du[EVT_LONGITUDE];
		tt = (uint32_t *) &altitude;
		tt[1] = du[EVT_ALTITUDE - 1];
		tt[0] = du[EVT_ALTITUDE];
		printf("\t Latitude = %9.7f Longitude = %9.7f Altitude = %9.7f\n",
			   57.3 * latitude, 57.3 * longitude, altitude);
		printf("\t GPS Receiver mode 0x%02x Disc. Mode 0x%02x Self Survey 0x%02x\n",
			   (du[EVT_GPSMODE] >> 24) & 0xff, (du[EVT_GPSMODE] >> 16) & 0xff, (du[EVT_GPSMODE] >> 8) & 0xff);
		printf("\t GPS Minor Alarms 0x%04x GNSS Decoding 0x%02x Disc. Act. 0x%02x\n",
			   (du[EVT_GPSSTATUS] >> 16) & 0xffff, (du[EVT_GPSSTATUS] >> 8) & 0xff, (du[EVT_GPSSTATUS]) & 0xff);

	}

	void print_channel_info(uint32_t *du)
	{
		printf("\t Trace Length = %d ns\n", 4 * (du[EVT_TRACELENGTH] >> 16));
		for (int ich = 0; ich < 3; ich++)
		{
			printf("\t Hardware Input Channel %d ", ich);
			if (((du[EVT_INP_SELECT] >> 5 * ich) & 0x1e) == 0) printf("Off");
			else
			{
				if (du[EVT_INP_SELECT] & (1 << (5 * ich))) printf("Filtered ");
				for (int iadc = 0; iadc < 4; iadc++)
				{
					if (du[EVT_INP_SELECT] & (1 << (5 * ich + iadc + 1)))
						printf("ADC_%c ", 'A' + iadc);
				}
			}
			printf("\n");
		}
		printf("\t Trigger Selection:\n\t");
		if (du[EVT_TRIG_SELECT] & (1))printf("\t Ch1");
		if (du[EVT_TRIG_SELECT] & (1 << 1))printf("\t Ch2");
		if (du[EVT_TRIG_SELECT] & (1 << 2))printf("\t Ch3");
		if (du[EVT_TRIG_SELECT] & (1 << 3))printf("\t Ch4");
		if (du[EVT_TRIG_SELECT] & (1 << 4))printf("\t Ch1&Ch2");
		if (du[EVT_TRIG_SELECT] & (1 << 5))printf("\t Ch1&Ch2&Ch3");
		if (du[EVT_TRIG_SELECT] & (1 << 6))printf("\t Ch1&Ch3&~Ch3");
		if (du[EVT_TRIG_SELECT] & (1 << 7))printf("\t 20Hz");
		if (du[EVT_TRIG_SELECT] & (1 << 8))printf("\t 10 sec");
		printf("\n");
		int period = ((du[EVT_TRIG_SELECT] >> 13) & 0xf) << (2 + ((du[EVT_TRIG_SELECT] >> 9) & 0xf));
		if (period != 0)printf("\t\t Custom Trigger Freq = %f Hz\n", 1.E9 / period);
		for (int ich = 0; ich < 3; ich++)
		{
			printf("\t Ch %d Signal Thres %d ADC Noise Thres %d ADC\n",
				   ich + 1, (du[EVT_THRES_C1 + ich] >> 12) & 0xfff,
				   du[EVT_THRES_C1 + ich] & 0xfff);
		}
		for (int ich = 0; ich < 3; ich++)
		{
			printf("\t Ch %d ", ich + 1);
			printf("Pre-trigger period = %d ns ", 4 * ((du[EVT_TRIG_C1 + 2 * ich] >> 21) & 0x1ff));
			printf("Post-trigger period = %d ns \n", 4 * ((du[EVT_TRIG_C1 + 2 * ich] >> 12) & 0x1ff));
			printf("\t\t Max time between T2s=%d ns ", 4 * ((du[EVT_TRIG_C1 + 2 * ich] >> 9) & 0x7));
			printf(" T2 Crossing Limits [%d,%d]\n", (du[EVT_TRIG_C1 + 2 * ich] >> 5) & 0xf, du[EVT_TRIG_C1 + 2 * ich] & 0x1f);
		}
		printf("\t Add gain ADC_A %g ADC_B %g\n",
			   2.5 * 37.5 * ((du[EVT_GAIN_AB] >> 16) - 0.5) / 4096 - 14,
			   2.5 * 37.5 * ((du[EVT_GAIN_AB] & 0xffff) - 0.5) / 4096 - 14);
		printf("\t Add gain ADC_C %g ADC_D %g\n",
			   2.5 * 37.5 * ((du[EVT_GAIN_CD] >> 16) - 0.5) / 4096 - 14,
			   2.5 * 37.5 * ((du[EVT_GAIN_CD] & 0xffff) - 0.5) / 4096 - 14);
		printf("\t Max. value Baseline subtraction ch1-3: ");
		printf("%d %d %d\n",
			   du[EVT_BASELINE_12] & 0x3ff,
			   (du[EVT_BASELINE_12] >> 13) & 0x3ff,
			   du[EVT_BASELINE_3] & 0x3ff);
		printf("\t # Samples in Baseline subtraction ch1-3: ");
		printf("%d %d %d\n",
			   1 << ((du[EVT_BASELINE_12] >> 10) & 0x7),
			   1 << ((du[EVT_BASELINE_12] >> 23) & 0x7),
			   1 << ((du[EVT_BASELINE_3] >> 10) & 0x7));
	}

	void print_notch_filters(uint32_t *du)
	{
		printf("\t Ch1 # Notch filters implemented %d\n",
			   (du[EVT_TRIG_SELECT] >> 17) & 0x7);
		printf("\t Ch2 # Notch filters implemented %d\n",
			   (du[EVT_TRIG_SELECT] >> 20) & 0x7);
		printf("\t Ch3 # Notch filters implemented %d\n",
			   (du[EVT_TRIG_SELECT] >> 23) & 0x7);
	}

	void print_adc_values(uint32_t *du)
	{
		int offset = EVT_START_ADC;
		short val1, val2;
		for (int ich = 0; ich < 3; ich++)
		{
			printf("Channel %d ADC:\n   0:", ich + 1);
			if (((du[EVT_INP_SELECT] >> 5 * ich) & 0x1e) != 0)
			{
				for (int i = 0; i < (du[EVT_TRACELENGTH] >> 16); i++)
				{
					val1 = du[offset + i] >> 16;
					val2 = du[offset + i] & 0xffff;
					printf("%5d %5d ", val1, val2);
					if ((i % 6) == 5)
					{
						printf("\n");
						if ((i + 1) < (du[EVT_TRACELENGTH] >> 16))printf("%4d:", 2 * (i + 1));
					}
				}
				offset += (du[EVT_TRACELENGTH] >> 16);
				printf("\n");
			}
		}
	}

	void print_du(uint32_t *du)
	{
		print_du_general(du);
		print_sensor(du);
		print_gps(du);
		print_channel_info(du);
		print_notch_filters(du);
		print_adc_values(du);
	}

//	void print_grand_event()
//	{
//		uint32_t *evdu;
//		int idu = EVENT_DU;                                                      //parameter indicating start of LS
//		int ev_end = event[EVENT_HDR_LENGTH] / 4;
//		printf("Event Size = %d\n", event[EVENT_HDR_LENGTH]);
//		printf("      Run Number = %d\n", event[EVENT_HDR_RUNNR]);
//		printf("      Event Number = %d\n", event[EVENT_HDR_EVENTNR]);
//		printf("      T3 Number = %d\n", event[EVENT_HDR_T3EVENTNR]);
//		printf("      First DU = %d\n", event[EVENT_HDR_FIRST_DU]);
//		printf("      Time Seconds = %u\n", event[EVENT_HDR_EVENT_SEC]);
//		printf("      Time Nano Seconds = %d\n", event[EVENT_HDR_EVENT_NSEC]);
//		printf("      Event Type = ");
//		//if((evdu[0] &TRIGGER_T3_MINBIAS)) printf("10 second trigger\n");
//		//else if((evdu[0] &TRIGGER_T3_RANDOM)) printf("random trigger\n");
//		//else printf("Shower event\n");
//		printf("\n");
//		printf("      Number of DU's = %d\n", event[EVENT_HDR_NDU]);
//		evdu = (unsigned int *) &event[EVENT_DU];
//		printf("IDU  \n");
//		//for(int i=0;i<40;i++) printf("\t %d 0x%08x\n",i,evdu[i]);
//		while (idu < ev_end)
//		{
//			printf("IDU = %d (end = %d)\n", idu, ev_end);
//			evdu = (uint32_t *) (&event[idu]);
//			print_du((uint32_t *) evdu);
//			idu += (evdu[EVT_LENGTH] >> 16);
//			printf("IDU = %d (end = %d)\n", idu, ev_end);
//		}
//	}


//	int main(int argc, char **argv)
//	{
//		FILE *fp;
//		int i, ich, ib;
//		char fname[100], hname[100];
//
//		fp = fopen(argv[1], "r");
//		if (fp == NULL) printf("Error opening  !!%s!!\n", argv[1]);
//
//		if (grand_read_file_header(fp))
//		{ //lets read events
//			print_file_header();
//			while (grand_read_event(fp) > 0)
//			{
//				print_grand_event();
//			}
//		}
//		if (fp != NULL) fclose(fp); // close the file
//	}
}
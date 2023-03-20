//#include "amsg.h"
//#include "scope.h"
#include "Traces.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>

#define INTSIZE 4
#define SHORTSIZE 2

//int *filehdr=NULL;
//unsigned short *event=NULL;


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

/*void print_file_header()*/
/*{*/
/*  int i,additional_int;*/
/*  struct tm *mytime;*/
/*  */
/*  additional_int = 1+(filehdr[FILE_HDR_LENGTH]/INTSIZE) - FILE_HDR_ADDITIONAL; //number of additional words in the header*/
/*  if(additional_int<0){*/
/*    printf("The header is too short!\n");*/
/*    return;*/
/*  }*/
/*  printf("Header Length is %d bytes\n",filehdr[FILE_HDR_LENGTH]);*/
/*  printf("Header Run Number is %d\n",filehdr[FILE_HDR_RUNNR]);*/
/*  printf("Header Run Mode is %d\n",filehdr[FILE_HDR_RUN_MODE]);*/
/*  printf("Header File Serial Number is %d\n",filehdr[FILE_HDR_SERIAL]);*/
/*  printf("Header First Event is %d\n",filehdr[FILE_HDR_FIRST_EVENT]);*/
/*  mytime = gmtime((const time_t *)(&filehdr[FILE_HDR_FIRST_EVENT_SEC]));*/
/*  printf("Header First Event Time is %s",asctime(mytime));*/
/*  printf("Header Last Event is %d\n",filehdr[FILE_HDR_LAST_EVENT]);*/
/*  mytime = gmtime((const time_t *)(&filehdr[FILE_HDR_LAST_EVENT_SEC]));*/
/*  printf("Header Last Event Time is %s",asctime(mytime));*/
/*  for(i=0;i<additional_int;i++){*/
/*    printf("HEADER Additional Word %d = %d\n",i,filehdr[i+FILE_HDR_ADDITIONAL]);*/
/*  }*/
/*}*/

int grand_read_event(FILE *fp, unsigned short **pevent, const char *file_format)
{
	// A bool means a lot of bools for more formats, but hopefully it won't come to that
	bool gp13v1 = false;
	if (strstr(file_format, "gp13v1")) gp13v1 = true;

	unsigned short *event = NULL;
	int isize, return_code;

	if (gp13v1) printf("Tag!!!\n");
	if (!fread(&isize, INTSIZE, 1, fp))
	{
		printf("Cannot read the Event length\n");
		return (0);                                                       //cannot read the header length
	}
	printf("The event length is %d bytes \n", isize);
	if (event != NULL)
	{
		if (event[0] != isize)
		{
			free((void *) event);                                           //free and alloc only if needed
			if (gp13v1) event = (unsigned short *) malloc(isize);                          //allocate memory for the event
			else event = (unsigned short *) malloc(isize + INTSIZE);                          //allocate memory for the event
		}
	} else
	{
		if (gp13v1) event = (unsigned short *) malloc(isize);                          //allocate memory for the event
		else event = (unsigned short *) malloc(isize + INTSIZE);                          //allocate memory for the event
	}
	if (event == NULL)
	{
		printf("Cannot allocate enough memory to save the event!\n");
		return (0);                                                       //cannot allocate memory for event
	}
	event[0] = isize & 0xffff;                                                  //put the size into the event
	event[1] = isize >> 16;
	if (gp13v1) return_code = fread(&(event[2]), 1, isize - INTSIZE, fp);
	else return_code = fread(&(event[2]), 1, isize, fp);

	if ((!gp13v1 && return_code != (isize)) || (gp13v1 && (return_code != (isize - INTSIZE))))
	{
		printf("Cannot read the full event (%d)\n", return_code);
		return (0);                                                       //cannot read the full event
	}
	*pevent = event;
	return (1);
}

/*
void print_du(uint16_t *du)
{
  int i,ic;
  int ioff;
  
  printf("\t T3 ID = %u\n",du[EVT_ID]);
  printf("\t DU ID = %u\n",du[EVT_HARDWARE]);
  printf("\t DU time = %u.%09u\n",*(uint32_t *)&du[EVT_SECOND],
         *(uint32_t *)&du[EVT_NANOSEC]);
  printf("\t Trigger Position = %d T3 FLAG = 0x%x\n",
         du[EVT_TRIGGERPOS],du[EVT_T3FLAG]);
  printf("\t Atmosphere (ADC) Temp = %d Pres = %d Humidity = %d\n",
         du[EVT_ATM_TEMP],du[EVT_ATM_PRES],du[EVT_ATM_HUM]);
  printf("\t Acceleration (ADC) X = %d Y = %d Z = %d\n",
         du[EVT_ACCEL_X],du[EVT_ACCEL_Y],du[EVT_ACCEL_Z]);
  printf("\t Battery (ADC) = %d\n",du[EVT_BATTERY]);
  printf("\t Event version = %d\n",du[EVT_VERSION]);
  printf("\t ADC: sampling frequency = %d MHz, resolution=%d bits\n",
         du[EVT_MSPS],du[EVT_ADC_RES]);
  printf("\t ADC Input channels =0x%x, Enabled Channels=0x%x\n",
         du[EVT_INP_SELECT],du[EVT_CH_ENABLE]);
  printf("\t Number of ADC samples Total = %d",16*du[EVT_TOT_SAMPLES]);
  for(i=1;i<=4;i++) printf(" Ch%d = %d",i,du[EVT_TOT_SAMPLES+i]);
  printf("\n");
  printf("\t Trigger pattern=0x%x Rate=%d\n",du[EVT_TRIG_PAT],du[EVT_TRIG_RATE]);
  printf("\t Clock tick %u Nticks/sec %u\n",
         *(uint32_t *)&du[EVT_CTD],*(uint32_t *)&du[EVT_CTP]);
  printf("\t GPS: Offset=%g LeapSec=%d Status 0x%x Alarms 0x%x Warnings 0x%x\n",
         *(float *)&du[EVT_PPS_OFFSET],du[EVT_LEAP],du[EVT_GPS_STATFLAG],
         du[EVT_GPS_CRITICAL],du[EVT_GPS_WARNING]);
  printf("\t GPS: %02d/%02d/%04d %02d:%02d:%02d\n",
         du[EVT_DAYMONTH]&0xff,(du[EVT_DAYMONTH]>>8)&0xff,du[EVT_YEAR],
         du[EVT_MINHOUR]&0xff,(du[EVT_MINHOUR]>>8)&0xff,du[EVT_STATSEC]&0xff);
  printf("\t GPS: Long = %g Lat = %g Alt = %g Chip Temp=%g\n",
         57.3*(*(double *)&du[EVT_LONGITUDE]),57.3*(*(double *)&du[EVT_LATITUDE]),
         *(double *)&du[EVT_ALTITUDE],*(float *)&du[EVT_GPS_TEMP]);
  printf("\t Digi CTRL");
  for(i=0;i<8;i++) printf(" 0x%x",du[EVT_CTRL+i]);
  printf("\n");
  printf("\t Digi Pre-Post trigger windows");
  for(i=0;i<8;i++) printf(" 0x%x",du[EVT_WINDOWS+i]);
  printf("\n");
  for(ic=1;ic<=4;ic++){
    printf("\t Ch%d properties:",ic);
    for(i=0;i<6;i++)printf(" 0x%x",du[EVT_CHANNEL+6*(ic-1)+i]);
    printf("\n");
  }
  for(ic=1;ic<=4;ic++){
    printf("\t Ch%d trigger settings:",ic);
    for(i=0;i<6;i++)printf(" 0x%x",du[EVT_TRIGGER+6*(ic-1)+i]);
    printf("\n");
  }
  ioff = du[EVT_HDRLEN];
  for(ic=1;ic<=4;ic++){
    printf("\t ADC %d:\n\t",ic);
    for(i=0;i<du[EVT_TOT_SAMPLES+ic];i++){
      printf("\t%d",(int16_t)du[ioff++]);
      if((i%12) ==11) printf("\n\t");
    }
    printf("\n");
  }
}

void print_grand_event(){
  uint16_t *evdu;
  unsigned int *evptr = (unsigned int *)event;
  int idu = EVENT_DU;                                                      //parameter indicating start of LS
  int ev_end = ((int)(event[EVENT_HDR_LENGTH+1]<<16)+(int)(event[EVENT_HDR_LENGTH]))/SHORTSIZE;
  printf("Event Size = %d\n",*evptr++);
  printf("      Run Number = %d\n",*evptr++);
  printf("      Event Number = %d\n",*evptr++);
  printf("      T3 Number = %d\n",*evptr++);
  printf("      First DU = %d\n",*evptr++);
  printf("      Time Seconds = %u\n",*evptr++);
  printf("      Time Nano Seconds = %d\n",*evptr++);
  evdu = (uint16_t *)evptr;
  printf("      Event Type = ");
  if((evdu[0] &TRIGGER_T3_MINBIAS)) printf("10 second trigger\n");
  else if((evdu[0] &TRIGGER_T3_RANDOM)) printf("random trigger\n");
  else printf("Shower event\n");
  printf("      Event Version = %d\n",*evdu++);
  evptr +=2;
  printf("      Number of DU's = %d\n",*evptr);
  while(idu<ev_end){
    evdu = (uint16_t *)(&event[idu]);
    print_du(evdu);
    idu +=(evdu[EVT_LENGTH]);
  }
}


int main(int argc, char **argv)
{
  FILE *fp;
  int i,ich,ib;
  char fname[100],hname[100];
  
  fp = fopen(argv[1],"r");
  if(fp == NULL) printf("Error opening  !!%s!!\n",argv[1]);
  
  if(grand_read_file_header(fp) ){ //lets read events
    print_file_header();
    while (grand_read_event(fp) >0 ) {
      print_grand_event();
    }
  }
  if (fp != NULL) fclose(fp); // close the file
}

*/

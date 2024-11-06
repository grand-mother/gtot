/// @file scope.h
/// @brief Header file for GP300 scope
/// @author C. Timmermans, Nikhef/RU

#ifndef GTOT_SCOPE_H
#define GTOT_SCOPE_H


/*
 * Header file for GP300 scope
 *
 * C. Timmermans
 * c.timmermans@science.ru.nl
 *
 * NOT backward compatible with older versions!
 *>
 */

#include <stdint.h>

namespace fv1
{

/*----------------------------------------------------------------------*/
const int SAMPLING_FREQ=500; //!< 500 MHz scope
const int ADC_RESOLUTION=14;

/*----------------------------------------------------------------------*/
#define GPS_EPOCH_UNIX  315964800 //!< GPS to unix offst, not counting leap sec

/* Message start / end flags */
const int MSG_START=0x99; //!< start of fpga message
const int MSG_END=0x66; //!< end of fpga message


/* Time stamp offsets (within 7B time field) */

const int TIME_YEAR=0;
const int TIME_MON=2;
const int TIME_DAY=3;
const int TIME_HOUR=4;
const int TIME_MIN=5;
const int TIME_SEC=6;


/*----------------------------------------------------------------------*/
/* Maxima / minima */

/* Maximum ADC size = 4 channels * max_samples/ch * 2 bytes/sample */
/* Maximum event size = header + ADC data + message end       */
const int DATA_MAX_SAMP=8192;                       //!< Maximal trace length (samples)

#define MAX_READOUT     (256 + DATA_MAX_SAMP*4) //!< Maximal raw event size

/*----------------------------------------------------------------------*/
#define TDAQ_BASE             0x80000000
/* Register Definitions*/
const int Reg_Dig_Control=0x000;
const int Reg_Trig_Enable=0x002;
const int Reg_TestPulse_ChRead=0x004;
const int Reg_Time_Common=0x006;
const int Reg_Inp_Select=0x008;
const int Reg_Spare_A=0x00A;
const int Reg_Spare_B=0x00C;
const int Reg_Spare_C=0x00E;
const int Reg_Time1_Pre=0x010;
const int Reg_Time1_Post=0x012;
const int Reg_Time2_Pre=0x014;
const int Reg_Time2_Post=0x016;
const int Reg_Time3_Pre=0x018;
const int Reg_Time3_Post=0x01A;
const int Reg_Time4_Pre=0x01C;
const int Reg_Time4_Post=0x01E;
const int Reg_ADC1_Gain=0x020;
const int Reg_ADC1_IntOff=0x022;
const int Reg_ADC1_BaseMa=0x024;
const int Reg_ADC1_BaseMi=0x026;
const int Reg_ADC1_SpareA=0x028;
const int Reg_ADC1_SpareB=0x02A;
const int Reg_ADC2_Gain=0x02C;
const int Reg_ADC2_IntOff=0x02E;
const int Reg_ADC2_BaseMax=0x030;
const int Reg_ADC2_BaseMin=0x032;
const int Reg_ADC2_SpareA=0x034;
const int Reg_ADC2_SpareB=0x036;
const int Reg_ADC3_Gain=0x038;
const int Reg_ADC3_IntOff=0x03A;
const int Reg_ADC3_BaseMax=0x03C;
const int Reg_ADC3_BaseMin=0x03E;
const int Reg_ADC3_SpareA=0x040;
const int Reg_ADC3_SpareB=0x042;
const int Reg_ADC4_Gain=0x044;
const int Reg_ADC4_IntOff=0x046;
const int Reg_ADC4_BaseMax=0x048;
const int Reg_ADC4_BaseMin=0x04A;
const int Reg_ADC4_SpareA=0x04C;
const int Reg_ADC4_SpareB=0x04E;
const int Reg_Trig1_ThresA=0x050;
const int Reg_Trig1_ThresB=0x052;
const int Reg_Trig1_Times=0x054;
const int Reg_Trig1_Tmax=0x056;
const int Reg_Trig1_Nmin=0x058;
const int Reg_Trig1_Qmin=0x05A;
const int Reg_Trig2_ThresA=0x05C;
const int Reg_Trig2_ThresB=0x05E;
const int Reg_Trig2_Times=0x060;
const int Reg_Trig2_Tmax=0x062;
const int Reg_Trig2_Nmin=0x064;
const int Reg_Trig2_Qmin=0x066;
const int Reg_Trig3_ThresA=0x068;
const int Reg_Trig3_ThresB=0x06A;
const int Reg_Trig3_Times=0x06C;
const int Reg_Trig3_Tmax=0x06E;
const int Reg_Trig3_Nmin=0x070;
const int Reg_Trig3_Qmin=0x072;
const int Reg_Trig4_ThresA=0x074;
const int Reg_Trig4_ThresB=0x076;
const int Reg_Trig4_Times=0x078;
const int Reg_Trig4_Tmax=0x07A;
const int Reg_Trig4_Nmin=0x07C;
const int Reg_Trig4_Qmin=0x07E;
const int Reg_FltA1_A1=0x080;
const int Reg_FltA1_A2=0x082;
const int Reg_FltA1_B1=0x084;
const int Reg_FltA1_B2=0x086;
const int Reg_FltA1_B3=0x088;
const int Reg_FltA1_B4=0x08A;
const int Reg_FltA1_B5=0x08C;
const int Reg_FltA1_B6=0x08E;
const int Reg_FWStatus=0x1C0;
const int Reg_GenStatus=0x1D0;
const int Reg_GenControl=0x1D4;
const int Reg_Data=0x1D8;
const int Reg_TestTrace=0x1DC;
const int Reg_Rate=0x1E0;
const int Reg_End=0x1FC;
/* Message definitions  Legacy*/

const int ID_PARAM_PPS=0xC4;
const int ID_PARAM_EVENT=0xC0;

const int ID_GPS_VERSION=4;

/*----------------------------------------------------------------------*/
/* Control register bits */
const int CTRL_SEND_EN=(1 << 0);
const int CTRL_PPS_EN=(1 << 1);
const int CTRL_FLTR_EN=(1 << 3);
const int CTRL_FAKE_ADC=(1 << 6);
const int CTRL_FILTER1=(1 <<  8);
const int CTRL_FILTER2=(1 <<  9);
const int CTRL_FILTER3=(1 << 10);
const int CTRL_FILTER4=(1 << 11);
const int CTRL_AUTOBOOT=(1 << 15);

const int GENSTAT_PPSFIFO=(1<<24);
const int GENSTAT_EVTFIFO=(1<<25);
const int GENSTAT_DMAFIFO=(1<<26);

const int TRIG_10SEC=(1 << 5);
const int TRIG_CAL=(1 << 6);

// general event types
const int SELF_TRIGGERED=0x0001;
const int CALIB_TRIGGER=0x0004;
const int EXT_T3_TRIGGER=0x0008;
const int RANDOM_TRIGGER=0x0010;
const int TRIGGER_T3_MINBIAS=0x1000;
const int TRIGGER_T3_RANDOM=0x8000;

/*----------------------------------------------------------------------*/
/* PPS definition */
const int MAGIC_PPS=0xFACE;
const int WCNT_PPS=32;
const int PPS_MAGIC=1;
const int PPS_TRIG_PAT=2;
const int PPS_TRIG_RATE=3;
const int PPS_CTD=4;
const int PPS_CTP=6;
const int PPS_OFFSET=8;
const int PPS_LEAP=10;
const int PPS_STATFLAG=11;
const int PPS_CRITICAL=12;
const int PPS_WARNING=13;
const int PPS_YEAR=14;
const int PPS_DAYMONTH=15;
const int PPS_MINHOUR=16;
const int PPS_STATSEC=17;
const int PPS_LONGITUDE=18;
const int PPS_LATITUDE=22;
const int PPS_ALTITUDE=26;
const int PPS_TEMPERATURE=30;

/*----------------------------------------------------------------------*/
/* Event definition */
const int MAGIC_EVT=0xADC0;
const int HEADER_EVT=256;
const int FORMAT_EVT=1;
const int EVT_LENGTH=0; // nr of int16 words
const int EVT_ID=1; // nr of int16 words
const int EVT_HARDWARE=2;
const int EVT_HDRLEN=3; //256 (int16 words in the header)
const int EVT_SECOND=4;
const int EVT_NANOSEC=6;
const int EVT_TRIGGERPOS=8;
const int EVT_T3FLAG=9;
const int EVT_ATM_TEMP=17;
const int EVT_ATM_PRES=18;
const int EVT_ATM_HUM=19;
const int EVT_ACCEL_X=20;
const int EVT_ACCEL_Y=21;
const int EVT_ACCEL_Z=22;
const int EVT_BATTERY=23;
const int EVT_VERSION=24;
const int EVT_MSPS=25;
const int EVT_ADC_RES=26;
const int EVT_INP_SELECT=27;
const int EVT_CH_ENABLE=28;
const int EVT_TOT_SAMPLES=29;
const int EVT_CH1_SAMPLES=30;
const int EVT_CH2_SAMPLES=31;
const int EVT_CH3_SAMPLES=32;
const int EVT_CH4_SAMPLES=33;
const int EVT_TRIG_PAT=34;
const int EVT_TRIG_RATE=35;
const int EVT_CTD=36;
const int EVT_CTP=38;
const int EVT_PPS_OFFSET=40;
const int EVT_LEAP = 42;
const int EVT_GPS_STATFLAG=43;
const int EVT_GPS_CRITICAL=44;
const int EVT_GPS_WARNING=45;
const int EVT_YEAR=46;
const int EVT_DAYMONTH=47;
const int EVT_MINHOUR=48;
const int EVT_STATSEC=49;
const int EVT_LONGITUDE=50;
const int EVT_LATITUDE=54;
const int EVT_ALTITUDE=58;
const int EVT_GPS_TEMP=62;
const int EVT_CTRL=64;
const int EVT_WINDOWS=72;
const int EVT_CHANNEL=80;
const int EVT_TRIGGER=104;
const int EVT_FILTER1=128;
const int EVT_FILTER2=160;
const int EVT_FILTER3=192;
const int EVT_FILTER4=224;

const int EVENT_BCNT=2; //bytecount
const int EVENT_TRIGMASK=4;
const int EVENT_GPS=6;
const int EVENT_STATUS=13;
const int EVENT_CTD=14;
const int EVENT_LENCH1=18;
const int EVENT_LENCH2=20;
const int EVENT_LENCH3=22;
const int EVENT_LENCH4=24;
const int EVENT_THRES1CH1=26;
const int EVENT_THRES2CH1=28;
const int EVENT_THRES1CH2=30;
const int EVENT_THRES2CH2=32;
const int EVENT_THRES1CH3=34;
const int EVENT_THRES2CH3=36;
const int EVENT_THRES1CH4=38;
const int EVENT_THRES2CH4=40;
const int EVENT_CTRL=42;
const int EVENT_WINDOWS=54;
const int EVENT_ADC=70;

/*----------------------------------------------------------------------*/
/* Error Definition */
const int ERROR_BCNT=2;
const int ERROR_ID=4;
const int ERROR_END=6;

/*----------------------------------------------------------------------*/
/* Calibration states */
const int CAL_END=0;
const int CAL_OFFSET=1;
const int CAL_GAIN=2;

/* Calibration targets */
const int CAL_OFFSET_TARG=0;
const int CAL_OFFSET_WIDTH=2;

const int CAL_OFFSET_OTARG=0;
const int CAL_OFFSET_OWIDTH=2;

const int CAL_GAIN_TARG=-7250;
const int CAL_GAIN_WIDTH=2;

const int CAL_GAIN_OTARG=-7000;
const int CAL_GAIN_OWIDTH=12;

/*----------------------------------------------------------------------*/
/* Trigger rate divider base frequency */
const int TRIG_RATE_BASE_HZ=4800;  //!< maximal fpga generated trigger frequency

/*----------------------------------------------------------------------*/
/* Macros */
#define FIRMWARE_VERSION(x) (10*((x>>20)&0xf)+((x>>16)&0xf)) //!< Calculation of Firmware version number
#define FIRMWARE_SUBVERSION(x) (10*((x>>12)&0xf)+((x>>9)&0x7)) //!< Calculation of subversion number
#define SERIAL_NUMBER(x) (100*((x>>8)&0x1)+10*((x>>4)&0xf)+((x>>0)&0xf)) //!< serial number of digital board
/*
  buffer definitions for the scope readout process.
 */
const int DEV_READ_BLOCK=100;      //!< fpga Device read blocksize, in Bytes

const int MAX_RATE=1000;            //!< maximum event rate, in Hz
#ifdef Fake
const int BUFSIZE=3            //!< store up to 10 events in circular buffer;
#else
const int BUFSIZE=3000;            //!< store up to 3000 events in circular buffer
#endif
const int GPSSIZE=35;              //!< buffer upto 35 GPS seconds info in circular buffer
const int MAXT3=200;               //!< 200 T3 events in circular cuffer

// next: what did we read from the scope?

const int SCOPE_PARAM=1;          //!< return code for reading a parameter list
const int SCOPE_EVENT=2;          //!< return code for reading an event
const int SCOPE_GPS=3;          //!< return code for reading a PPS message

const int PARAM_NUM_LIST=0x18;     //!< Number of parameter lists for the fpga
const int PARAM_LIST_MAXSIZE=46;   //!< maximal listsize 46 bytes


	typedef struct
	{
		uint16_t event_nr;        //!< an event number
		uint32_t ts_seconds;      //!< second marker
		uint32_t t3calc;          //!< was the T3 time calculated (1/0)
		uint32_t t3_nanoseconds;  //!< proper timing
		uint32_t t2_nanoseconds;  //!< rough timing for t2 purposes only
		uint32_t CTD;             //!< clock tick of the trigger
		uint32_t trig_flag;       //!< trigger flag
		uint32_t evsize;          //!< size of the event
		float quant1;             //!< quant error previous PPS
		float quant2;             //!< quant error next PPS
		uint32_t CTP;             //!< Number of clock ticks between PPS pulses
		int16_t sync;             //!< Positive or Negative clock edge
		uint8_t buf[MAX_READOUT]; //!< raw data buffer
	} EV_DATA;

	typedef struct
	{
		uint32_t ts_seconds;      //!< time marker in GPS sec
		uint16_t data[WCNT_PPS];  //! all data read in PPS
	} GPS_DATA;


// the routines

	void scope_set_parameters(uint32_t reg_addr, uint32_t value, uint32_t to_shadow);

	void scope_raw_write(uint32_t reg_addr, uint32_t value);

	int32_t scope_raw_read(uint32_t reg_addr, uint32_t *value);

	int scope_open();

	void scope_get_parameterlist(uint8_t list);

	void scope_reset();

	void scope_start_run();

	void scope_stop_run();

	void scope_reboot();

	void scope_print_parameters(int32_t list);

	void scope_print_pps(uint8_t *buf);

	void scope_print_event(uint8_t *buf);

	void scope_initialize();

	void scope_init_shadow();

	void scope_fill_shadow(int8_t *ppsbuf);

	int8_t *scope_get_shadow(int32_t list);

	int scope_read(int ioff);

	int scope_read_pps();

	int scope_read_event(int32_t ioff);

	int scope_read_error();

	int scope_no_run_read();

	int scope_run_read();

	int scope_cal_read();

	int scope_calc_t3nsec(EV_DATA *buf);

	int scope_calc_evnsec();

	void scope_calibrate();

	void scope_initialize_calibration();

	int scope_calibrate_evt();

	void scope_close();

	void scope_create_memory();

	void scope_copy_shadow();
//
}
#endif
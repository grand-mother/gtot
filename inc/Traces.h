#ifndef GTOT_TRACES_H
#define GTOT_TRACES_H


/* First define the header words */
const int FILE_HDR_LENGTH=0;
const int FILE_HDR_RUNNR=1;
const int FILE_HDR_RUN_MODE=2;
const int FILE_HDR_SERIAL=3;
const int FILE_HDR_FIRST_EVENT=4;
const int FILE_HDR_FIRST_EVENT_SEC=5;
const int FILE_HDR_LAST_EVENT=6;
const int FILE_HDR_LAST_EVENT_SEC=7;
const int FILE_HDR_ADDITIONAL=8; //start of additional info to be defined

const int EVENT_HDR_LENGTH=0;
const int EVENT_HDR_RUNNR=2;
const int EVENT_HDR_EVENTNR=4;
const int EVENT_HDR_T3EVENTNR=6;
const int EVENT_HDR_FIRST_DU=8;
const int EVENT_HDR_EVENT_SEC=10;
const int EVENT_HDR_EVENT_NSEC=12;
const int EVENT_HDR_EVENT_TYPE=14;
const int EVENT_HDR_EVENT_VERS=15;
const int EVENT_HDR_AD1=16; //start of additional info to be defined
const int EVENT_HDR_AD2=18; //                    info to be defined
const int EVENT_HDR_AD3=20; //                    info to be defined
const int EVENT_DU=22;

const int INTSIZE=4;
const int SHORTSIZE=2;

#endif //GTOT_TRACES_H

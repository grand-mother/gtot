#ifndef GTOT_TRACES_H
#define GTOT_TRACES_H


/* First define the header words */
#define FILE_HDR_LENGTH          0
#define FILE_HDR_RUNNR           1
#define FILE_HDR_RUN_MODE        2
#define FILE_HDR_SERIAL          3
#define FILE_HDR_FIRST_EVENT     4
#define FILE_HDR_FIRST_EVENT_SEC 5
#define FILE_HDR_LAST_EVENT      6
#define FILE_HDR_LAST_EVENT_SEC  7
#define FILE_HDR_ADDITIONAL      8 //start of additional info to be defined

#define EVENT_HDR_LENGTH          0
#define EVENT_HDR_RUNNR           2
#define EVENT_HDR_EVENTNR         4
#define EVENT_HDR_T3EVENTNR       6
#define EVENT_HDR_FIRST_DU        8
#define EVENT_HDR_EVENT_SEC      10
#define EVENT_HDR_EVENT_NSEC     12
#define EVENT_HDR_EVENT_TYPE     14
#define EVENT_HDR_EVENT_VERS     15
#define EVENT_HDR_AD1            16 //start of additional info to be defined
#define EVENT_HDR_AD2            18 //                    info to be defined
#define EVENT_HDR_AD3            20 //                    info to be defined
#define EVENT_DU                 22

#define INTSIZE 4
#define SHORTSIZE 2

#endif //GTOT_TRACES_H

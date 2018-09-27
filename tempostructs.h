#ifndef TEMPOSTRUCTS_H
#define TEMPOSTRUCTS_H
#include <stdint.h>

namespace icd{

#define ICD_PREMBLE  0x9a5bc0dd
#define USB_MAX_MSG_SIZE 64

#define ICD_HEADER_SIZE 8

#define USB_REQ_MASTER_SEND 0
#define USB_REQ_MASTER_RECEIVE 1

#define USB_MSG_TYPE_STATUS 				0x01
#define USB_MSG_TYPE_REPORT_RECORDING 		0x02
#define USB_MSG_TYPE_GET_RECORDING			0x03
#define USB_MSG_TYPE_START_UPDATE           0x04
#define USB_MSG_TYPE_START_COMM             0x05




typedef struct __attribute__ ((packed))
{
    uint32_t Signature;
    uint8_t nRecordnum;
    uint8_t	bLastRecording;
    uint8_t eTimeDiff;
    uint8_t rsvd8;
    uint16_t nNumOfSamples;
    uint16_t nSectorStart;
    uint8_t rsvd[4];
}flash_file_header_type;


typedef struct __attribute__ ((packed))
{
    uint8_t nPremble[4];
    uint8_t nSequence;
    uint8_t nReq;
    uint8_t nMsgtype;
    uint8_t nMsglen;
}icd_header;



typedef struct __attribute__ ((packed))
{
    icd_header sHeader;
    uint8_t nMsgdata[USB_MAX_MSG_SIZE];

}icd_template;







typedef struct __attribute__ ((packed)){
    icd_header sHeader;
    uint16_t nBattPercent;
    uint16_t nTemp;

}icd_status_msg_type;

typedef struct {
    icd_header sHeader;
    flash_file_header_type sRecHeader;
    uint8_t nTotalRecs;

}icd_get_recording_type;
typedef struct{
    icd_header sHeader;
    uint8_t nIdx;


}icd_req_recording_type;


const uint8_t timediffs[] = {1,5,10,30,60};


}


#endif // TEMPOSTRUCTS_H

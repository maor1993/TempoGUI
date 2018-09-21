#ifndef TEMPOSTRUCTS_H
#define TEMPOSTRUCTS_H
#include <stdint.h>

namespace icd{

#define ICD_PREMBLE  0xa5a5
#define USB_MAX_MSG_SIZE 64

#define ICD_HEADER_SIZE 6

#define USB_REQ_MASTER_SEND 0
#define USB_REQ_MASTER_RECEIVE 1

#define USB_MSG_TYPE_STATUS 				0x01
#define USB_MSG_TYPE_REPORT_RECORDING 		0x02
#define USB_MSG_TYPE_GET_RECORDING			0x03


typedef struct __attribute__ ((packed))
{
    uint16_t nPremble;
    uint8_t nSequence;
    uint8_t nReq;
    uint8_t nMsglen;
    uint8_t nMsgtype;

}icd_header;



typedef struct __attribute__ ((packed))
{
    icd_header sHeader;
    uint8_t nMsgdata[USB_MAX_MSG_SIZE];

}icd_template;

//align only body!




typedef struct __attribute__ ((packed)){
    icd_header sHeader;
    uint16_t nBattPercent;
    uint16_t nTemp;

}icd_status_msg_type;




}


#endif // TEMPOSTRUCTS_H

/**
 * @file pdu.h
 * @author Mobin Byn
 * @gmail Mobin.byn
 * @brief Encode/Decode PDU data
 * @version 0.1.1
 * @date 2023-02-15
 *
 * @copyright Copyright (c) 2023
 *
 * @release
 * 0.1.1    Original release
 *
 */

#ifndef PDU_H_
#define PDU_H_


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdarg.h>

/* uncomment if you need add ctrl+z and end marker to end of the packet */
//#define CTRL_Z
//#define PM   // uncomment to implement Arduino PROGMEM feature



#define BITMASK_7BITS 0x7F
// DCS bit masks
#define DCS_COMPRESSED (5<<1)
#define DCS_CLASS_MEANING (4<<1)
#define DCS_ALPHABET_MASK (3<<2)
#define DCS_ALPHABET_OFFSET 2
#define DCS_7BIT_ALPHABET_MASK  0x0 //(0<<2)
#define DCS_8BIT_ALPHABET_MASK  0x4 //1<<2)
#define DCS_16BIT_ALPHABET_MASK 0x8 //(2<<2)
#define DCS_CLASS_MASK 3
#define DCS_IMMEDIATE_DISPLAY 3
#define DCS_ME_SPECIFIC_MASK 1
#define DCS_SIM_SPECIFIC_MASK 2
#define DCS_TE_SPECIFIC_MASK 3


#define PDU_BUFFER_LENGTH 360
#define MAX_SMS_LENGTH_7BIT 160 // GSM 3.4
#define MAX_NUMBER_OCTETS 140
#define MAX_NUMBER_LENGTH 20    // gets packed into BCD or packed 7 bit
#define UTF8_BUFF_SIZE 150   // tailor to what you need

//SCA (12) + type + mRef + address(12) + pid + dcs + vp(7) + length + data(140)
#define PDU_BINARY_MAX_LENGTH 180

// type of address
//#define INTERNATIONAL_NUMBER 0x91
//#define NATIONAL_NUMBER 0xA1

#define EXT_MASK 0x80   // bit 7
#define TON_MASK 0x70   // bits 4-6
#define TON_OFFSET 4
#define NPI_MASK 0x0f   // bits 0-3

/* Define Non-Printable Characters as a question mark */
#define NPC7    63
#define NPC8    '?'

#define PDU_VALIDITY_NOT_PRESENT 0
#define PDU_VALIDITY_PRESENT_ENHANCED 1
#define PDU_VALIDITY_PRESENT_RELATIVE 2
#define PDU_VALIDITY_PRESENT_ABSOLUTE 3

/**
 * @defgroup Message Type Indicator (MTI)
 *
 *              Bit 1 Bit 0 	Direction SC->MS 		Direction MS->SC
 *                 0    0 		SMS-DELIVER 				SMS-DELIVER REPORT
 *                 1 		0 		SMS-STATUS REPORT 	SMS-COMMAND
 *	               0 		1 		SMS-SUBMIT REPORT 	SMS-SUBMIT
 *	               1 		1 		RESERVED
 *
 */
#define MTI_SMS_DELIVER 							0x00	// SMSC -> MS
#define MTI_SMS_DELIVER_REPORT				0x00	// MS -> SMSC
#define MTI_SMS_SUBMIT								0x01	// MS -> SMSC
#define MTI_SMS_SUBMIT_REPORT					0x01	// SMSC -> MS
#define MTI_SMS_STATUS_REPORT					0x02	// SMSC -> MS
#define MTI_SMS_COMMAND								0x02	// MS -> SMSC


#define PID_SHORT_MESSAGE							0x00
#define PID_SHORT_MESSAGE_TYPE1				0x41
#define PID_SHORT_MESSAGE_TYPE2				0x42
#define PID_SHORT_MESSAGE_TYPE3				0x43
#define PID_SHORT_MESSAGE_TYPE4				0x44
#define PID_SHORT_MESSAGE_TYPE5				0x45
#define PID_SHORT_MESSAGE_TYPE6				0x46
#define PID_SHORT_MESSAGE_TYPE7				0x47


/** @brief SCA is in octets, sender/recipient nibbles */
typedef enum
{
    OCTET,
    NIBBLES
}PDU_NumberLengthType_t;

typedef enum
{
    OBSOLETE_ERROR = -1,
    UCS2_TOO_LONG = -2,
    GSM7_TOO_LONG = -3,
    MULTIPART_NUMBERS = -4,
    ADDRESS_FORMAT=-5,
    WORK_BUFFER_TOO_SMALL=-6,
    ALPHABET_8BIT_NOT_SUPPORTED = -7
}PDU_EncodeError_t;

typedef enum
{
    UNKNOWN_NUMBER = 0x80,
    INTERNATIONAL_NUMBER = 0x91,
    NATIONAL_NUMBER = 0x81,
    ALPHABETIC_NUMBER = 0xD0
}PDU_NumberFormat_t;


typedef enum
{
    DCS_7BIT 	= 0x00,
    DCS_8BIT	= 0x04,
    DCS_16BIT = 0x08
}PDU_DataCodingScheme_t;

typedef enum
{
    VPF_NOT_PRESENT	=							PDU_VALIDITY_NOT_PRESENT,
    VPF_PRESENT_ENHANCED =        PDU_VALIDITY_PRESENT_ENHANCED,
    VPF_PRESENT_RELATIVE = 				PDU_VALIDITY_PRESENT_RELATIVE,
    VPF_PRESENT_ABSOLUTE =				PDU_VALIDITY_PRESENT_ABSOLUTE
}PDU_VPF_t;

#define MTI_MMS_BIT 2
#define VPF_BIT 3
#define SRI_BIT 5
#define UDHI_BIT 6
#define RP_BIT 7

typedef union
{
    struct{
        uint8_t MTI: 2;				// Message Type Indicator: Parameter describing the message type 00 for SMS-DELIVER
        uint8_t MMS : 1;			// More Message to Send: Parameter indicating whether there are more message to send
        uint8_t BIT_4_3 : 2;	// Bits 3 and 4 are not used and are set to 0
        uint8_t SRI : 1;			// Status Report Indication: Parameter indicating if the SME has requested a status report
        uint8_t UDHI : 1;			// User Data Header Indicator: Parameter indicating that the UD field contains a submit_header
        uint8_t RP : 1; 			// Reply Path: Parameter indicating that replay path exists
    }deliver;
    uint8_t allfields;			// PDU-Type octet
}PDU_Type_t;



typedef struct
{
    char 										SCA[MAX_NUMBER_LENGTH]; // Service Center Address: Telephone number of the Service Center

    PDU_Type_t				      PDU_TYPE;								// Protocol Data Unit Type

    PDU_NumberFormat_t      numberFormat;           // Number Format
    int                     OA_Len;                 // Originator Address length
    char										OA[MAX_NUMBER_LENGTH];	// Originator Address

    PDU_DataCodingScheme_t 	DCS;										// Data Coding Scheme: Parameter identifying the coding scheme within the User Data(UD)

    char 										SCTS[15];								// Service Center Time Stamp: Parameter identifying time when the SMSC received the message

    uint8_t									UDL;										// User Data Length: Parameter indicating the length of the UD-field
    char                    UDH[30];                // User Data Header

    /* allocate dynamically */
    //char										*UD;					          // User Data: Data of the SM
}PDU_DeliverHeader_t;
extern PDU_DeliverHeader_t deliverHeader;

void PDU_Init(int);
char *PDU_getPDUBuffer();

/**
 * @brief Encode a PDU block for sending to an GSM modem
 *
 * @param SCA
 * @param ReplayPath
 * @param headerPresent
 * @param statusReport
 * @param VPF_Format
 * @param messageReference
 * @param recipient Phone number, must be numeric, no whitespace. International numbers prefixed by '+'
 * @param DCS
 * @param VPF_Value
 * @param message
 * @param items
 * @param ...
 * @return int The length of the message, need for the GSM command <b>AT+CSMG=nn</b>
*/
int PDU_encode(const char *SCA, bool ReplayPath, bool headerPresent, bool statusReport, PDU_VPF_t VPF_Format, int messageReference, const char *recipient, /*PDU_DataCodingScheme_t DCS,*/ int VPF_Value, const char *message, int items, ...);



/**
 * @brief Decode a PDU, typically received from a GSM modem when in PDU mode.
 * After a successful decoding you can retrieve the components parts, described below.
 *
 * @param pdu A pointer to the PDU
 * @return true If the decoding succeeded.
 * @return false If the decoding did not succeed.
 */
bool PDU_decode(const char *pdu);


/**
 * @brief  The total bytes of PDU message is excluded the SCA address field expressed in decimal value.
 * @return The length of the message
 */
int PDU_getPDUCodeSize(void);



extern const
#ifdef PM
    PROGMEM
#endif
short lookup_ascii8to7[];

extern const
#ifdef PM
    PROGMEM
#endif
unsigned char lookup_gsm7toUnicode[];

extern const
#ifdef PM
    PROGMEM
#endif
unsigned short lookup_Greek7ToUnicode[];

#define GREEK_UCS_MINIMUM 0x393
extern const
#ifdef PM
    PROGMEM
#endif
unsigned short lookup_UnicodeToGreek7[];

typedef struct  {
    unsigned short min,max;
}sRange;

extern const
#ifdef PM
PROGMEM
#endif
sRange gsm7_legal[];

#endif //PDU_H_

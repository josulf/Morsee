#ifndef SIEMENSTCP_APP_H_
#define SIEMENSTCP_APP_H_

#include <uip/uipopt.h>
#include <stdint.h>

/************************************************/
/* Constants									*/
/************************************************/
#define	SIEMENS_TCP_PORT	102		/* KepServer */
//#define	SIEMENS_TCP_PORT	2000	/* PLC - ISO on TCP */

/************************************************/
/* Type definitions								*/
/************************************************/
typedef enum _siemensTCP_state_t {
	MSG_CR_CC, MSG_INI_DT, MSG_DT, MSG_DT_RD, MSG_DT_WR, MSG_UNK 
} siemensTCP_state_t;

#define	_PACK	__attribute__ ((packed))  

/////////////////////////////////////////////////////////
// NOTES:
// * All uin16_t values in network endiannes!!
// * Write is considered a data transfer from host to PLC

// ISO 8073 COTP, Connection Oriented Transport Protocol : Connection Request & Confirm
typedef struct _cotp_cr_t {
	_PACK uint8_t		len;		// sizeof(this) - 1 = 17
	_PACK uint8_t		type;		// 0xE0 (CR: Connect Request)
	_PACK uint16_t		dest_ref;	// 0x0000
	_PACK uint16_t		src_ref;	// 0x0001
	_PACK uint8_t		flags;		// 0x00 (Class: bits 7-4, ext.format: bit 1, Explicit flow: bit 0)
	_PACK uint8_t		param_code1;// 0xC0 (tpd-size)
	_PACK uint8_t		param_len1;	// 1
	_PACK uint8_t		tpdu_size;	// 9 -> TPDUSize = (1 << tpdu_size)
	_PACK uint8_t		param_code2;// 0xC1 (src-tsap)
	_PACK uint8_t		param_len2;	// 2
	_PACK uint16_t		src_tsap;	// 0x4B54 : KT
	_PACK uint8_t		param_code3;// 0xC2 (dst-tsap)
	_PACK uint8_t		param_len3;	// 2
	_PACK uint16_t		dst_tsap;	// 0x0302	
} COTP_CR_t;

typedef struct _siemensTCP_CR_CC_t {
	_PACK uint8_t		version;	// 3
	_PACK uint8_t		reserved;	// 0
	_PACK uint16_t		length;		// 22 bytes: size to frame end in bytes
	_PACK COTP_CR_t		cotp;		// type(resp) = 0x0D (CC: Connect Confirm)
									// dest_ref(resp) = src_ref(req)
									// src_ref(resp) = 0x4431
} siemensTCP_CR_CC_t;

// ISO 8073 COTP, Connection Oriented Transport Protocol : Initial Data Request/Response
typedef struct _ini_dt_req_t  {
	_PACK uint16_t		request;	// 0x3201
	_PACK uint16_t		zero;		// 0x0000
	_PACK uint16_t		timestamp;	// copy to response
	_PACK uint16_t		frame_type;	// 0x0008
	_PACK uint16_t		unk_codes[5];// copy to response
} ini_dt_req_t;

typedef struct _ini_dt_resp_t {
	_PACK uint16_t		response;	// 0x3203 
	_PACK uint16_t		zero;		// 0x0000
	_PACK uint16_t		timestamp;	// copy from request
	_PACK uint16_t		frame_type;	// 0x0008
	_PACK uint16_t		unk_new;	// 0x0000
	_PACK uint16_t		unk_codes[5];// copy from request
} ini_dt_resp_t;

typedef struct _cotp_dt_t {
	_PACK uint8_t		len;		// 2 = sizeof(this)-1
	_PACK uint8_t		type;		// 0xF0 , COTP DT data 
	_PACK uint8_t		flags;		// 0x80 (bit 8: Last unit, bit 7-0: TPDU number)
} COTP_DT_t;

typedef struct _siemensTCP_IniDT_t {
	_PACK uint8_t		version;	// 3
	_PACK uint8_t		reserved;	// 0
	_PACK uint16_t		length;		// req = 25 / resp = 27 bytes (size to frame end in bytes)
	_PACK COTP_DT_t		cotp;
	_PACK union {
		_PACK ini_dt_req_t	req;
		_PACK ini_dt_resp_t	resp;
	};
} siemensTCP_iniDT_t;

// ISO 8073 COTP, Connection Oriented Transport Protocol: Data Request
typedef struct _wr_data_t {
	_PACK uint16_t		mask;		// only when writing bits (otherwise 0x0004)
	_PACK uint16_t		data_bits;	// number of bits in data type ( 0x0001 = 1 bit / 0x0010 = 16 bits )  
	_PACK uint8_t		data[1];	// minimun 1 byte of data to write
} wr_data_t;

typedef struct _dt_req_grp_t {
	_PACK uint16_t		magic1;		// 0x120A
	_PACK uint16_t		magic2;		// 0x1002 / 0x1001 (when writing bits)
	_PACK uint16_t		req_bytes;	// Number of bytes requested to read or write
	_PACK uint16_t		reserved;	// 0x0000
	_PACK uint16_t		data_type;	// 0x8100=E / 0x8200=A / 0x8300=M
	_PACK uint16_t		address;	// (real_address << 3) or (real_address << 3 | bit_address)  
} dt_req_grp_t;

typedef struct _dt_req_t {
	_PACK uint16_t		request;	// 0x3201
	_PACK uint16_t		zero;		// 0x0000
	_PACK uint16_t		timestamp;	// copy to response
	_PACK uint16_t		frame_type;	// 0x000E (1 data group) / 0x001A (multiple data group)
	_PACK uint16_t		wr_bytes;	// 0x0000 or >=0x0005 when writing
	_PACK uint8_t		rw;			// 0x04 = Read / 0x05 = Write
	_PACK uint8_t		num_grp;	// number of groups (minimun 1)
	_PACK dt_req_grp_t	grp[1];		// groups of data to write or read (at least 1)
	_PACK wr_data_t		wr_data[0];	// Data to write (optional, size given by wr_bytes in the following frame part)
} dt_req_t;

// ISO 8073 COTP, Connection Oriented Transport Protocol: Data Request -> Response
typedef struct _dt_resp_grp_t {
	_PACK uint16_t		magic1;		// HTONS(0xFF04)
	_PACK uint16_t		data_bits;	// number of bits in data
	_PACK uint8_t		data[1];	// data bytes (at least 1 if frame present)
} dt_resp_grp_t;

typedef struct _dt_resp_t {
	_PACK uint16_t		response;	// 0x3203
	_PACK uint16_t		zero;		// 0x0000
	_PACK uint16_t		timestamp;	// copy from request
	_PACK uint16_t		frame_type;	// 0x0002
	_PACK uint16_t		data_bytes;	// Total Data Bytes
	_PACK uint16_t		wr_bytes;	// 0x0000 (not used in responses)
	_PACK uint8_t		rw;			// 0x04 = Read / 0x05 = Write
	_PACK uint8_t		num_grp;	// number of groups (minimun 0)
	_PACK union {
		_PACK uint8_t		write_end[1];	// 0xFF;
		_PACK dt_resp_grp_t	grp[0];
	};
} dt_resp_t;

typedef struct _siemensTCP_DT_t {
	_PACK uint8_t		version;	// 3
	_PACK uint8_t		reserved;	// 0
	_PACK uint16_t		length;
	_PACK COTP_DT_t		cotp;
	_PACK union {
		_PACK dt_req_t	req;
		_PACK dt_resp_t	resp;
	};
} siemensTCP_DT_t;

// PLC Memory request information
typedef struct _req_info_t {
	uint8_t		data[50];
	int16_t		num_bytes;
	uint8_t		is_write;
	uint16_t	addr;
	uint8_t		bit;
} req_info_t;

typedef struct _req_transfer_data_t {
	uint8_t*	data;			/* Pointer to PLC data values */
	uint16_t	address;		/* Starting PLC address of requested/received data */
	uint16_t	num_bytes;		/* Number of bytes requested/received */
	uint8_t		num_bit;		/* Number of bits received (used only on reception) */
} req_transfer_data_t;

/************************************************/
/* Global vars									*/
/************************************************/
extern char 				siemensTCP_msg[21];				 
extern siemensTCP_state_t	siemensTCP_state;

/************************************************/
/* Prototypes									*/
/************************************************/
void siemensTCP_app_init(void);
void siemensTCP_appcall(void);

/* Interface functions */
void	PLC_ReceiveMemoryData	(req_transfer_data_t* tr);
void	PLC_SendMemoryData		(req_transfer_data_t* tr);

#endif /*SIEMENSTCP_APP_H_*/


#include <string.h>
#include <uip/uip.h>
#include "siemensTCP_app.h"
#include "network_state.h"

/************************************************/
/* Local constants								*/
/************************************************/

/************************************************/
/* Global variables								*/
/************************************************/
siemensTCP_state_t	siemensTCP_state = MSG_UNK;
char 				siemensTCP_msg[21] = "Wait.Conn.";
frame_stats_t		siemens_stats;
req_info_t			Request;

/************************************************/
/* Prototypes									*/
/************************************************/
int process_siemensTCP_frame(void);

/************************************************/
/* Implementations								*/
/************************************************/
void siemensTCP_app_init(void)
{
	// Listen to port 102
	uip_listen( HTONS( SIEMENS_TCP_PORT ) );	// stap service
}

void siemensTCP_appcall(void)
{
	if( uip_conn->lport != HTONS( SIEMENS_TCP_PORT ) ) return;
	siemens_stats.frames++;
    // We use the uip_ test functions to deduce why we were
    // called. If uip_connected() is non-zero, we were called
    // because a remote host has connected to us. If
    // uip_newdata() is non-zero, we were called because the
    // remote host has sent us new data, and if uip_acked() is
    // non-zero, the remote host has acknowledged the data we
    // previously sent to it. */
    
    if(uip_connected()) {
    }
	if(uip_newdata()) {
    	siemens_stats.recv++;
    	int len = process_siemensTCP_frame();
    	if( len > 0 ) {  
        	siemens_stats.sent++;
    		uip_send( uip_appdata, len ); return;
    	}else{
    		uip_abort();
    	}
    }
    if(uip_acked()) {
    }
    // Finally, return to uIP. Our outgoing packet will soon be on its way...
    return;
}

int process_siemensTCP_frame(void)
{
	int i;
	siemensTCP_DT_t *dt = (siemensTCP_DT_t *)(uip_appdata);
	
	if( dt->version != 3 || dt->reserved != 0 ) {
		strcpy(siemensTCP_msg, "Err<INCOR>"); return -1;
	}
	if( dt->length == HTONS(22) && dt->cotp.type == 0xE0 ) siemensTCP_state = MSG_CR_CC;
	else if(	dt->cotp.type == 0xF0 && dt->req.request == HTONS(0x3201) ) {
		if( dt->req.frame_type == HTONS(8) ) siemensTCP_state = MSG_INI_DT;  
		else								 siemensTCP_state = MSG_DT;  
	}else{
		strcpy(siemensTCP_msg, "Err<Unkwn>"); return -1;	// unidentified frame
	} 

	switch( siemensTCP_state )
	{
	case MSG_CR_CC: 
		{
			siemensTCP_CR_CC_t *cdt = (siemensTCP_CR_CC_t *)(uip_appdata);
			if( cdt->length != HTONS(22) || cdt->cotp.type != 0xE0 ) {
				strcpy(siemensTCP_msg, "Err<CR_CC>"); uip_abort(); return -1;
			}
			strcpy(siemensTCP_msg, "OK<CR_CC> ");
			cdt->cotp.type = 0xD0;
			cdt->cotp.dest_ref = cdt->cotp.src_ref;
			cdt->cotp.src_ref  = HTONS(0x4431);
			return ntohs(cdt->length); 
		}
	case MSG_INI_DT:
		{
			siemensTCP_iniDT_t *idt = (siemensTCP_iniDT_t *)(uip_appdata);
			if( idt->cotp.type != 0xF0 ||
				idt->req.frame_type != HTONS(8) || idt->req.request != HTONS(0x3201) ) {
				strcpy(siemensTCP_msg, "Err<IniDT>"); uip_abort(); return -1;
			}
			strcpy(siemensTCP_msg, "OK<IniDT> ");
			idt->length = htons( ntohs(idt->length) + 2 ); 
			idt->resp.response = HTONS(0x3203);
			for(i=4; i>=0; i--) idt->resp.unk_codes[i] = idt->req.unk_codes[i];
			idt->resp.unk_new = 0;
			return ntohs(idt->length);
		}
	case MSG_DT:
		{
			req_transfer_data_t tr;
			if( dt->cotp.type != 0xF0 ||
				dt->req.request != HTONS(0x3201) || dt->req.num_grp != 1) {
				strcpy(siemensTCP_msg, "Err<DT>   "); uip_abort(); return -1;
			}
			dt->resp.response 	= HTONS(0x3203);
			dt->resp.frame_type	= HTONS(0x0002);
			// parse possible write command
			if( dt->req.rw == 0x05 && ntohs(dt->req.wr_bytes) >= 5 ) {
				uint16_t	size = 0;
				for(i = 0; i < dt->req.num_grp; i++) {
					size += ntohs(dt->req.grp[i].req_bytes) + 4;
					if( i < (dt->req.num_grp-1) ) size = (size+1)&~1;
				}
				if( ntohs(dt->req.wr_bytes) != size ) {
					strcpy(siemensTCP_msg, "Err<DT_WR>"); uip_abort(); return -1;
				} 
				strcpy(siemensTCP_msg, "OK<DT_WR> ");
				uint8_t *wr_data_u8 = ((uint8_t*)&dt->req.grp[0]) + dt->req.num_grp * sizeof(dt->req.grp[0]);			
				for(i = 0; i < dt->req.num_grp; i++) {
					tr.num_bytes = ntohs(dt->req.grp[i].req_bytes);
					if( dt->req.grp[i].data_type == HTONS(0x8300) ) { // Trying to write into a M zone
						wr_data_t *wr_data = (wr_data_t *)wr_data_u8;
						tr.address = (ntohs(dt->req.grp[i].address) >> 3);
						tr.data = wr_data[0].data; // dt->req.wr_data[i].data;
						tr.num_bit = ntohs(dt->req.grp[i].address) & 7;
						if( wr_data[0].data_bits != HTONS( 1 ) ) tr.num_bit = UINT8_MAX;
						// Copy request info 
//						Request.addr = tr.address; Request.is_write = 1; 
//						Request.num_bytes = tr.num_bytes; Request.bit = tr.num_bit;
//						if( Request.num_bytes > sizeof( Request.data ) )
//							Request.num_bytes = sizeof( Request.data );
//						memcpy( Request.data, data, Request.num_bytes );
						// Do actual data transfer	
						PLC_ReceiveMemoryData( &tr );
					}
					// Next write data
					wr_data_u8 += ((tr.num_bytes + 5) & ~1);
				}
				// prepare the response
				for(i = 0; i < dt->req.num_grp; i++) {
					dt->resp.write_end[i]= 0xFF;
				}
				dt->resp.num_grp 	= dt->req.num_grp;				
				dt->resp.rw			= dt->req.rw;				
				dt->resp.wr_bytes 	= 0;
				dt->resp.data_bytes = htons( dt->resp.num_grp );
				dt->length 			= htons( 21 + dt->resp.num_grp );
				return ntohs(dt->length);
			}
			// trying to read some values
			strcpy(siemensTCP_msg, "OK<DT_RD> ");
			tr.address = (ntohs(dt->req.grp[0].address) >> 3);
			tr.num_bytes = ntohs(dt->req.grp[0].req_bytes);
			tr.data = dt->resp.grp[0].data;
			// Copy request info 
//			Request.addr = tr.address; Request.is_write = 0; 
//			Request.num_bytes = tr.num_bytes; Request.bit = UINT8_MAX;
//			if( Request.num_bytes > sizeof( Request.data ) )
//				Request.num_bytes = sizeof( Request.data );
//			memcpy( Request.data, tr.data, Request.num_bytes );

			// Do actual data transfer	
			PLC_SendMemoryData( &tr );
			
			// prepare the rest of the response
			dt->resp.grp[0].magic1 = HTONS(0xFF04);
			dt->resp.grp[0].data_bits = htons( tr.num_bytes << 3 );
			 			
			dt->resp.num_grp 	= dt->req.num_grp;				
			dt->resp.rw			= dt->req.rw;
			dt->resp.wr_bytes 	= 0;
			dt->resp.data_bytes = htons( tr.num_bytes + 4 );
			dt->length 			= htons( 25 + tr.num_bytes );	// 21 + num_bytes + 4
			return ntohs(dt->length);
		}
	}
	return -1;
}


/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ethernet.h>
#include <driverlib/ethernet.h>
#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/interrupt.h>
#include "task_definitions.h"
#include <uip/uip.h>
#include <uip/uip_arp.h>
#include "uip_app/protocol_switcher_app.h"

/************************************************/
/* Constants									*/
/************************************************/
#define	ETH_PACKET_RX_BIT	0
#define	ETH_TIMEOUT_BIT		1
#define	UIP_BUF_ETH_ADR		((struct uip_eth_hdr *)&uip_buf[0])

// UIP Timers (in MS)
#define UIP_PERIODIC_TIMER_MS	500
#define UIP_ARP_TIMER_MS		10000

/************************************************/
/* Global variables								*/
/************************************************/
xSemaphoreHandle	Sem_EthPacketReceived = NULL;
uint32_t			lPeriodicTimer = 0; 
uint32_t			lARPTimer = 0;
uint8_t				bInitUIP = true; 

/************************************************/
/* Prototypes									*/
/************************************************/
uint8_t Initialize_UIP(void);
void	Initialize_EthernetHW(void);
void	Loop_UIP(void);

/************************************************/
/* Function implementations						*/
/************************************************/

portTASK_FUNCTION( Task_Ethernet, pvParameters )
{
	portTickType Time_Start, Time_End;

	/* The parameters are not used */
	( void ) pvParameters;
	
    /* We are using the semaphore for synchronisation so we create a binary
    semaphore rather than a mutex.  We must make sure that the interrupt
    does not attempt to use the semaphore before it is created! */
    vSemaphoreCreateBinary( Sem_EthPacketReceived );

	/* Initialize Ethernet HW */
	Initialize_EthernetHW();

	/* Finish initialization once the ethernet cable is connected */
	while( !Initialize_UIP() ) {};
	//if( Initialize_UIP() ) {;
	
	Time_Start = xTaskGetTickCount();

	for(;;) {	// Forever loop
			/* Check if we have received a packet */
//#if portTICK_RATE_MS > 0 
//			xSemaphoreTake( Sem_EthPacketReceived, tsk_ETHERNET_RATE_MS / portTICK_RATE_MS );
//#else
			xSemaphoreTake( Sem_EthPacketReceived, tsk_ETHERNET_RATE_MS * portTICK_RATE_MS_INV );
//#endif		
			/* Update UIP timers */
			Time_End = xTaskGetTickCount();
//#if portTICK_RATE_MS > 0 
//	        lPeriodicTimer += (Time_End - Time_Start) * portTICK_RATE_MS;	// ticks -> ms
//	        lARPTimer += (Time_End - Time_Start) * portTICK_RATE_MS;
//#else
	        lPeriodicTimer += (Time_End - Time_Start) / portTICK_RATE_MS_INV;	// ticks -> ms
	        lARPTimer += (Time_End - Time_Start) / portTICK_RATE_MS_INV;
//#endif
	        Time_Start = Time_End;
	
			/* UIP Main loop */		
			Loop_UIP();
//		}else{
//			// Wait until Eth cable is connected
//			/* We need to initialise the time counters */
//			Time_Start = xTaskGetTickCount();	
//		}
	}
}

void Initialize_EthernetHW(void)
{
    // Enable and Reset the Ethernet Controller.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

    // Enable Port F for Ethernet LEDs.
    //  LED0        Bit 3   Output
    //  LED1        Bit 2   Output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    // Configure SysTick for a periodic interrupt.
    //SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
    //SysTickEnable();
    //SysTickIntEnable();	// Interrupt every second and timer
    
    // Intialize the Ethernet Controller and disable all Ethernet Controller
    // interrupt sources.
    EthernetIntDisable(ETH_BASE, (ETH_INT_PHY | ETH_INT_MDIO | ETH_INT_RXER |
                       ETH_INT_RXOF | ETH_INT_TX | ETH_INT_TXER | ETH_INT_RX));
    unsigned long ulTemp = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, ulTemp);

    // Initialize the Ethernet Controller for operation.
    EthernetInitExpClk(ETH_BASE, SysCtlClockGet());
    
    // Configure the Ethernet Controller for normal operation.
    // - Full Duplex
    // - TX CRC Auto Generation
    // - TX Padding Enabled
    EthernetConfigSet(ETH_BASE, (ETH_CFG_TX_DPLXEN | ETH_CFG_TX_CRCEN |
                                 ETH_CFG_TX_PADEN));	
}

uint8_t	Initialize_UIP(void)
{
    // Wait for the link to become active.
    unsigned long ulTemp = EthernetPHYRead(ETH_BASE, PHY_MR1);
    if( (ulTemp & 0x0004) == 0 )
    {
    	// Disable the Ethernet Controller.
    	bInitUIP = true; 
    	return 0;
    }
    if(!bInitUIP) return 1;
	
    // Enable the Ethernet Controller.
    EthernetEnable(ETH_BASE);
    
    // Enable the Ethernet interrupt.
    IntEnable(INT_ETH);
    
    // Enable the Ethernet RX Packet interrupt source.
    EthernetIntEnable(ETH_BASE, ETH_INT_RX);

    // Initialize the uIP TCP/IP stack.
    uip_init();
	lPeriodicTimer = 0; 
	lARPTimer = 0;
	uip_ipaddr_t ipaddr;
#ifdef USE_STATIC_IP
    uip_ipaddr(&ipaddr, DEFAULT_IPADDR0, DEFAULT_IPADDR1, DEFAULT_IPADDR2, DEFAULT_IPADDR3);
    uip_sethostaddr(&ipaddr);
    uip_ipaddr(&ipaddr, DEFAULT_NETMASK0, DEFAULT_NETMASK1, DEFAULT_NETMASK2, DEFAULT_NETMASK3);
    uip_setnetmask(&ipaddr);

    uip_ipaddr(&ipaddr, DEFAULT_IPADDR0, DEFAULT_IPADDR1, DEFAULT_IPADDR2, 1);
    uip_setdraddr(&ipaddr);

    conn_state = IP_CONFIGURED;        
#else
    uip_ipaddr(&ipaddr, 0, 0, 0, 0);
    uip_sethostaddr(&ipaddr);
    uip_ipaddr(&ipaddr, 0, 0, 0, 0);
    uip_setnetmask(&ipaddr);
#endif

	// Get MAC address from USER0 & USER1 Flash memory
	unsigned long	ulUser0, ulUser1;
    FlashUserGet(&ulUser0, &ulUser1);
    if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
    {
        while(1) {}	// Incorrect MAC Address in Flash Memory
    }

    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    static struct uip_eth_addr sTempAddr;
    sTempAddr.addr[0] = ((ulUser0 >>  0) & 0xff);
    sTempAddr.addr[1] = ((ulUser0 >>  8) & 0xff);
    sTempAddr.addr[2] = ((ulUser0 >> 16) & 0xff);
    sTempAddr.addr[3] = ((ulUser1 >>  0) & 0xff);
    sTempAddr.addr[4] = ((ulUser1 >>  8) & 0xff);
    sTempAddr.addr[5] = ((ulUser1 >> 16) & 0xff);

    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.
    EthernetMACAddrSet(ETH_BASE, (unsigned char *)&sTempAddr);
    uip_setethaddr(sTempAddr);

    // Initialize all protocols 
    protocol_switcher_app_init();
    
    return 1;
}

void Loop_UIP()
{
	long lPacketLength = 0;

    // Check for an RX Packet and read it.
    lPacketLength = EthernetPacketGetNonBlocking(ETH_BASE, uip_buf,
                                                 sizeof(uip_buf));
    if(lPacketLength > 0)
    {
        // Set uip_len for uIP stack usage.
        uip_len = (unsigned short)lPacketLength;

        // Clear the RX Packet event and renable RX Packet interrupts.
        EthernetIntEnable(ETH_BASE, ETH_INT_RX);

        // Process incoming IP packets here.
        if(UIP_BUF_ETH_ADR->type == HTONS(UIP_ETHTYPE_IP))
        {
            uip_arp_ipin();
            uip_input();

            // If the above function invocation resulted in data that
            // should be sent out on the network, the global variable
            // uip_len is set to a value > 0.
            if(uip_len > 0)
            {
                uip_arp_out();
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }
        // Process incoming ARP packets here.
        else if(UIP_BUF_ETH_ADR->type == HTONS(UIP_ETHTYPE_ARP))
        {
            uip_arp_arpin();

            // If the above function invocation resulted in data that
            // should be sent out on the network, the global variable
            // uip_len is set to a value > 0.
            if(uip_len > 0)
            {
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }
    }		

    // Process TCP/IP Periodic Timer here.
    unsigned long ulTemp;
    if(lPeriodicTimer > UIP_PERIODIC_TIMER_MS)
    {
        lPeriodicTimer = 0;
        for(ulTemp = 0; ulTemp < UIP_CONNS; ulTemp++)
        {
            uip_periodic(ulTemp);

            // If the above function invocation resulted in data that
            // should be sent out on the network, the global variable
            // uip_len is set to a value > 0.
            if(uip_len > 0)
            {
                uip_arp_out();
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }

#if UIP_UDP
        for(ulTemp = 0; ulTemp < UIP_UDP_CONNS; ulTemp++)
        {
            uip_udp_periodic(ulTemp);

            // If the above function invocation resulted in data that
            // should be sent out on the network, the global variable
            // uip_len is set to a value > 0.
            if(uip_len > 0)
            {
                uip_arp_out();
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }
#endif // UIP_UDP
    }

    // Process ARP Timer here.
    if(lARPTimer > UIP_ARP_TIMER_MS)
    {
        lARPTimer = 0;
        uip_arp_timer();
    }
}

//*****************************************************************************
//
// The interrupt handler for the Ethernet interrupt.
//
//*****************************************************************************
void EthernetIntHandler(void)
{
    unsigned long ulTemp;

    // Read and Clear the interrupt.
    ulTemp = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, ulTemp);

    // Check to see if an RX Interrupt has occured.
    if(ulTemp & ETH_INT_RX)
    {
        // Disable Ethernet RX Interrupt.
        EthernetIntDisable(ETH_BASE, ETH_INT_RX);
        
        // Do processing in the Loop_UIP()
        static portBASE_TYPE xHigherPriorityTaskWoken;
        xSemaphoreGiveFromISR( Sem_EthPacketReceived, &xHigherPriorityTaskWoken );
    }
}

//*****************************************************************************
//! When using the timer module in UIP, this function is required to return
//! the number of ticks.  Note that the file "clock-arch.h" must be provided
//! by the application, and define CLOCK_CONF_SECONDS as the number of ticks
//! per second, and must also define the typedef "clock_time_t".
//*****************************************************************************
#include <uip/clock.h>
clock_time_t clock_time(void)
{
    return((clock_time_t)xTaskGetTickCount());
}

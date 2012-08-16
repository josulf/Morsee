/**
 * Selección del demo
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>

/**
 * Function prototypes
 */
extern	int	main_demo_01(void);		// Buttons & LED (+ Display)
extern	int	main_demo_02(void);		// Analog-Digital Converter and Temperature measurements
extern	int	main_demo_03(void);		// PWM demo
extern	int	main_demo_04(void);		// Serial communications
extern	int	main_demo_04b(void);	// Serial communications XON/XOFF protocol demo
extern	int	main_demo_04c(void);	// Serial communications with interrupts
extern	int	main_demo_05(void);		// LCD screen graphical demo
extern	int	main_demo_06(void);		// FreeRTOS demo
extern	int	main_demo_07(void);		// Internet client and server demo
extern	int	main_demo_07b(void);	// Internet client, server and PLC demo
extern	int	main_demo_08(void);		// Music demo
extern  int main_demo_aa(void);		// Nuestra demo

int main(void)
{
	static int res;
//	res = main_demo_01();	// Buttons & LED (+ Display)
//	res = main_demo_02();	// Analog-Digital Converter and Temperature measurements
//	res = main_demo_03();	// PWM demo
//	res = main_demo_04();	// Serial communications
//	res = main_demo_04b();	// Serial communications XON/XOFF protocol demo
//	res = main_demo_04c();	// Serial communications with interrupts
//	res = main_demo_05();	// LCD screen graphical demo
//	res = main_demo_06();	// FreeRTOS demo
//	res = main_demo_07();	// Internet client and server demo
//	res = main_demo_07b();	// Internet client, server and PLC demo
//	res = main_demo_08();	// Music demo
	res = main_demo_aa();
	return res;
}

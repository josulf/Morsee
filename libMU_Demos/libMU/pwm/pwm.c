/**
 * @addtogroup	libMU_PWM
 * Library for easy access to the serial port
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */

#include <inc/hw_types.h>		// New data types
#include <inc/hw_memmap.h>		// Peripheral memory map
#include <driverlib/sysctl.h>	// System control functions
#include <driverlib/gpio.h>		// General Purpose IO functions
#include <driverlib/pwm.h>		// Pulse Width Modulation functions
#include <libMU/pwm.h>

/**
 * Configuration constants
 * pins - PF0/PWM0 (led), PG1/PWM1 (sound),  PB0/PWM2,
 *        PB1/PWM3,       PE1/PWM4 (button), PE2/PWM5 (button)
 */
typedef struct _pwmc_t {
	uint32_t sysctl_periph, gpio_port, gpio_pin, gen_code, gen_bit, out_code, out_bit; 
} pwmc_t;
static const pwmc_t PWMC[ LIBMU_PWM_5 + 1 ] = {
	{ SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_0, PWM_GEN_0, PWM_GEN_0_BIT, PWM_OUT_0, PWM_OUT_0_BIT },
	{ SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE, GPIO_PIN_1, PWM_GEN_0, PWM_GEN_0_BIT, PWM_OUT_1, PWM_OUT_1_BIT },
	{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_0, PWM_GEN_1, PWM_GEN_1_BIT, PWM_OUT_2, PWM_OUT_2_BIT },
	{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, GPIO_PIN_1, PWM_GEN_1, PWM_GEN_1_BIT, PWM_OUT_3, PWM_OUT_3_BIT },
	{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_1, PWM_GEN_2, PWM_GEN_2_BIT, PWM_OUT_4, PWM_OUT_4_BIT },
	{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_2, PWM_GEN_2, PWM_GEN_2_BIT, PWM_OUT_5, PWM_OUT_5_BIT }
}; 

/**
 * PWM generator input frequency
 */
static	uint32_t	libMU_PWM_Freq;

/**
 * Set-up PWM output 
 * @param	outputID	Output identifier (single output)
 * @param	freq		Desired PWM frequency
 * @return				1 if error, 0 if ok		
 * @note
 * The CPU frequency must be set before calling this function
 * Careful with specific outputs:
 * - PWM0 is connected to LED0
 * - PWM1 is connected to the buzzer
 * - PWM4 is connected to up button input	(Shortcircuit risk!!!)
 * - PWM5 is connected to down button input	(Shortcircuit risk!!!)
 */
int	libMU_PWM_InitializeOutput( libMU_PWM_Constants_t outputID, uint32_t freq )
{
	uint32_t	cpu_freq = SysCtlClockGet(); 
	uint32_t	count = cpu_freq / freq;
	uint32_t	div, div_code;
	for( div = 0; count > 0xFFFF; div++ ) count >>= 1;
	switch( div ) {
		case 0: div_code = SYSCTL_PWMDIV_1;	break;
		case 1: div_code = SYSCTL_PWMDIV_2;	break;
		case 2: div_code = SYSCTL_PWMDIV_4;	break;
		case 3: div_code = SYSCTL_PWMDIV_8;	break;
		case 4: div_code = SYSCTL_PWMDIV_16;	break;
		case 5: div_code = SYSCTL_PWMDIV_32;	break;
		case 6: div_code = SYSCTL_PWMDIV_64;	break;
		default: return 1;		// This frequency can not obtained
	}
	libMU_PWM_Freq = cpu_freq / (1<<div);
	switch( outputID ) {
	case LIBMU_PWM_0:
    	SysCtlPWMClockSet		( div_code );	// Set the PWM clock to the system clock
    	SysCtlPeripheralEnable	( SYSCTL_PERIPH_PWM );	// Enable PWM peripheral
	    SysCtlPeripheralEnable	( SYSCTL_PERIPH_GPIOF );// Enable port for PWM pins & assign pins
	    GPIOPinTypePWM	( GPIO_PORTF_BASE, GPIO_PIN_0 );
	    PWMGenConfigure	( PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN );
	    //PWMSyncUpdate	( PWM_BASE, PWM_GEN_0_BIT );
	    PWMGenPeriodSet	( PWM_BASE, PWM_GEN_0, count );
	    PWMPulseWidthSet( PWM_BASE, PWM_OUT_0, (count>>1) );// Duty cycle %50
	    PWMOutputState	( PWM_BASE, PWM_OUT_0_BIT, true );	// Enable PWM0/PF0
	    PWMGenEnable	( PWM_BASE, PWM_GEN_0 );			// Enables PWM0 counter
		return 0;
	}
	if( outputID > LIBMU_PWM_5 ) return 1;
	SysCtlPeripheralEnable	( SYSCTL_PERIPH_PWM );	// Enable PWM peripheral
	SysCtlPWMClockSet		( div_code );	// Set the PWM clock to the system clock
	SysCtlPeripheralEnable	( PWMC[outputID].sysctl_periph );// Enable port for PWM pins
	GPIOPinTypePWM	( PWMC[outputID].gpio_port, PWMC[outputID].gpio_pin );
	PWMGenConfigure	( PWM_BASE, PWMC[outputID].gen_code, PWM_GEN_MODE_DOWN );
	//PWMSyncUpdate	( PWM_BASE, PWMC[outputID].gen_bit );	// Force PWM updates when counter reaches 0
	PWMGenPeriodSet	( PWM_BASE, PWMC[outputID].gen_code, count );
	PWMPulseWidthSet( PWM_BASE, PWMC[outputID].out_code, (count>>1) );	// Duty cycle %50
	PWMOutputState	( PWM_BASE, PWMC[outputID].out_bit, true );	
	PWMGenEnable	( PWM_BASE, PWMC[outputID].gen_code );
//	PWMSyncTimeBase	( PWM_BASE, PWM_GEN_0_BIT | PWM_GEN_1_BIT | PWM_GEN_2_BIT );
	return 0;
}

/**
 * Set-up PWM output for  chopper control with deadbands 
 * @param	groupId		Output group identifier (single output)
 * @param	freq		Desired PWM frequency		
 * @param	deadband	Required deadband time in ns
 * @return				1 if error, 0 if ok		
 * @note
 * The CPU frequency must be set before calling this function
 * Careful with specific outputs:
 * - PWM_GRP_01 is connected to LED0 and Buzzer
 * - PWM_GRP_45 is connected to the up/down button inputs (Shortcircuit risk!!!)
 * - The generated deadband is between the two outputs of the group
 * - The maximum deadband is 4095*Tpwm;
 */
int	libMU_PWM_InitializeChopperOutput( libMU_PWM_Constants_t groupId, uint32_t freq, uint32_t deadband )
{
	uint32_t	cpu_freq = SysCtlClockGet(); 
	uint32_t	count = cpu_freq / freq;
	uint32_t	pwm_freq, div, div_code;
	for( div = 0; count > 0xFFFF; div++ ) count >>= 1;
	switch( div ) {
		case 0: div_code = SYSCTL_PWMDIV_1;	break;
		case 1: div_code = SYSCTL_PWMDIV_2;	break;
		case 2: div_code = SYSCTL_PWMDIV_4;	break;
		case 3: div_code = SYSCTL_PWMDIV_8;	break;
		case 4: div_code = SYSCTL_PWMDIV_16;	break;
		case 5: div_code = SYSCTL_PWMDIV_32;	break;
		case 6: div_code = SYSCTL_PWMDIV_64;	break;
		default: return 1;		// This frequency can not obtained
	}	
	pwm_freq = cpu_freq / (1<<div);
	libMU_PWM_Freq = cpu_freq / (1<<div);
	deadband = ((uint64_t)deadband * (uint64_t)pwm_freq) / 1000000000ull;	// Convert microseconds to PWM clock ticks
	if( deadband >= 4096 ) {
		for(;;);	// Error, it can not generate this dead band size
	}
	SysCtlPeripheralEnable	( SYSCTL_PERIPH_PWM );	// Enable PWM peripheral
	SysCtlPWMClockSet		( div_code );	// Set the PWM clock to the system clock
	switch( groupId ) {
	case LIBMU_PWM_GRP_01:
		SysCtlPeripheralEnable	( PWMC[0].sysctl_periph );// Enable port for PWM pins
		SysCtlPeripheralEnable	( PWMC[1].sysctl_periph );// Enable port for PWM pins
		GPIOPinTypePWM	( PWMC[0].gpio_port, PWMC[0].gpio_pin );
		GPIOPinTypePWM	( PWMC[1].gpio_port, PWMC[1].gpio_pin );
		PWMGenConfigure	( PWM_BASE, PWMC[0].gen_code, PWM_GEN_MODE_DOWN );
		//PWMSyncUpdate	( PWM_BASE, PWMC[0].gen_bit );	// Force PWM updates when counter reaches 0
		PWMGenPeriodSet	( PWM_BASE, PWMC[0].gen_code, count );
		PWMPulseWidthSet( PWM_BASE, PWMC[0].out_code, (count>>1) );	// Duty cycle %50
		PWMPulseWidthSet( PWM_BASE, PWMC[1].out_code, (count>>1) );	// Duty cycle %50
		PWMOutputState	( PWM_BASE, PWMC[0].out_bit, true );	
		PWMOutputState	( PWM_BASE, PWMC[1].out_bit, true );	
		PWMDeadBandEnable(PWM_BASE, PWMC[0].gen_code, deadband, deadband );
		PWMGenEnable	( PWM_BASE, PWMC[0].gen_code );
		return 0;
	case LIBMU_PWM_GRP_23:
		SysCtlPeripheralEnable	( PWMC[2].sysctl_periph );// Enable port for PWM pins
		SysCtlPeripheralEnable	( PWMC[3].sysctl_periph );// Enable port for PWM pins
		GPIOPinTypePWM	( PWMC[2].gpio_port, PWMC[2].gpio_pin );
		GPIOPinTypePWM	( PWMC[3].gpio_port, PWMC[3].gpio_pin );
		PWMGenConfigure	( PWM_BASE, PWMC[2].gen_code, PWM_GEN_MODE_DOWN );
		//PWMSyncUpdate	( PWM_BASE, PWMC[2].gen_bit );	// Force PWM updates when counter reaches 0
		PWMGenPeriodSet	( PWM_BASE, PWMC[2].gen_code, count );
		PWMPulseWidthSet( PWM_BASE, PWMC[2].out_code, (count>>1) );	// Duty cycle %50
		PWMPulseWidthSet( PWM_BASE, PWMC[3].out_code, (count>>1) );	// Duty cycle %50
		PWMOutputState	( PWM_BASE, PWMC[2].out_bit, true );	
		PWMOutputState	( PWM_BASE, PWMC[3].out_bit, true );	
		PWMDeadBandEnable(PWM_BASE, PWMC[2].gen_code, deadband, deadband );
		PWMGenEnable	( PWM_BASE, PWMC[2].gen_code );
		return 0;
	case LIBMU_PWM_GRP_45:
		SysCtlPeripheralEnable	( PWMC[4].sysctl_periph );// Enable port for PWM pins
		SysCtlPeripheralEnable	( PWMC[5].sysctl_periph );// Enable port for PWM pins
		GPIOPinTypePWM	( PWMC[4].gpio_port, PWMC[4].gpio_pin );
		GPIOPinTypePWM	( PWMC[5].gpio_port, PWMC[5].gpio_pin );
		PWMGenConfigure	( PWM_BASE, PWMC[4].gen_code, PWM_GEN_MODE_DOWN );
		//PWMSyncUpdate	( PWM_BASE, PWMC[4].gen_bit );	// Force PWM updates when counter reaches 0
		PWMGenPeriodSet	( PWM_BASE, PWMC[4].gen_code, count );
		PWMPulseWidthSet( PWM_BASE, PWMC[4].out_code, (count>>1) );	// Duty cycle %50
		PWMPulseWidthSet( PWM_BASE, PWMC[5].out_code, (count>>1) );	// Duty cycle %50
		PWMOutputState	( PWM_BASE, PWMC[4].out_bit, true );	
		PWMOutputState	( PWM_BASE, PWMC[5].out_bit, true );	
		PWMDeadBandEnable(PWM_BASE, PWMC[4].gen_code, deadband, deadband );
		PWMGenEnable	( PWM_BASE, PWMC[4].gen_code );
		return 0;
	}
	PWMSyncTimeBase	( PWM_BASE, PWM_GEN_0_BIT | PWM_GEN_1_BIT | PWM_GEN_2_BIT );
	return 1;
}
	
/**
 * Set-up PWM output pulse width 
 * @param	outId		Output identifier (single output)
 * @param	pulsewidth	In hundreds of a percentage: 0 -> 0.00%, 10000 -> 100.00%, 4567 -> 45.67%		
 * @note
 * The resolution may be better or worse than 0.1% depending on cpu and pwm frequency
 */
void	libMU_PWM_SetPulseWidth( libMU_PWM_Constants_t outId, uint32_t pulsewidth )
{
	pulsewidth = ( pulsewidth * PWMGenPeriodGet( PWM_BASE, PWMC[outId].gen_code ) ) / 10000;
	if( pulsewidth == 0 ) pulsewidth = 1;
	PWMPulseWidthSet( PWM_BASE, PWMC[outId].out_code, pulsewidth );
}

/**
 * Set-up PWM output frequency
 * @param	outId		Output identifier (single output)
 * @param	freq		In Hz
 * @note
 * The resolution may be quite bad
 */
void	libMU_PWM_SetFrequency( libMU_PWM_Constants_t outId, uint32_t freq )
{
	uint32_t	period	= libMU_PWM_Freq / freq;
	PWMGenPeriodSet( PWM_BASE, PWMC[outId].gen_code, period );
}

/**
 * Get PWM generator period in internal ticks
 * @param	outId		Output identifier (single output)
 * @return				PWM Generator period
 */
uint32_t	libMU_PWM_GetPeriodIU( libMU_PWM_Constants_t outId )
{
	return PWMGenPeriodGet( PWM_BASE, PWMC[outId].gen_code );
}

/**
 * Get PWM output pulse width in internal ticks
 * @param	outId		Output identifier (single output)
 * @return				PWM pulse width
 */
uint32_t	libMU_PWM_GetPulseWidthIU( libMU_PWM_Constants_t outId )
{
	return PWMPulseWidthGet( PWM_BASE, PWMC[outId].out_code );
}

/**
 * Disable PWM output
 * @param	outId		Output identifier (single output)
 */
void	libMU_PWM_OutputDisable( libMU_PWM_Constants_t outId )
{
	PWMOutputState( PWM_BASE, PWMC[outId].gen_code, false );
}

/**
 * Enable PWM output
 * @param	outId		Output identifier (single output)
 */
void	libMU_PWM_OutputEnable( libMU_PWM_Constants_t outId )
{
	PWMOutputState( PWM_BASE, PWMC[outId].gen_code, true );
}

/**
 * @}
 */
	

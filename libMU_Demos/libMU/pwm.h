/**
 * @addtogroup	libMU_PWM
 * Library for easy access to the PWM outputs
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef _LIBMU_PWM_H_
#define _LIBMU_PWM_H_

#include <stdint.h>

/**
 * PWM output identifiers
 */
typedef	enum _libMU_PWM_Constants_t {
	LIBMU_PWM_0 = 0,
	LIBMU_PWM_1,
	LIBMU_PWM_2,
	LIBMU_PWM_3,
	LIBMU_PWM_4,
	LIBMU_PWM_5,
	LIBMU_PWM_GRP_01,	/* output groups for PWM chopper control with dead band */
	LIBMU_PWM_GRP_23,
	LIBMU_PWM_GRP_45
} libMU_PWM_Constants_t;

/**
 * Set-up PWM output 
 * @param	outputId	Output identifier (single output)
 * @param	freq		Desired PWM frequency		
 * @return				1 if error, 0 if ok		
 * @note
 * Careful with specific outputs:
 * - PWM0 is connected to LED
 * - PWM1 is connected to the buzzer
 * - PWM4 is connected to up button input	(Shortcircuit risk!!!)
 * - PWM5 is connected to down button input	(Shortcircuit risk!!!)
 */
int	libMU_PWM_InitializeOutput( libMU_PWM_Constants_t outputId, uint32_t freq );

/**
 * Set-up PWM output for  chopper control with deadbands 
 * @param	groupId		Output identifier (single output)
 * @param	freq		Desired PWM frequency		
 * @param	deadband	Required deadband time in ns
 * @return				1 if error, 0 if ok		
 * @note
 * Careful with specific outputs:
 * - PWM_GRP_01 is connected to LED and buzzer
 * - PWM_GRP_45 is connected to the up/down button inputs (Shortcircuit risk!!!)
 */
int	libMU_PWM_InitializeChopperOutput( libMU_PWM_Constants_t groupId, uint32_t freq, uint32_t deadband ); 

/**
 * Set-up PWM output pulse width 
 * @param	outId		Output identifier (single output)
 * @param	pulsewidth	In hundredths of a percentage: 0 -> 0.00%, 10000 -> 100.00%, 4567 -> 45.67%
 * @note
 * The resolution may be better or worse than 0.1% depending on cpu and pwm frequency
 */
void	libMU_PWM_SetPulseWidth( libMU_PWM_Constants_t outId, uint32_t pulsewidth );

/**
 * Set-up PWM output frequency
 * @param	outId		Output identifier (single output)
 * @param	freq		In Hz
 * @note
 * The resolution may be quite bad
 */
void	libMU_PWM_SetFrequency( libMU_PWM_Constants_t outId, uint32_t freq );

/**
 * Get PWM generator period in internal ticks
 * @param	outId		Output identifier (single output)
 * @return				PWM Generator period
 */
uint32_t	libMU_PWM_GetPeriodIU( libMU_PWM_Constants_t outId );

/**
 * Get PWM output pulse width in internal ticks
 * @param	outId		Output identifier (single output)
 * @return				PWM pulse width
 */
uint32_t	libMU_PWM_GetPulseWidthIU( libMU_PWM_Constants_t outId );

/**
 * Disable PWM output
 * @param	outId		Output identifier (single output)
 */
void	libMU_PWM_OutputDisable( libMU_PWM_Constants_t outId );
/**
 * Enable PWM output
 * @param	outId		Output identifier (single output)
 */
void	libMU_PWM_OutputEnable( libMU_PWM_Constants_t outId );

#endif /*_LIBMU_PWM_H_*/
/**
 * @}
 */

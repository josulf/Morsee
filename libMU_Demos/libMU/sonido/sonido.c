/**
 * @addtogroup libMU_Sound
 * Libreria para la generación de sonidos
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <inc/hw_types.h>		// New data types
#include <driverlib/sysctl.h>	// System control functions
#include <libMU/pwm.h>
#include <libMU/sonido.h>

/**
 * Global variables for sound control
 */
uint8_t						libMU_Sound_Volume;		/* Volume */
uint8_t						libMU_Sound_Playing;	/* Playing */
uint32_t					libMU_Sound_Repeat;		/* number of repetitions */

/**
 * Local variables for sound control
 */
static uint32_t				libMU_Sound_Tempo_Mult;	/* tempo multiplication */
static uint32_t				libMU_Sound_Tempo_Div;	/* tempo division */
static libMU_Sound_Score_t	libMU_Sound_Score;

/**
 * Sound player task
 * @param	unused	Unused parameter (necesarry for FreeRTOS task prototype format)
 */
static void libMU_Sound_Task(void* unused);

/**
 * Initialize the sound library
 * @note	This library uses the FreeRTOS operating system to generate the music timings
 */
void	libMU_Sound_Initialize(void)
{
	static int initDone = 0;
	SysCtlClockSet	( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );
	libMU_PWM_InitializeOutput	( LIBMU_PWM_1, 25/*Hz*/ );
	libMU_PWM_OutputDisable		( LIBMU_PWM_1 );
	libMU_PWM_SetPulseWidth		( LIBMU_PWM_1, 0 );
	/* initialize local variables */
	libMU_Sound_Volume			= 25;
	libMU_Sound_Playing			= 0;
	libMU_Sound_Repeat			= 0;
	libMU_Sound_Tempo_Mult		= portTICK_RATE_MS_INV;
	libMU_Sound_Tempo_Div		= 1;
	libMU_Sound_Score.song		= NULL;
	libMU_Sound_Score.num_notes	= 0;
	/* create task (only once)  */
	if( !initDone ) {
		xTaskCreate( libMU_Sound_Task, "Music", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
		initDone = 1;
	}
}

/**
 * Set the song to be played
 * @param	notes		Pointer to note frequencies and durations array
 * @param	num_notes	Number of notes in the previous score
 */
void	libMU_Sound_SetSong( libMU_Sound_ScoreNote_t* notes, uint32_t num_notes )
{
	if( notes == NULL || num_notes == 0 ) {
		notes = NULL; num_notes = 0;
	}
	libMU_Sound_Score.song = notes;
	libMU_Sound_Score.num_notes = num_notes;
}


/**
 * Sound player task
 * @param	unused	Unused parameter (necesarry for FreeRTOS task prototype format)
 */
static void libMU_Sound_Task(void* unused)
{
	static portTickType	last;
	uint32_t			note;
	uint32_t			time = 20 * portTICK_RATE_MS_INV;
	unsigned int		cur_note = 0;
	unsigned int		prv_playing;

	for(;;)	// repite indefinidamente
	{
		if( libMU_Sound_Playing && !prv_playing ) {
			cur_note = 0;	// We start playing again
			libMU_PWM_OutputEnable( LIBMU_PWM_1 );	// Enable output
		}
		if( !libMU_Sound_Playing && prv_playing ) {
			libMU_PWM_OutputDisable( LIBMU_PWM_1 );	// Disable output
		}
		prv_playing = libMU_Sound_Playing;
		if( libMU_Sound_Playing ) {
			if( libMU_Sound_Score.song != NULL ) {
				note = libMU_Sound_Score.song[cur_note].note;
				time = libMU_Sound_Score.song[cur_note].time;
				time *= libMU_Sound_Tempo_Mult;
				time /= libMU_Sound_Tempo_Div;
				if( note && libMU_Sound_Volume ) {
					libMU_PWM_SetFrequency	( LIBMU_PWM_1, note );
					libMU_PWM_SetPulseWidth	( LIBMU_PWM_1, libMU_Sound_Volume );
				}else{
					libMU_PWM_SetFrequency	( LIBMU_PWM_1, 25/*Hz*/ );
					libMU_PWM_SetPulseWidth	( LIBMU_PWM_1, 0 );
				}
				cur_note++;
				if( cur_note >= libMU_Sound_Score.num_notes ) {
					if( libMU_Sound_Repeat > 0 )	{	/* repeat if necessary */
						cur_note = 0;
						libMU_Sound_Repeat--;
					}else{
						libMU_Sound_Playing = 0;		/* stop playing */
					}
				}
				vTaskDelayUntil( &last, time );
			}else{
				libMU_Sound_Playing = 0;
			}
		}
		libMU_PWM_SetFrequency	( LIBMU_PWM_1, 25/*Hz*/ );
		libMU_PWM_SetPulseWidth	( LIBMU_PWM_1, 0 );
		vTaskDelayUntil( &last, 20 * portTICK_RATE_MS_INV );
	}
}

/**
 * @}
 */

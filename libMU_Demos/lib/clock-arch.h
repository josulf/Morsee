#ifndef CLOCKARCH_H_
#define CLOCKARCH_H_

#include <stdint.h>

#include "FreeRTOS.h"
typedef	uint32_t	clock_time_t;

// Define how many clock ticks in one second.
// Note:  This should match the FreeRTOS configTICK_RATE_HZ
#define CLOCK_CONF_SECOND       configTICK_RATE_HZ

#endif /*CLOCKARCH_H_*/

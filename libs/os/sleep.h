/*
 * Copyright 2009 Anton Staaf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __libs_os_sleep_h__
#define __libs_os_sleep_h__

#include "libs/types/types.h"

typedef bool (*SleepCheck)(void *user_data);

#define SIGNAL_EXPAND(signal)	signal ## _index
#define SIGNAL_INDEX(signal)	SIGNAL_EXPAND(signal)

#define	__vector_1_index	0
#define	__vector_2_index	1
#define	__vector_3_index	2
#define	__vector_4_index	3
#define	__vector_5_index	4
#define	__vector_6_index	5
#define	__vector_7_index	6
#define	__vector_8_index	7
#define	__vector_9_index	8
#define	__vector_10_index	9
#define	__vector_11_index	10
#define	__vector_12_index	11
#define	__vector_13_index	12
#define	__vector_14_index	13
#define	__vector_15_index	14
#define	__vector_16_index	15
#define	__vector_17_index	16
#define	__vector_18_index	17
#define	__vector_19_index	18
#define	__vector_20_index	19
#define	__vector_21_index	20
#define	__vector_22_index	21
#define	__vector_23_index	22
#define	__vector_24_index	23
#define	__vector_25_index	24
#define	__vector_26_index	25
#define	__vector_27_index	26
#define	__vector_28_index	27
#define	__vector_29_index	28
#define	__vector_30_index	29
#define	__vector_31_index	30
#define	__vector_32_index	31

void os_inhibit_deep_sleep(uint8 vector);
void os_enable_deep_sleep(uint8 vector);
void os_set_deep_sleep(uint8 vector, bool enable);
void os_sleep_cpu(void *user_data, SleepCheck sleep_check);

#endif //__libs_os_sleep_h__

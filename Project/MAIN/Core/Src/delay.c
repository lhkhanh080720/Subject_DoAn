/*
 * delay.c
 *
 *  Created on: Mar 19, 2022
 *      Author: USER
 */
/************************************************************************
 Internal CLock = 72MHz = 72 000 000 Hz
 Ftimer = 72 000 000 / Prescaler(1440) = 50 000
 Ttimer = 1/Ftimer = 2.10^-5 s = 0.02 ms
 => Time ngat tran (ms) = 1ms => Counter Period = 50
 ************************************************************************/
#include "main.h"



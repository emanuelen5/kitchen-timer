/* ---------------------------------------------------------------------------
toneAC Library

AUTHOR/LICENSE:
Created by Tim Eckel - tim@leethost.com
Copyright 2019 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
--------------------------------------------------------------------------- */

#ifndef toneAC_h
#define toneAC_h

#include "stdint.h"

#define NOTONEAC 0

void toneAC(unsigned long frequency = NOTONEAC, uint8_t volume = 10);
void noToneAC();
#endif
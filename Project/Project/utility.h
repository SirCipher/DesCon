/*----------------------------------------------------------------------------
 * Name:    utility.h
 * Purpose: providing common and general functions for other classes
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __UTILITY_H
#define __UTILITY_H


extern void SysTick_Handler(void);
extern void Delay (uint32_t dlyTicks);
extern unsigned int String_Len(const char* str);


#endif

/************************************************************************/
/*																											*/
/*	ACL2.h	--	Interface Declarations for ACL2.cpp			        	*/
/*																											*/
/************************************************************************/
/*	Author:		Samuel Lowe																*/
/*	Copyright (c) 2014, Digilent Inc. All rights reserved.					*/

/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  Module Description:													*/
/*																						*/
/*	This is the static PmodACL2 header file					*/
/*																						*/
/************************************************************************/
/*  Revision History:														*/
/*																						*/
/*	10/17/2011(SamL): created										*/
/*																						*/
/************************************************************************/

#if !defined(ACL2_H)
#define ACL2_H

#include "SPI.h"



extern "C" {
  #include <stdint.h>
}

/* ------------------------------------------------------------ */
/*					Miscellaneous Declarations					*/
/* ------------------------------------------------------------ */



/*  Data Registers
*/

//Sensor's memory register addresses:
const uint8_t PART_ID = 0x02;  //Device ID
const uint8_t X_DATA = 0x08;  
const uint8_t Y_DATA = 0x09;  
const uint8_t Z_DATA = 0x0A; 
const uint8_t STATUS = 0x0B; //STATUS register 
const uint8_t FIFO_ENTRIES_L = 0x0C;
const uint8_t FIFO_ENTRIES_H = 0x0D;
const uint8_t XDATA_L = 0x0E;
const uint8_t XDATA_H = 0x0F;
const uint8_t YDATA_L = 0x10;
const uint8_t YDATA_H = 0x11;
const uint8_t ZDATA_L = 0x12;
const uint8_t ZDATA_H = 0x13;
const uint8_t TEMP_L = 0x14;
const uint8_t TEMP_H = 0x15;
const uint8_t SOFT_RESET = 0x1F;
const uint8_t THRESH_INACT_L = 0x23;
const uint8_t THRESH_INACT_H  = 0x24;  
const uint8_t TIME_INACT_L = 0x25;  
const uint8_t ACT_INACT_CTL = 0x27 ;  
const uint8_t FIFO_CONTROL = 0x28;
const uint8_t FIFO_SAMPLES = 0x29;
const uint8_t INTMAP1  = 0x2A;
const uint8_t INTMAP2 = 0x2B;
const uint8_t FILTER_CTL = 0x2C;   
const uint8_t POWER_CTL = 0x2D;


//command bytes

const uint8_t READ = 0x0B;   				//command byte for reading from ACL2
const uint8_t WRITE = 0x0A;  				//command byte for writing to ACL2
const uint8_t FIFO_READ = 0x0D;		//command byte for reading from ACL2 FIFO buffer	

/*	Initialization Parameters
*/
const uint8_t FREE_FALL_THRESH = 0x96;  	//Sets free-fall detection threshold to 600mg
const uint8_t FREE_FALL_TIME = 0x03;    		//Sets free-fall detection time to 30ms
const uint8_t ABS_INACT_ENABLE = 0x0C; 	//Enable absolute inactivity detect
const uint8_t SET_INACT_INTERUPT = 0x20;	//Sets the inactivity interrupt to the interrupt 1 pin
const uint8_t SENSOR_RANGE_8 = 0x83;      	//Sets sensor range to 8g with 100Hz ODR
const uint8_t SENSOR_RANGE_4 = 0x43;     	//Sets sensor range to 4g with 100Hz ODR
const uint8_t SENSOR_RANGE_2 = 0x3;      	//Sets sensor range to 2g with 100Hz ODR
const uint8_t BEGIN_MEASURE = 0x22;     		//Begins measurement




/* ------------------------------------------------------------ */
/*					Object Class Declarations					*/
/* ------------------------------------------------------------ */



class myQueue
{
	public:	
		myQueue();
		void empty();
		int size();
		int front();
		int back();
		void push_back(int value);
		int pop_front();
		void resetQueue();
		void getQueue(int* outqueue);
		
	private:		
		int dataQueue[512];
		int front_ptr;
		int back_ptr;
		int head_ptr;
		int tail_ptr;
};


class ACL2
{
	public:
	
		ACL2();
		void begin(int CS);
		void init();
		
		int getX();		
		int getY();
		int getZ();
		int getTemp();		
		uint8_t getStatus();	
		uint8_t getRange();
		
		uint8_t readRegister(uint8_t thisRegister);
		void writeRegister(uint8_t thisRegister, uint8_t thisValue);	
		
		void reset();
		void updateRange();
		
		void setRange(int newRange);
		void setZero();		
		
		int getFIFOentries();
		void initFIFO();
		void fillFIFO();
		
		int getData(uint8_t reg1, uint8_t reg2);		
		
		myQueue xFIFO;
		myQueue yFIFO;
		myQueue zFIFO;
		myQueue tempFIFO;
		
	private:	
			
		
		uint16_t twosToBin(uint16_t input);		
		char getDIR(uint16_t value);
		
		int chipSelect;	
		uint8_t range; 
		int xZero;
		int yZero;
		int zZero;			
		
};

#endif //ACL2_H
/************************************************************************/
/*																								*/
/*	ACL2.cpp	--	Driver for Accelerometer				        	*/
/*																								*/
/************************************************************************/
/*	Author: 	Samuel Lowe														*/
/*	Copyright (c) 2014, Digilent Inc, All rights reserved		*/
/************************************************************************/
/*  Module Description: 															*/
/*																								*/
/*			This the PmodACL2 CPP file										*/
/*																								*/
/************************************************************************/
/*  Revision History:																*/
/*																								*/
/*	10/26/2011(SamL): created												*/
/*																								*/
/************************************************************************/
/*	Needs work:																											*/
/*																																	*/
/*		Various functionality could be added to further the library such as threshold	*/
/*		inactivity 																												*/
/*																																	*/
/*																																	*/
/*		For the PmodACL2 datasheet, refer to:															*/
/*   	http://www.analog.com/static/imported-files/data_sheets/ADXL362.pdf 		*/
/*																																	*/
/****************************************************************************************/
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


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <ACL2.h>
#include <deque>

using namespace std;
extern "C" {
  #include <stdint.h>
}

/* ------------------------------------------------------------ */
/*			Accelerometer Driver Object Instantiation					*/
/* ------------------------------------------------------------ */
/* Instantiate a single static instance of this object class
*/



/* ------------------------------------------------------------ */
/*  ACL2
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Constructor to the class ACL2
*/
ACL2::ACL2(){	
}

/* ------------------------------------------------------------ */
/*  begin()
**
**  Parameters:
**    CS is the desired chip select pin for the SPI interface. Need 
**		this value to access into our read and write register functions
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	initializes the class parameters and calls for the IC to be initialized
*/
void ACL2::begin(int CS){	
	SPI.begin();
	chipSelect = CS;
		
	//if we know the ACL2 will be at rest during start up run setZero() instead.
	xZero = -120;
	yZero = -175;
	zZero = -320;

	reset();
}

/* ------------------------------------------------------------ */
/*  init()
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	This initializes the IC for a sensitivity of +- 8g (256 per 1g) and sets up default settings on
** 	activity and drop detection by writing to various registers
*/
void ACL2::init(){
	
  delay(10);
  writeRegister(THRESH_INACT_L,FREE_FALL_THRESH);	  	//Sets free fall detection threshold to 600mg
  delay(10);
  writeRegister(TIME_INACT_L,FREE_FALL_TIME);					//Sets free-fall detection time to 30ms  
  delay(10);
  writeRegister(ACT_INACT_CTL,ABS_INACT_ENABLE);			//Enable absolute inactivity detect  
  delay(10);
  writeRegister(THRESH_INACT_H,SET_INACT_INTERUPT);	//Sets the inactivity interrupt to the interrupt pin 1
  delay(10);  
  writeRegister( FILTER_CTL,SENSOR_RANGE_8);					//Sets sensor range to 8g with 100Hz ODR
  delay(10);
  writeRegister( POWER_CTL, BEGIN_MEASURE);					//Begins measurement  
  delay(10);
	
  updateRange();  																				//sets class range value
  
}

/* ------------------------------------------------------------ */
/*  getX()
**
**  Parameters:
**    none
**
**  Return Value:
**    int result - an integer value of the read X data from getData
**
**  Errors:
**    none
**
**  Description:
**   	call getData with the XDATA_H and XDATA_L registers and return that value
*/
int ACL2::getX(){
	int result;	
	
	//get data from xdata register
	result = getData(XDATA_H, XDATA_L);	
	
	//process to achieve desired reading
	result = result * 1000;
	result = result / (2000 / range);
	result = result + xZero;
	
	return result;
}

/* ------------------------------------------------------------ */
/*  getY()
**
**  Parameters:
**    none
**
**  Return Value:
**    int result - an integer value of the read Y data from getData
**
**  Errors:
**    none
**
**  Description:
**   	call getData with the YDATA_H and YDATA_L registers and return that value
*/
int ACL2::getY(){	

	int result = 0;
	
	//get data from ydata register
	result = getData(YDATA_H, YDATA_L);
	
	//process to achieve desired reading
	result = result * 1000;
	result = result /(2000 / range);
	result = result + yZero;
	
	return result;
}

/* ------------------------------------------------------------ */
/*  getZ()
**
**  Parameters:
**    none
**
**  Return Value:
**    int result - an integer value of the read Z data from getData
**
**  Errors:
**    none
**
**  Description:
**   	call getData with the ZDATA_H and ZDATA_L registers and return that value
*/
int ACL2::getZ(){	
	
	int result = 0;
	
	//get data from ydata register
	result = getData(ZDATA_H, ZDATA_L);	
	
	//process to achieve desired reading
	result = result * 1000;
	result = result / (2000 / range);
	result = result + zZero;
	
	return result;
	
}

/* ------------------------------------------------------------ */
/*  getTemp()
**
**  Parameters:
**    none
**
**  Return Value:
**    int result - an integer value of the read temp data from getData
**
**  Errors:
**    none
**
**  Description:
**   	call getData with the TEMP_H and TEMP_L registers and return that value
*/
int ACL2::getTemp(){
	
	//call getData with the two Temp data registers high then low
	return getData(TEMP_H, TEMP_L);
	
}

/* ------------------------------------------------------------ */
/*  getStatus()
**
**  Parameters:
**    none
**
**  Return Value:
**   	uint8_t result - 8 bit binary from status register
**
**  Errors:
**    none
**
**  Description:
**   	read the STATUS register and return that value
*/
uint8_t ACL2::getStatus(){	
	
	//get the data from the STATUS register
	return readRegister(STATUS);	
	
}

/* ------------------------------------------------------------ */
/*  getRange()
**
**  Parameters:
**   none
**
**  Return Value:
**   uint8_t Range: the range sensitivity of the ACL2
**
**  Errors:
**    none
**
**  Description:
**   	Grabs the range value from the class and returns it
**		
*/
uint8_t ACL2::getRange(){
	return range;
}

/* ------------------------------------------------------------ */
/*  readRegister()
**
**  Parameters:
**    thisRegister - address of the register to read from
**
**  Return Value:
**    uint8_t result - byte from register
**
**  Errors:
**    none
**
**  Description:
**   	This function reads a byte from a given register and returns the byte read
**		
*/
uint8_t ACL2::readRegister(uint8_t thisRegister){
	
  uint8_t inByte = 0; //byte from register
  
  
  //set cs low
  digitalWrite((uint8_t)chipSelect, LOW);

  //send instruction type
  SPI.transfer(READ);
  //send address of register
  SPI.transfer(thisRegister);
  // send a value of 0 to read the first byte returned:
  inByte = SPI.transfer(0);
  
  digitalWrite(chipSelect, HIGH);
  
  return(inByte);   
  
}

/* ------------------------------------------------------------ */
/*  writeRegister()
**
**  Parameters:
**    	thisRegister - register address to be written to
**		thisValue - byte to be written to register
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	this function writes a byte to a register given by thisRegister's address.
*/
void ACL2::writeRegister(uint8_t thisRegister, uint8_t thisValue){	
	
	//set chip select pin low
	digitalWrite((uint8_t)chipSelect, LOW);

	//send Write instruction
	SPI.transfer(WRITE);	
	
	//Send address to write to
	SPI.transfer(thisRegister);
	
	//Send Data to write to register
	SPI.transfer(thisValue);  
	
	// take the chip select high to de-select:
	digitalWrite(chipSelect, HIGH);	
	
}

/* ------------------------------------------------------------ */
/*  reset()
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	writes the byte 'R' to the reset register to initiate a soft reset. Then call
**		init to set the sensor up for measurement again
*/
void ACL2::reset(){
	
	//write 'R' to soft reset register
	writeRegister(SOFT_RESET, 'R');
	
	//go through init sequence
	init();
	
}

/* ------------------------------------------------------------ */
/*  updateRange()
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	Reads the filter control register and stores the sensitivity range into  
**		a private variable
*/
void ACL2::updateRange(){
	
	uint8_t value;
	value = readRegister(FILTER_CTL);
	
	//only looking at first two bits. 192 = 0b11000000 = 0xC0
	value = value & 0xC0;
	
	//set range based on register value
	switch(value){
		case 0xC0:
			range = 8;
			break;
			
		case 0x80:
			range = 8;
			break;
		
		case 0x40:
			range = 4;
			break;
		
		case 0:
			range = 2;
			break;
	}	
}

/* ------------------------------------------------------------ */
/*  setRange()
**
**  Parameters:
**    int newRange: The new sensitivity range for the ACL to use (2,4,8)
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	takes in the user choice newRange and if it is valid, writes to the filter
**		control to change the sensitivity range while keeping other filter preferences
*/
void ACL2::setRange(int newRange){
	
	uint8_t temp = 0;
	
	//get range control data
	temp = readRegister(FILTER_CTL);
	
	//modify the temp data to change to desired range
	switch(newRange){
		case 2: 
			temp = temp & 0x3F;
			break;
		case 4:
			temp = temp & 0x3F;
			temp = temp | 0x40;
			break;
		case 8:
			temp = temp | 0x80;
			break;
		default:
			break;				//if not a valid range, will just set the FILTER_CTL register to what it was
	}
	
	//write modified temp back to FILTER_CTL
	writeRegister(FILTER_CTL, temp);

	//set class range value
	updateRange();
	
}

/* ------------------------------------------------------------ */
/*  setZero()
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	Sets the zeroing variables so that the ACL puts out x= 0, y = 0, z = 1000
**		has a 100 point sample
*/
void ACL2::setZero(){
	//math sets the current acceleration to read zero
	
	int xAvg = 0;
	int yAvg = 0;
	int zAvg = 0;
	
	
	for(int i = 0; i < 100; i ++){
			xAvg =  xAvg + getX();
			yAvg = yAvg + getY();
			zAvg = zAvg + getZ();
	}
	xAvg = xAvg / 100;
	yAvg = yAvg / 100;
	zAvg = zAvg / 100;
	
		
	xZero = 0 - (xAvg - xZero);
	yZero = 0 - (yAvg - yZero);
	zZero = 1000 - (zAvg - zZero);	
}



/* ------------------------------------------------------------ */
/*  getFIFOentries()
**
**  Parameters:
**    none
**
**  Return Value:
**    int result: the number of FIFO entries stored in the buffer
**
**  Errors:
**    none
**
**  Description:
**   	This function reads the FIFO_ENTRIES registers and returns the value inside
**		
*/
int ACL2::getFIFOentries(){
	
	return getData(FIFO_ENTRIES_H, FIFO_ENTRIES_L);		

}

/* ------------------------------------------------------------ */
/*  initFIFO()
**
**  Parameters:
**    none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	This function manipulates the FIFO and control registers to start the FIFO buffer in a streaming mode
**		
*/
void ACL2::initFIFO(){

	//set interupt1 pin to data ready
	writeRegister(INTMAP1, 1);
	
	//turn on FIFO
	writeRegister(FIFO_CONTROL, 10);
	writeRegister(FIFO_SAMPLES, 255);		//set to 512 values for each

}

/* ------------------------------------------------------------ */
/*  fillFIFO()
**
**  Parameters:
**	   none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**   	This function initiates the FIFO read then reads and processes the data
**		coming out of the FIFO buffer and places them into their respective myQueue
*/
void ACL2::fillFIFO(){		
	
	uint16_t buffer = 0;
	uint16_t LSB = 0;
	int result = 0;	
	int sign = 0;
	char dir = '\0';
	int samples = 0;
	int i = 0;
	
	//get the number of samples
	samples = getFIFOentries();	
	
	if(samples > 0){
		
		//lower chip select and send FIFO_READ byte. 
		//->chipSelect needs to stay low throughout the transfer
		digitalWrite(chipSelect, LOW);
		SPI.transfer(FIFO_READ);
		
		while(i < (samples)){		
			//read the 8 LSBs in LSB buffer
			LSB = SPI.transfer(0);
			//read the 8 MSBs into buffer
			buffer = SPI.transfer(0);
			
			//shift MSBs to correct position then OR with LSB
			buffer = buffer << 8;
			buffer = buffer | LSB;
			
			//receive axis of data
			dir = getDIR(buffer);
			
			//get rid of first two directional bits
			buffer = buffer & 0b0011111111111111;
			
			//check to see if value is negative and if so run through twosToBin
			if(buffer > 8192){
				sign = 1;
				buffer = twosToBin(buffer);
			}		
			
			//copy and mask buffer data into a signed int for storing
			result = buffer & 0x07ff;
			
			//account for negative
			if(sign == 1){
				result = 0 - result;
			}
			//scales data
			result *= (1000 / (2000 / range));
			//processes the data and puts it into the correct myQueue according to DIR
			if(dir == 'x'){				
				result = result + xZero;
				xFIFO.push_back(result);
			}			
			else if(dir == 'y'){				
				result = result + yZero;
				yFIFO.push_back(result);
			}
			else if(dir == 'z'){
				result = result + zZero;
				zFIFO.push_back(result);
			}
			else{
			}
					
			//increment counter
			i = i + 1;
			
			sign = 0;
			dir = '\0';
		}
		//set chip select high again once FIFO transfer is over
		digitalWrite(chipSelect, HIGH);
		
	}
	return;
}

/* ------------------------------------------------------------ */
/*  getData()
**
**  Parameters:
**    	reg1 - first register to read from. the high data value which contains 
**		the 3 MSBs
**		reg2 - second register to read from. the low data value which contains
**		the 8 LSBs
**
**  Return Value:
**    int result - an integer value of the combined data in the two registers
**
**  Errors:
**    none
**
**  Description:
**   	get Data is used to get the 11 bit x,y,z and temp values that come from 2 registers
**		from the accelerometer. The function spits out a signed integer value
*/
int ACL2::getData(uint8_t reg1, uint8_t reg2){
	
	//create variable to be returned
	uint16_t buffer = 0;
	int result = 0;	
	int sign = 0;
	
	//store XData high into result
	buffer = readRegister(reg1);
	
	//Shift over 8 to make room for lower bits
	buffer = buffer << 8;

	//store XData low into 8 LSB
	buffer = readRegister(reg2) | buffer;
	
	if(buffer >= 0x8000){// If negative
		sign = 1;
		buffer = twosToBin(buffer);
	}
	
	//Get rid of first 4 bits
	result = buffer & 0x07ff;

	if(sign == 1){
		result = 0 - result;
	}
	
	return result;	

}

/* ------------------------------------------------------------ */
/*  twosToBin()
**
**  Parameters:
**    input - an 11 bit twos complement value to be converted to a binary number
**
**  Return Value:
**    returns a 16 bit unsigned integer with the positive value of the negative twos compliment
**
**  Errors:
**    none
**
**  Description:
**   	This function converts a negative twos compliment value and preforms a bitwise flip and subtracts
**		one to return the positive int value.
*/
uint16_t ACL2::twosToBin(uint16_t input){	
	
	//flip all 11 bits
	input = input ^ 0x07ff;
	
	//subtract 1 to get binary
	input = input - 1;
	
	return input;	
}

/* ------------------------------------------------------------ */
/*  getDIR()
**
**  Parameters:
**    uint16_t value: FIFO raw data to parse direction from
**
**  Return Value:
**    char result: axis that the FIFO data represents
**
**  Errors:
**    does not handle temperature FIFO data
**
**  Description:
**   	This function takes the raw FIFO data and analyses the 2 MSBs to determine
**		the axis the data represents
*/
char ACL2::getDIR(uint16_t value){
	char result = '\0';	
	
	//only care about first 2 bits
	value = value >> 14;
	
	if(value  == 0)
		result = 'x';
	if(value == 1)
		result = 'y';
	if(value ==  2)
		result = 'z';
	if(value == 3)
		result = 't';
	
	return result;	

}

/* ------------------------------------------------------------ */
/* 	myQueue()
**
**  Parameters: 
**		none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Constructor for queue sets the pointers to initial positions
**		and clears the queue by calling resetQueue()
*/
myQueue::myQueue(){		
	resetQueue();
	return;
}

/* ------------------------------------------------------------ */
/* 	empty()
**
**  Parameters: 
**		none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    sets all members of the queue to -1
**	   resets the pointers to the beginning
*/
void myQueue::empty(){
	
	//traverse through queue and set to -1
	for(int i = 0; i < 512; i ++){
		dataQueue[i] = -999999;
	}		
	
	//set ptrs to the beginning
	tail_ptr = 0;
	head_ptr = 0;
	return;
}

/* ------------------------------------------------------------ */
/* 	size()
**
**  Parameters: 
**		none
**
**  Return Value:
**    int size: number of items in the queue
**
**  Errors:
**    none
**
**  Description:
**    returns the location of the tail_ptr which equals number of items
**		
*/
int myQueue::size(){
	return tail_ptr;
}

/* ------------------------------------------------------------ */
/* 	front()
**
**  Parameters: 
**		none
**
**  Return Value:
**    int front: the value in the front of the queue
**
**  Errors:
**    none
**
**  Description:
**    returns the first item in the queue
**		
*/
int myQueue::front(){
	return dataQueue[head_ptr];
}

/* ------------------------------------------------------------ */
/* 	back()
**
**  Parameters: 
**		none
**
**  Return Value:
**    int back: the last item in the queue
**
**  Errors:
**    none
**
**  Description:
**    returns the last item in the queue
**		
*/
int myQueue::back(){
	return dataQueue[tail_ptr - 1];
}

/* ------------------------------------------------------------ */
/* 	push_back()
**
**  Parameters: 
**		int value: item to be pushed onto the back of the queue
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    accepts a value then pushes it onto the back of the queue
**	
*/
void myQueue::push_back(int value){
		dataQueue[tail_ptr] = value;
		if (tail_ptr != back_ptr){
			tail_ptr = tail_ptr + 1;
		}
		return;
}

/* ------------------------------------------------------------ */
/* 	pop_front()
**
**  Parameters: 
**		none
**
**  Return Value:
**    int front: item that is popped off front of the queue
**
**  Errors:
**    none
**
**  Description:
**    Takes the item first in the queue and returns it then 
**    moves the head_ptr to the next item in the queue
**		
*/
int myQueue::pop_front(){
	
	//store front value to return
	int result = dataQueue[head_ptr];
	
	//if queue not empty, move head ptr back one over
	//new first item
	if(tail_ptr > head_ptr){
		head_ptr = head_ptr + 1;	
	}
	//if the queue is empty reset pointers to start again
	

	else{
		//Serial.println("The queue is empty");
		resetQueue();
		return 0;
	}	
	
	return result;
}

/* ------------------------------------------------------------ */
/* 	resetQueue()
**
**  Parameters: 
**		none
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    sets pointer values to start over queue then empties it by calling empty()
**	
*/
void myQueue::resetQueue(void)
{
	//initialize pointers
	back_ptr = 511;
	front_ptr = 0;
	tail_ptr = 0;
	head_ptr = 0;
	
	//clear queue
	empty();
	return;
}

/* ------------------------------------------------------------ */
/* 	getQueue()
**
**  Parameters: 
**		int* outQueue	an integer array to copy data to
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    accepts an integer array pointer then empties the dataQueue into the array. 
**	
*/
void  myQueue::getQueue(int* outqueue){
	
	//traverse queue and print and pop
	for(int i = 0; i < (size()); i ++){
		outqueue[i]=pop_front();
	}
	resetQueue();
	
	return;
}

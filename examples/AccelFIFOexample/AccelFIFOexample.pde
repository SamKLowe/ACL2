#include <ACL2.h>

/**************************************************/
/* PmodACL2FIFO Demo                                   */
/**************************************************/
/*    Author: Samuel Lowe                         */
/*    Copyright 2014, Digilent Inc.               */
/*                                                */
/*   Made for use with chipKIT Pro MX3            */
/*   PmodACL2 on connector JC                     */
/**************************************************/
/*  Module Description:                           */
/*                                                */
/*    This module implements a demo application   */
/*    of the PmodACL2 FIFO buffer functionality   */
/*                                                */
/*  Functionality:                                */
/*                                                */  
/*    This module initializes the PmodACL2        */
/*    then reads the x, y, z FIFO buffers         */
/*    into int arrays then prints them out.       */
/*    The sample data will be at a 100 kHz        */
/*    speed. Note the baude rate is 115200        */
/*                                                */
/**************************************************/
/*  Revision History:                             */
/*                                                */
/*      10/14/2014(SamL): Created                 */
/*                                                */
/**************************************************/

// the sensor communicates using SPI, so include the library:
#include <SPI.h>



const int chipSelectPin = SS;




ACL2 myACL;


void setup() {
  //baud rate at 115200 to allow 100kHz to be shown.
  //currently 9600 baud breaks the fifo code
  Serial.begin(115200);
  int test = 0;
  SPI.begin();// start the SPI library
  
  // initalize the  data ready and chip select pins
  pinMode(chipSelectPin, OUTPUT);
 

  // initialize sensor
  myACL.begin(chipSelectPin);
  myACL.setZero();
  Serial.println(myACL.getStatus(), BIN);
  myACL.initFIFO();
  delay(100);
}

void loop() {
 
  
  //create arrays
  int xqueue[512];
  int yqueue[512];
  int zqueue[512];
  int length = 0;
  int i = 0;
 
 //populate myQueue elements
 myACL.fillFIFO();
 
 //pop myQueues into the user arrays
 length = myACL.xFIFO.size();
 
 //pass in arrays to fill up
 myACL.xFIFO.getQueue(xqueue);
 myACL.yFIFO.getQueue(yqueue);
 myACL.zFIFO.getQueue(zqueue);
 
 //print the arrays
 Serial.println("X FIFO: ");
 for(i = 0; i < length; i ++){
  Serial.println(xqueue[i]);
 }
 
 Serial.println("Y FIFO: ");
 for(i = 0; i < length; i ++){
  Serial.println(yqueue[i]);
 }
 
 Serial.println("Z FIFO: ");
 for(i = 0; i < length; i ++){
  Serial.println(zqueue[i]);
 }
  

//reset arrays
for(i; i < length; i++){
 xqueue[i] = -1;
 yqueue[i] = -1;
 zqueue[i] = -1;
} 
 
 delay(100);
  
 
}



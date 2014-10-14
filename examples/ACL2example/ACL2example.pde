#include <ACL2.h>


/**************************************************/
/* PmodACL2 Demo                                   */
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
/*    of the PmodACL2.                            */
/*                                                */
/*  Functionality:                                */
/*                                                */  
/*    This module initializes the PmodACL2        */
/*    then reads the x, y, z and temperature      */
/*    data and prints it continuosly to the       */
/*    serial monitor                      `       */
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
Serial.begin(9600);
  int test = 0;
 
  
  // initalize the  data ready and chip select pins
  pinMode(chipSelectPin, OUTPUT);
  

  // initialize sensor
  myACL.begin(chipSelectPin);
  Serial.println(myACL.getStatus(), BIN);
  myACL.setZero();
  delay(100);
}

void loop() {
 
  //initialize variables
  
  int x = 0;
  int y = 0;
  int z = 0;
  int temp = 0;
  
  //store values into variables
  x = myACL.getX(); 
  y = myACL.getY(); 
  z = myACL.getZ(); 
  temp = myACL.getTemp();
  
  
  //print
  Serial.print("X: "); Serial.println(x);
  Serial.print("Y: "); Serial.println(y);
  Serial.print("Z: "); Serial.println(z);
  Serial.print("Temp: "); Serial.println(temp);
 
 
  
 
}



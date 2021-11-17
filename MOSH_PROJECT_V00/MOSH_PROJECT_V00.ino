/////////MOSH Project - Baptite Lerat - Florian Convert - Ewan Mackay\\\\\\\\\\\
//////////////////////////////// 17 - 11 - 2021 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 
#include <Arduino.h>
#include <SoftwareSerial.h>

/*
 * DEFINE
 */
#define TX1PIN 2
#define RX1PIN 3
#define RSTPIN 4

/*
 * Prototypes
 */

/*
 * Déclaration de variables
 */
SoftwareSerial loraSerial(TX1PIN, RX1PIN);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(57600);

  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(1000);
  pinMode(7,INPUT);
  
  loraSerial.begin(57600);   

  loraSerial.print(0x55);

  loraSerial.print("sys get ver\r\n");
  char * c = loraSerial.read();
  Serial.println(c);
  
  Serial.println("Done");

}

void loop() {
  // put your main code here, to run repeatedly:

}

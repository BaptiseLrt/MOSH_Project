/*
 * Author: JP Meijers
 * Date: 2016-02-07
 * Previous filename: TTN-Mapper-TTNEnschede-V1
 *
 * This program is meant to be used with an Arduino UNO or NANO, conencted to an RNxx3 radio module.
 * It will most likely also work on other compatible Arduino or Arduino compatible boards, like The Things Uno, but might need some slight modifications.
 *
 * Transmit a one byte packet via TTN. This happens as fast as possible, while still keeping to
 * the 1% duty cycle rules enforced by the RN2483's built in LoRaWAN stack. Even though this is
 * allowed by the radio regulations of the 868MHz band, the fair use policy of TTN may prohibit this.
 *
 * CHECK THE RULES BEFORE USING THIS PROGRAM!
 *
 * CHANGE ADDRESS!
 * Change the device address, network (session) key, and app (session) key to the values
 * that are registered via the TTN dashboard.
 * The appropriate line is "myLora.initABP(XXX);" or "myLora.initOTAA(XXX);"
 * When using ABP, it is advised to enable "relax frame count".
 *
 * Connect the RN2xx3 as follows:
 * RN2xx3 -- Arduino
 * Uart TX -- 10
 * Uart RX -- 11
 * Reset -- 12
 * Vcc -- 3.3V
 * Gnd -- Gnd
 *
 * If you use an Arduino with a free hardware serial port, you can replace
 * the line "rn2xx3 myLora(mySerial);"
 * with     "rn2xx3 myLora(SerialX);"
 * where the parameter is the serial port the RN2xx3 is connected to.
 * Remember that the serial port should be initialised before calling initTTN().
 * For best performance the serial port should be set to 57600 baud, which is impossible with a software serial port.
 * If you use 57600 baud, you can remove the line "myLora.autobaud();".
 *
 */
#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define LORARST 7
#define GASPIN A0
#define INTPIN 2

SoftwareSerial mySerial(6, 5); // RX, TX

//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);
bool danger_detected;


void gas_handler(void){
    danger_detected=true;
}

// the setup routine runs once when you press reset:
void setup()
{
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  initialize_radio();
 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  //transmit a startup message
  myLora.tx("TTN Mapper on TTN Enschede node");
  
  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(LORARST, OUTPUT);
  digitalWrite(LORARST, LOW);
  delay(500);
  digitalWrite(LORARST, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;


  /*
   * ABP: initABP(String addr, String AppSKey, String NwkSKey);
   * Paste the example code from the TTN console here:
   */
  const char *devAddr = "260B3B60";
  const char *nwkSKey = "10E106FE42AFE00C592F6776A6B1767E";
  const char *appSKey = "CDA82056ED3A976F7FAB18E7B9F658D4";

  join_result = myLora.initABP(devAddr, appSKey, nwkSKey);

  /*
   * OTAA: initOTAA(String AppEUI, String AppKey);
   * If you are using OTAA, paste the example code from the TTN console here:
   */
  //const char *appEui = "70B3D57ED00001A6";
  //const char *appKey = "A23C96EE13804963F8C2BD6285448198";

  //join_result = myLora.initOTAA(appEui, appKey);


  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");
}

// the loop routine runs over and over again forever:
void loop()
{
  uint16_t gval = get_gas_value();

  Serial.print("value :");Serial.println(gval);
  Serial.print("Danger:");Serial.println(danger_detected);
  
  if(danger_detected){ //On transmet de la valeur uniquement pendant un danger
 
    detachInterrupt(digitalPinToInterrupt(INTPIN)); //On enleve l'interruption pour pas qu'elle soit activé tout le temps. 
    Serial.println("J'ai bien dormi");
    delay(1000);
    Serial.println("TXing");
   // myLora.tx(String(get_gas_value())); //one byte, blocking function
    byte my_value[2];   
    my_value[1]=highByte(gval);
    Serial.println(my_value[1], HEX);
    my_value[0]=lowByte(gval);
    Serial.println(my_value[0], HEX);
    myLora.txBytes(my_value, 2);
    Serial.println(gval);
    delay(1000);
  }
  
  if(gval<650){ //Une fois le danger écarté, on peut sleep
    Serial.println("On veut dormir");
    danger_detected=false;
    delay(1000);
    go_sleep();
 }
  
  delay(1000);
}

void go_sleep(){
  
  attachInterrupt(digitalPinToInterrupt(INTPIN), gas_handler, RISING);  
  Serial.println("Going to sleep");
  delay(1000);

  sleep_mode();
}

uint16_t get_gas_value(void){
  float gvalue=0;
  gvalue=analogRead(GASPIN);
  
  return (gvalue/1024.0*5.0)*1000;
}

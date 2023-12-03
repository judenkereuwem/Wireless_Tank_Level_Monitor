

#include "BluetoothSerial.h"
 
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

const int LED = 21;
const int TrigPin = 5;//Trig attach to pin5
const int EchoPin = 18;//Echo attach to pin18
 
// Bluetooth Serial Object (Handle)
BluetoothSerial SerialBT;
 
// ESP32 Bluetooth (Slave) Device Information
const char *pin = "1234"; // Change this to more secure PIN.
String device_name = "ESP32-BT-Slave";
 

 
void setup() {

  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  pinMode(TrigPin,OUTPUT);
  pinMode(EchoPin,INPUT);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  SerialBT.setPin(pin);
  Serial.println("Using PIN");
  
  digitalWrite(LED, HIGH);
  delay(2000);
  digitalWrite(LED, LOW);
}
 
void loop() {

  digitalWrite(TrigPin,LOW);
   delayMicroseconds(2);
   digitalWrite(TrigPin,HIGH);
   delayMicroseconds(10);
   digitalWrite(TrigPin,LOW);
 
   long duration = pulseIn(EchoPin, HIGH); 
   int distance = (duration/2)/29.1;

   distance = constrain(distance, 0, 20);

   int dataToSend = distance;
   SerialBT.write((uint8_t*)&dataToSend, sizeof(dataToSend));
   delay(20);
    
   Serial.println(distance);
   delay(200);

}

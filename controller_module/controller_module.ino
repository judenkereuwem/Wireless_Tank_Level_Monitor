
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
 
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif
  
const int switch1Pin = 5;
const int switch2Pin = 17;

const int buzzer = 4;

String pSource = "GRID";
String pump = "OFF";

const int power_relay = 2;
const int pump_relay = 18;

const int tankHeight = 20; //set according to tank size

String info = "";

 
BluetoothSerial SerialBT;
const char *pin = "1234"; // Change this to reflect the pin expected by the real slave BT device
String slaveName = "ESP32-BT-Slave"; // Change this to reflect the real name of your slave BT device
String myName = "ESP32-BT-Master";

void setup() {
  bool connected;
  Serial.begin(115200);
  SerialBT.begin(myName, true);
  Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", myName.c_str());
  SerialBT.setPin(pin);
  Serial.println("Using PIN");
  
  pinMode(pump_relay, OUTPUT);
  pinMode(power_relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP);

  digitalWrite(pump_relay, HIGH);
  digitalWrite(power_relay, HIGH);
  

  lcd.init();
  lcd.backlight();

  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WaterL: ");

  lcd.setCursor(0, 1);
  lcd.print("Power: ");

  lcd.setCursor(0, 2);
  lcd.print("Pump: ");

  lcd.setCursor(0, 3);
  lcd.print("Info: ");


  // connect(address) is fast (up to 10 secs max), connect(slaveName) is slow (up to 30 secs max) as it needs
  // to resolve slaveName to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices Bluetooth address and device names
  connected = SerialBT.connect(slaveName);
  Serial.printf("Connecting to slave BT device named \"%s\"\n", slaveName.c_str());
  if(connected) {
    Serial.println("Connected Successfully!");
  } else {
    while(!SerialBT.connected(3000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }
  // Disconnect() may take up to 10 secs max
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Successfully!");
  }
  // This would reconnect to the slaveName(will use address, if resolved) or address used with connect(slaveName/address).
  SerialBT.connect();
  if(connected) {
    Serial.println("Reconnected Successfully!");
  } else {
    while(!SerialBT.connected(3000)) {
      Serial.println("Failed to reconnect. Make sure remote device is available and in range, then restart app.");
    }
  }

  //digitalWrite(buzzer, HIGH);
  //delay(300);
  //digitalWrite(buzzer, LOW);
}



void loop() {

  const int grid_switch = digitalRead(switch1Pin);
  const int solar_switch = digitalRead(switch2Pin);

  int receivedData;
  int percent;

  
  if (SerialBT.available()){
    
    SerialBT.readBytes((char*)&receivedData, sizeof(receivedData));
    Serial.print("Data: ");
    Serial.print(receivedData);

    percent = map(receivedData, 3, tankHeight, 100, 1);
    percent = constrain(percent, 1, 100);
    Serial.print("  Percent: ");
    Serial.println(percent);
  }

  else if(!SerialBT.connected()){
      Serial.print(percent);
      Serial.println("   disconnected");
      digitalWrite(pump_relay, HIGH);
      digitalWrite(buzzer, HIGH);

      lcd.setCursor(6, 2);
      lcd.print("            ");
      lcd.setCursor(6, 2);
      lcd.print("OFF");
      lcd.setCursor(6, 3);
      lcd.print("Sensor Offline");
  }

    lcd.setCursor(8, 0);
    lcd.print("            ");
    lcd.setCursor(8, 0);
    lcd.print(percent);
    lcd.setCursor(12, 0);
    lcd.print("%");

    lcd.setCursor(8, 1);
    lcd.print("          ");
    lcd.setCursor(8, 1);
    lcd.print(pSource);
   


   if ((percent < 50)&&(percent > 40)){
    pump = "Pumping";
    digitalWrite(pump_relay, LOW);
    digitalWrite(buzzer, LOW);
    lcd.setCursor(6, 2);
    lcd.print("            ");
    lcd.setCursor(6, 2);
    lcd.print(pump);
    lcd.setCursor(6, 3);
    lcd.print("              ");
   }

  else if (percent == 100){
    pump = "OFF";
    digitalWrite(pump_relay, HIGH);
    digitalWrite(buzzer, LOW);
    lcd.setCursor(6, 2);
    lcd.print("            ");
    lcd.setCursor(6, 2);
    lcd.print(pump);
    lcd.setCursor(6, 3);
    lcd.print("              ");
    lcd.setCursor(6, 3);
    lcd.print("              ");
   }


   else if ((percent < 40)&&(percent > 0)) {
    digitalWrite(pump_relay, LOW);
    digitalWrite(buzzer, HIGH);
    pump = "Not pumping";
    info = "Check pump PWR";

    lcd.setCursor(6, 2);
    lcd.print("            ");
    lcd.setCursor(6, 2);
    lcd.print(pump);
    lcd.setCursor(6, 3);
    lcd.print("              ");
    lcd.setCursor(6, 3);
    lcd.print(info);
   }

  

  //Select pump power source
  if (grid_switch == 0){
    pSource = "Grid";
    digitalWrite(power_relay, HIGH);
    delay(300);
  }
  
  else if (solar_switch == 0){
    pSource = "Solar";
    digitalWrite(power_relay, LOW);
    delay(300);
  }
   
  delay(100);

}

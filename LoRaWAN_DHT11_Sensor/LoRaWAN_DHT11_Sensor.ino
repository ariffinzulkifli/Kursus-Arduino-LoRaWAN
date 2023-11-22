/*
* LORAWAN AND DATA VISUALISATION
* 
* Penulis: Mohamad Ariffin Zulkifli
* Syarikat: Myinvent Technologies Sdn Bhd
* 
*/

#include <SimpleDHT.h>
#include <SoftwareSerial.h>

SimpleDHT11 dht11(2);

String inputString = "";
bool stringComplete = false;

long previousTTN = millis();
long uplinkInterval = 10000;

bool receiveCallback = false;
bool getDataStatus = false;

bool networkJoinedStatus = false;

SoftwareSerial LA66(10, 11);

char rxbuff[128];
uint8_t rxbuff_index = 0;

byte temperature, humidity;

void setup() {
  Serial.begin(115200);
  LA66.begin(9600);
  
  inputString.reserve(200);
  
  // Reset LA66 MCU
  LA66.println("ATZ");

}

void loop() {
  // Handling LoRaWAN communication
  LA66.listen();

  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)  {
    // Serial.print("Read DHT11 failed, err=");
    // Serial.print(SimpleDHTErrCode(err));
    // delay(1000);
    return;
  }
  
  if((millis() - previousTTN >= uplinkInterval) && (networkJoinedStatus == 1)){
    previousTTN = millis();
    getDataStatus = false;

    Serial.println("\n===== AMBIENT SENSOR PARAMETERS");
    Serial.println("Temperature: " + String((int)temperature) + " °C");
    Serial.println("Humidity: " + String((int)humidity) + " %");
    
    char sensor_data_buff[128] = "\0";

    Serial.println("===== SEND DATA TO TTN");

    // AT+SENDB=<confirn_status>,<Fport>,<data_len>,<data>
    snprintf(sensor_data_buff, sizeof(sensor_data_buff),
         "AT+SENDB=%d,%d,%d,%02X%02X%02X%02X",
         0, 2, 4,  // Confirm status, Fport, data length (4 bytes in total)
         (temperature >> 8) & 0xFF, temperature & 0xFF,
         (humidity >> 8) & 0xFF, humidity & 0xFF
        );
    
    LA66.println(sensor_data_buff);
  }

  if(receiveCallback == true){
    receiveCallback = false;
    getDataStatus = true;
    delay(1000);
    
    LA66.println("AT+CFG");    
  }

  while (LA66.available()) {
    char inChar = (char) LA66.read();
    inputString += inChar;

    rxbuff[rxbuff_index++]=inChar;

    if(rxbuff_index>128)
    break;
    
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
      rxbuff[rxbuff_index]='\0';
      
      if(strncmp(rxbuff, "JOINED", 6) == 0){
        networkJoinedStatus=1;
      }

      if(strncmp(rxbuff, "Dragino LA66 Device", 19) == 0){
        networkJoinedStatus=0;
      }

      if(strncmp(rxbuff, "Run AT+RECVB=? to see detail", 28) == 0){
        receiveCallback=true;
        stringComplete=false;
        inputString = "\0";
      }

      if(strncmp(rxbuff,"AT+RECVB=",9)==0){       
        stringComplete=false;
        inputString = "\0";
        Serial.print("\r\nGet downlink data(FPort & Payload) ");
        Serial.println(&rxbuff[9]);
      }
      
      rxbuff_index=0;

      if(getDataStatus==true){
        stringComplete=false;
        inputString = "\0";
      }
    }
  }

  while (Serial.available()) {
    char inChar = (char) Serial.read();
    inputString += inChar;
    if (inChar == '\n' || inChar == '\r') {
      LA66.print(inputString);
      inputString = "\0";
    }
  }

  if (stringComplete) {
    Serial.print(inputString);
    
    inputString = "\0";
    stringComplete = false;
  }
}

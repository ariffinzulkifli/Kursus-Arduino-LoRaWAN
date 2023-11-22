/*
* MJII - LORAWAN AND DATA VISUALISATION
* 
* Penulis: Mohamad Ariffin Zulkifli
* Syarikat: Myinvent Technologies Sdn Bhd
* 
*/

#include <SoftwareSerial.h>

String inputString = "";
bool stringComplete = false;

long old_time = millis();
long new_time;

long uplink_interval=10000;

bool receiveCallback = false;
bool getDataStatus = false;

bool networkJoinedStatus = false;

SoftwareSerial LA66(10, 11);

char rxbuff[128];
uint8_t rxbuff_index = 0;

void setup(){
  // Initialize serial
  Serial.begin(115200);
  LA66.begin(9600);
  
  inputString.reserve(200);
  
  // Reset LA66 MCU
  LA66.println("ATZ");

  pinMode(13, OUTPUT);
}

void loop() {
  // Handling LoRaWAN communication
  LA66.listen();

  new_time = millis();

  if((new_time-old_time>=uplink_interval)&&(networkJoinedStatus==1)){
    old_time = new_time;
    getDataStatus=false;
    LA66.println("AT+SENDB=0,2,1,00");  //confirm status, Fport,  payload length, payload(HEX)
  }

  if(receiveCallback == true){
    receiveCallback = false;
    getDataStatus = true;
    delay(1000);
    
    LA66.println("AT+CFG");    
  }

  while (LA66.available()){
    char inChar = (char) LA66.read();
    inputString += inChar;

    rxbuff[rxbuff_index++]=inChar;

    if(rxbuff_index>128)
    break;
    
    if (inChar == '\n' || inChar == '\r'){
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
        Serial.println("\r\n===== RECEIVE DOWNLINK DATA");
        Serial.print("(Port & Payload) ");
        Serial.println(&rxbuff[9]);

        if (strncmp(&rxbuff[11], "1301", 4) == 0){
          Serial.println("Relay ON");
          digitalWrite(13, HIGH);
        }
        else if (strncmp(&rxbuff[11], "1300", 4) == 0){
          Serial.println("Relay OFF");
          digitalWrite(13, LOW);
        }
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
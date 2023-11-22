/*
* LORAWAN AND DATA VISUALISATION
* 
* Penulis: Mohamad Ariffin Zulkifli
* Syarikat: Myinvent Technologies Sdn Bhd
* 
*/

#include <SoftwareSerial.h>

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

#define sensorFrameSize     19
#define sensorWaitingTime   1000
#define sensorID            0x01
#define sensorFunction      0x03
#define sensorByteResponse  0x0E

SoftwareSerial sensor(2, 3);

unsigned char byteRequest[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
unsigned char byteResponse[19] = {};

int moisture, temperature, ph, ec, nitrogen, phosphorus, potassium;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  sensor.begin(4800);
  LA66.begin(9600);
  
  inputString.reserve(200);
  
  // Reset LA66 MCU
  LA66.println("ATZ");  
}

void loop() {
  // Handling LoRaWAN communication
  LA66.listen();

  if((millis() - previousTTN >= uplinkInterval) && (networkJoinedStatus == 1)){
    previousTTN = millis();
    getDataStatus = false;

    Serial.println("\n===== SOIL SENSOR PARAMETERS");
    Serial.print("Byte Request: ");

    String requestString;
    for (int j = 0; j < 8; j++) {
      requestString += byteRequest[j] < 0x10 ? " 0" : " ";
      requestString += String(byteRequest[j], HEX);
      requestString.toUpperCase();
    }
    Serial.println(requestString);

    sensor.write(byteRequest, 8);

    sensor.listen();

    unsigned long resptime = millis();
    while ((sensor.available() < sensorFrameSize) && ((millis() - resptime) < sensorWaitingTime)) {
      delay(1);
    }

    while (sensor.available()) {
      for (int n = 0; n < sensorFrameSize; n++) {
        byteResponse[n] = sensor.read();
      }

      if (byteResponse[0] != sensorID || byteResponse[1] != sensorFunction || byteResponse[2] != sensorByteResponse) {
        return;
      }

      Serial.print("Byte Response: ");

      String responseString;
      for (int j = 0; j < sensorFrameSize; j++) {
        responseString += byteResponse[j] < 0x10 ? " 0" : " ";
        responseString += String(byteResponse[j], HEX);
        responseString.toUpperCase();
      }
      Serial.println(responseString);

      moisture = sensorValue((int)byteResponse[3], (int)byteResponse[4]);
      temperature = sensorValue((int)byteResponse[5], (int)byteResponse[6]);
      ec = sensorValue((int)byteResponse[7], (int)byteResponse[8]);
      ph = sensorValue((int)byteResponse[9], (int)byteResponse[10]);
      nitrogen = sensorValue((int)byteResponse[11], (int)byteResponse[12]);
      phosphorus = sensorValue((int)byteResponse[13], (int)byteResponse[14]);
      potassium = sensorValue((int)byteResponse[15], (int)byteResponse[16]);
      
      Serial.println("Moisture: " + String(moisture * 0.1) + " %");
      Serial.println("Temperature: " + String(temperature * 0.1) + " °C");
      Serial.println("pH: " + String(ph * 0.1));
      Serial.println("EC: " + String(ec) + " uS/cm");
      Serial.println("Nitrogen (N): " + String(nitrogen) + " mg/kg");
      Serial.println("Phosporus (P): " + String(phosphorus) + " mg/kg");
      Serial.println("Potassium (K): " + String(potassium) + " mg/kg");

      Serial.println();
    }
    
    char sensor_data_buff[128] = "\0";

    Serial.println("===== SEND DATA TO TTN");

    // AT+SENDB=<confirn_status>,<Fport>,<data_len>,<data>
    snprintf(sensor_data_buff, sizeof(sensor_data_buff),
         "AT+SENDB=%d,%d,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
         0, 2, 14,  // Confirm status, Fport, data length (14 bytes in total)
         (moisture >> 8) & 0xFF, moisture & 0xFF,
         (temperature >> 8) & 0xFF, temperature & 0xFF,
         (ph >> 8) & 0xFF, ph & 0xFF,
         (ec >> 8) & 0xFF, ec & 0xFF,
         (nitrogen >> 8) & 0xFF, nitrogen & 0xFF,
         (phosphorus >> 8) & 0xFF, phosphorus & 0xFF,
         (potassium >> 8) & 0xFF, potassium & 0xFF
        );

    
    LA66.println(sensor_data_buff);
  }

  LA66.listen();

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

int sensorValue(int x, int y) {
  int t = 0;
  t = x * 256;
  t = t + y;
  
  return t;
}
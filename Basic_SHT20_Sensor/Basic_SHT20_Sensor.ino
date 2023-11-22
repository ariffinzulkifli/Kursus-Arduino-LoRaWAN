/*
* LORAWAN AND DATA VISUALISATION
* 
* Penulis: Mohamad Ariffin Zulkifli
* Syarikat: Myinvent Technologies Sdn Bhd
* 
*/

#include <SoftwareSerial.h>

#define sensorFrameSize     9
#define sensorWaitingTime   1000
#define sensorID            0x01
#define sensorFunction      0x04
#define sensorByteResponse  0x04

SoftwareSerial sensor(2, 3);

unsigned char byteRequest[8] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x02, 0x20, 0x0B};
unsigned char byteResponse[9] = {};

float temperature, humidity;

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  sensor.begin(9600);
}


void loop() {

  Serial.println("===== AMBIENT SENSOR PARAMETERS");
  Serial.print("Byte Request: ");

  String requestString;
  for (int j = 0; j < 8; j++) {
    requestString += byteRequest[j] < 0x10 ? " 0" : " ";
    requestString += String(byteRequest[j], HEX);
    requestString.toUpperCase();
  }
  Serial.println(requestString);

  sensor.write(byteRequest, 8);

  unsigned long resptime = millis();
  while ((sensor.available() < sensorFrameSize) && ((millis() - resptime) < sensorWaitingTime)) {
    delay(1);
  }

  while (sensor.available()) {
    for (int n = 0; n < sensorFrameSize; n++) {
      byteResponse[n] = sensor.read();
    }

    if (byteResponse[0] != sensorID && byteResponse[1] != sensorFunction && byteResponse[2] != sensorByteResponse) {
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

    temperature = sensorValue((int)byteResponse[3], (int)byteResponse[4]) * 0.1 ;
    humidity = sensorValue((int)byteResponse[5], (int)byteResponse[6]) * 0.1;
    
    Serial.println("Temperature: " + (String)temperature + " °C");
    Serial.println("Humidity: " + (String)humidity + " %");

    if (millis() - lastMillis > 10000) {
      lastMillis = millis();
    }

    Serial.println();

    delay(1000);
  }
}

int sensorValue(int x, int y) {
  int t = 0;
  t = x * 256;
  t = t + y;
  
  return t;
}

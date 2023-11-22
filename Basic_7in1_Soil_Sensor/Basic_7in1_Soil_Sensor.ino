/*
* LORAWAN AND DATA VISUALISATION
* 
* Penulis: Mohamad Ariffin Zulkifli
* Syarikat: Myinvent Technologies Sdn Bhd
* 
*/

#include <SoftwareSerial.h>

#define sensorFrameSize     19
#define sensorWaitingTime   1000
#define sensorID            0x01
#define sensorFunction      0x03
#define sensorByteResponse  0x0E

SoftwareSerial sensor(2, 3);

unsigned char byteRequest[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
unsigned char byteResponse[19] = {};

float moisture, temperature, ph, ec, nitrogen, phosphorus, potassium;

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  sensor.begin(4800);
}


void loop() {

  Serial.println("===== SOIL SENSOR PARAMETERS");
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

    moisture = sensorValue((int)byteResponse[3], (int)byteResponse[4]) * 0.1 ;
    temperature = sensorValue((int)byteResponse[5], (int)byteResponse[6]) * 0.1;
    ec = sensorValue((int)byteResponse[7], (int)byteResponse[8]);
    ph = sensorValue((int)byteResponse[9], (int)byteResponse[10]) * 0.1;
    nitrogen = sensorValue((int)byteResponse[11], (int)byteResponse[12]);
    phosphorus = sensorValue((int)byteResponse[13], (int)byteResponse[14]);
    potassium = sensorValue((int)byteResponse[15], (int)byteResponse[16]);

    Serial.println("Moisture: " + (String)moisture + " %");
    Serial.println("Temperature: " + (String)temperature + " °C");
    Serial.println("pH: " + (String)ph);
    Serial.println("EC: " + (String)ec + " uS/cm");
    Serial.println("Nitrogen (N): " + (String)nitrogen + " mg/kg");
    Serial.println("Phosporus (P): " + (String)phosphorus + " mg/kg");
    Serial.println("Potassium (K): " + (String)potassium + " mg/kg");

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

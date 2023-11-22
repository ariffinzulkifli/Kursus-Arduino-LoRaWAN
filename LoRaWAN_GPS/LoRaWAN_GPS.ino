/*
* MJII - LORAWAN AND DATA VISUALISATION
* 
* Penulis: Mohamad Ariffin Zulkifli
* Syarikat: Myinvent Technologies Sdn Bhd
* 
*/

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial gpsSerial(2, 3);

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

float latitude, longitude;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  gpsSerial.begin(9600);
  LA66.begin(9600);

  inputString.reserve(200);

  // Reset LA66 MCU
  LA66.println("ATZ");
}

void loop() {
  LA66.listen();

  if ((millis() - previousTTN >= uplinkInterval) && (networkJoinedStatus == 1)) {
    previousTTN = millis();
    getDataStatus = false;

    gpsSerial.listen();

    Serial.println("\n===== GPS PARAMETERS");

    while (!gps.location.isValid()) {
      do {
        if (gpsSerial.available()) {
          gps.encode(gpsSerial.read());
        }
      } while (gps.charsProcessed() < 100);
    }

    Serial.print(F("Location: "));

    latitude = gps.location.lat();
    longitude = gps.location.lng();

    Serial.print(latitude, 6);
    Serial.print(F(","));
    Serial.println(longitude, 6);

    Serial.println();

    char sensor_data_buff[128] = "\0";

    Serial.println("===== SEND DATA TO TTN");

    unsigned char *lat = (unsigned char *)&latitude;
    unsigned char *lon = (unsigned char *)&longitude;

    // AT+SENDB=<confirn_status>,<Fport>,<data_len>,<data>
    snprintf(sensor_data_buff, sizeof(sensor_data_buff),
             "AT+SENDB=%d,%d,%d,%02X%02X%02X%02X%02X%02X%02X%02X",
             0, 2, 8,  // Confirm status, Fport, data length (8 bytes in total)
             lat[0], lat[1], lat[2], lat[3],
             lon[0], lon[1], lon[2], lon[3]
            );

    LA66.println(sensor_data_buff);
  }

  LA66.listen();

  if (receiveCallback == true) {
    receiveCallback = false;
    getDataStatus = true;
    delay(1000);

    LA66.println("AT+CFG");
  }

  while (LA66.available()) {
    char inChar = (char)LA66.read();
    inputString += inChar;

    rxbuff[rxbuff_index++] = inChar;

    if (rxbuff_index > 128)
      break;

    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
      rxbuff[rxbuff_index] = '\0';

      if (strncmp(rxbuff, "JOINED", 6) == 0) {
        networkJoinedStatus = 1;
      }

      if (strncmp(rxbuff, "Dragino LA66 Device", 19) == 0) {
        networkJoinedStatus = 0;
      }

      if (strncmp(rxbuff, "Run AT+RECVB=? to see detail", 28) == 0) {
        receiveCallback = true;
        stringComplete = false;
        inputString = "\0";
      }

      if (strncmp(rxbuff, "AT+RECVB=", 9) == 0) {
        stringComplete = false;
        inputString = "\0";
        Serial.print("\r\nGet downlink data(FPort & Payload) ");
        Serial.println(&rxbuff[9]);
      }

      rxbuff_index = 0;

      if (getDataStatus == true) {
        stringComplete = false;
        inputString = "\0";
      }
    }
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();
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
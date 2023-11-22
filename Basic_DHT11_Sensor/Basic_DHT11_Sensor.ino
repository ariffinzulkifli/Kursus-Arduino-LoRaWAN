#include <SimpleDHT.h>

SimpleDHT11 dht11(2);

byte temperature, humidity;

void setup() {
  Serial.begin(115200);

}

void loop() {
  
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)  {
    // Serial.print("Read DHT11 failed, err=");
    // Serial.print(SimpleDHTErrCode(err));
    // delay(1000);
    return;
  }

  Serial.println("\n===== AMBIENT SENSOR PARAMETERS");
  Serial.println("Temperature: " + String((int)temperature) + " °C");
  Serial.println("Humidity: " + String((int)humidity) + " %");

  delay(1500);
}

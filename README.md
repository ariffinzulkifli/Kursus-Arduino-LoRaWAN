# Kursus Arduino LoRaWAN
```
   __  ___     _                   __    ______        __             __          _       
  /  |/  /_ __(_)__ _  _____ ___  / /_  /_  __/__ ____/ /  ___  ___  / /__  ___ _(_)__ ___
 / /|_/ / // / / _ \ |/ / -_) _ \/ __/   / / / -_) __/ _ \/ _ \/ _ \/ / _ \/ _ `/ / -_|_-<
/_/  /_/\_, /_/_//_/___/\__/_//_/\__/   /_/  \__/\__/_//_/_//_/\___/_/\___/\_, /_/\__/___/
       /___/                                                              /___/           
ai.iot.cloud.technology.training.trading =================================================

Ts. Mohamad Ariffin Zulkifli
ariffin@myduino.com
```
This repository is designed to provide enthusiasts, students, and professionals with hands-on examples and projects to deepen their understanding of IoT LoRaWAN-based applications using Arduino as the LoRaWAN end device prototyping and development platform.

### Hardware Used In This Course
#### LoRaWAN End Device
1. Arduino UNO
2. [Dragino LA66 LoRaWAN Shield](https://myduino.com/product/drg-030/)
3. [Relay Module](https://myduino.com/product/jhs-131/)
4. [DHT11 Sensor](https://myduino.com/product/jhs-061/)
5. [RS485 SHT20 Sensor](https://myduino.com/product/jhs-297/)
6. [RS485 Soil Sensor](https://myduino.com/product/jhs-268/)
7. [GPS Module](https://myduino.com/product/jhs-127/)

<p align="center"><a href="https://myduino.com/product/drg-030/"><img src="https://www.dragino.com/media/k2/galleries/231/LA66-30.jpg" width="400"></a></p>

#### LoRaWAN Gateway
1. [Raspberry Pi 4](https://myduino.com/product/ctm-011/)
2. [Dragino PG1302 LoRaWAN Concentrator HAT](https://myduino.com/product/drg-029/)

<p align="center"><a href="https://myduino.com/product/drg-029/"><img src="https://www.dragino.com/media/k2/galleries/223/PG1302_10.jpg" width="400"></a></p>

### LoRaWAN Network Server
I am using [TheThingsNetwork](https://www.thethingsnetwork.org/) as the Network Server.

The payload from the LoRaWAN End Device need to be formatted using Custom Javascript formatter in TTN, you can find the decoder on `TTN_Decoder.js` file.
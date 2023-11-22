function bytesToFloat(by) {
    var bits = by[3]<<24 | by[2]<<16 | by[1]<<8 | by[0];
    var sign = (bits>>>31 === 0) ? 1.0 : -1.0;
    var e = bits>>>23 & 0xff;
    var m = (e === 0) ? (bits & 0x7fffff)<<1 : (bits & 0x7fffff) | 0x800000;
    var f = sign * m * Math.pow(2, e - 150);
    return f;
}

function Decoder(bytes, port) {
    var decoded = {};

    // if bytes length is less than 4, return empty object
    if (bytes.length < 4) {
        return decoded;
    }
    
    // Decode DHT11
    var temperature = (bytes[0] << 8) | bytes[1];
    decoded.temperature = temperature;
    
    var humidity = (bytes[2] << 8) | bytes[3];
    decoded.humidity = humidity;

    // // Decode RS485 SHT20
    // var temperature = (bytes[0] << 8) | bytes[1];
    // decoded.temperature = temperature / 10;
    
    // var humidity = (bytes[2] << 8) | bytes[3];
    // decoded.humidity = humidity / 10;
    
    // // Decode RS485 Soil Sensor
    // var mois = (bytes[0] << 8) | bytes[1];
    // decoded.moisture = mois / 10;

    // var temp = (bytes[2] << 8) | bytes[3];
    // decoded.temperature = temp / 10;

    // var ph = (bytes[4] << 8) | bytes[5];
    // decoded.ph = ph / 10;

    // var ec = (bytes[6] << 8) | bytes[7];
    // decoded.ec = ec;

    // var nitrogen = (bytes[8] << 8) | bytes[9];
    // decoded.nitrogen = nitrogen;

    // var phosphorus = (bytes[10] << 8) | bytes[11];
    // decoded.phosphorus = phosphorus;

    // var potassium = (bytes[12] << 8) | bytes[13];
    // decoded.potassium = potassium;
    
    // // Decode GPS
    // decoded.latitude = bytesToFloat(bytes.slice(0, 4));
    // decoded.longitude = bytesToFloat(bytes.slice(4, 8));

    return decoded;
}
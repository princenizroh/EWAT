#include <DHT.h>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xE2, 0xF1, 0xED};

#define DHTPIN 9
#define DHTTYPE DHT22
#define RELAY_PIN 8 // RELAY_PIN bisa sebagai relay atau keluaran solenoid valve


DHT dht(DHTPIN, DHTTYPE);

IPAddress ip(192,168,2,45);
IPAddress server(192,168,2,45);

EthernetClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  Ethernet.begin(mac,ip);
  Serial.print("IP server");
  Serial.println(Ethernet.localIP()); 
}

void loop() {
  float humi = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);

  if (isnan(tempC) || isnan(tempF) || isnan(humi)){
    Serial.println("-----Gagal membaca dari DHT Sensor-----");
  
  } else {

    Serial.print("Humidity : ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print(" | ");

    Serial.print("Temperature: ");
    Serial.print(tempC);
    String postData = "temperature C = " + String(tempC) + "%humidity = " + String(humi) + "&temperature F" + String(tempF);
    if (client.connect(server, 80)){
      client.println("POST /api/data HTTP/1.1");
      client.println("Host: 192.168.1.200");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(postData.length());
      client.println();
      client.print(postData);
      client.println();
      client.stop();
    } else {
      Serial.println("Failed to connect to server");
    }
    if (tempC >= 32){
      digitalWrite(RELAY_PIN, HIGH);
    }
    else{
      digitalWrite(RELAY_PIN, LOW);
    }
    Serial.print("°C  ~  ");
    Serial.print(tempF);
    Serial.println("°F");

  }
  delay(100);
}

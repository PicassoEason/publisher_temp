#include <DHT.h>         // 引入DHT庫
#include <WiFi.h>        // 引入WiFi庫
#include <PubSubClient.h> // 引入MQTT庫
#include <Adafruit_Sensor.h>
#define WIFI_SSID "TrunkStudio-2.4G"        // WiFi名稱
#define WIFI_PASSWORD "22019020" // WiFi密碼

#define MQTT_SERVER "192.168.168.184" // MQTT伺服器的IP位址
#define MQTT_PORT 1883             // MQTT伺服器的埠口
#define MQTT_TOPIC "msg/test"          // MQTT主題名稱
#define MQTT_USERNAME "user"  // 請替換為您的MQTT使用者名稱
#define MQTT_PASSWORD "1234"  // 請替換為您的MQTT密碼
#define DHT_PIN 32      // DHT11感測器的輸入腳
#define DHT_TYPE DHT11 // DHT11感測器型號

// 建立DHT感測器物件
DHT dht(DHT_PIN, DHT_TYPE);

// 建立WiFi客戶端物件
WiFiClient wifiClient;

// 建立MQTT客戶端物件
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600); // 設定Serial通訊速率
  delay(1000);        // 等待Serial通訊準備完成

  // 連接WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // 連接MQTT伺服器
  Serial.println("Connecting to MQTT server...");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("arduino-client",MQTT_USERNAME,MQTT_PASSWORD)) {
      Serial.println("MQTT connected");
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  // 初始化DHT感測器
  Serial.println("Initializing DHT sensor...");
  dht.begin();
}

void loop() {
  // 讀取DHT11感測器的溫濕度值
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    char payload[50];
    sprintf(payload, "{\"temperature\":%.2f,\"humidity\":%.2f}", temperature, humidity);
    mqttClient.publish(MQTT_TOPIC, payload);
  } else {
    Serial.println("Failed to read DHT11 sensor data");
  }

  // 等待5秒後再次讀取
  delay(3000);
}

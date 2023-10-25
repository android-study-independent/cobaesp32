#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define DHTPIN 22
#define DHTTYPE DHT11

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <BlynkSimpleEsp32.h>
#include <FirebaseESP32.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";
/* 2. Define the API Key */
#define API_KEY ""
/* 3. Define the RTDB URL */
#define DATABASE_URL "" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL ""
#define USER_PASSWORD ""

#define LED 2

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

BlynkTimer timer;

DHT dht(DHTPIN, DHTTYPE);

void setupFirebase()
{
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  Serial.println("------------------------------------");
  Serial.println("Sign in user...");

  Firebase.begin(&config, &auth);
}

void initWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

FirebaseJson temperature_json;
FirebaseJson humidity_json;

void readDHTSensor()
{
  delay(1000);

  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.print(F("Temperature: "));
  Serial.print(temp);
  Serial.println("");
  Serial.print("Humidity: ");
  Serial.print(humidity);

  temperature_json.set("value", temp);
  temperature_json.set("device", "device_test");

  humidity_json.set("value", humidity);
  humidity_json.set("device", "device_test");

  if (Firebase.pushJSON(fbdo, "coba_temp/device_test", temperature_json))
  {
    Serial.println("Berhasil push data firebase :");
    Serial.print("path -> ");
    Serial.println(fbdo.dataPath());
  }
  else
  {
    Serial.println("Gagal push data");
    Serial.print("error msg: ");
    Serial.print(fbdo.errorReason());
  }

  if (Firebase.pushJSON(fbdo, "coba_hum/device_test", humidity_json))
  {
    Serial.println("Berhasil push data firebase :");
    Serial.print("path -> ");
    Serial.println(fbdo.dataPath());
  }
  else
  {
    Serial.println("Gagal push data");
    Serial.print("error msg: ");
    Serial.print(fbdo.errorReason());
  }

  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, humidity);

  Serial.println("");
}

void setup()
{
  // function yang pertama kali akkan dipanggil
  Serial.begin(9600);

  initWifi();
  setupFirebase();

  Serial.println(F("Test Sensor DHT"));

  pinMode(LED, OUTPUT);

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // Setup a function to be called every second
  timer.setInterval(2000L, readDHTSensor);
}

void blinkOnboardLED()
{
  // put your main code here, to run repeatedly:
  delay(500);
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
}

void loop()
{
  Blynk.run();
  timer.run();
}

// Tugas Week #11
// - bikin artikel
// - design system,
//   --bikin design system apps yang relate dengan agriculture + iot + android apps-- sample smarternak
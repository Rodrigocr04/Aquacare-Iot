#include "DHT.h"  // Include the DHT library for temperature and humidity sensor
#define DHTPIN D1  // Define the pin connected to the DHT sensor
#define DHTTYPE DHT11  // Define the DHT sensor type as DHT11

#include <OneWire.h>  // Include the OneWire library for the DS18B20 sensor
#include <DallasTemperature.h>  // Include the DallasTemperature library for DS18B20

#include <Arduino.h>  // Include Arduino core library
#include <ESP8266WiFi.h>  // Include WiFi library for ESP8266
#include <Firebase_ESP_Client.h>  // Include Firebase ESP client library
#include <Servo.h>  // Include Servo library to control a servo motor

const int oneWireBus = D6;  // Define the pin for DS18B20 sensor data
int LDR = A0;  // Define the pin for the light-dependent resistor (LDR)
int ledPinD4 = D4;  // Define the pin for LED 1
int ledPinD3 = D3;  // Define the pin for LED 2
int ledPinD2 = D2;  // Define the pin for LED 3
int buzzerPin = D0;  // Define the pin for the buzzer
int servoPin = D5;  // Define the pin for the servo motor

const int triggerPin = D7;  // Define the pin for ultrasonic sensor trigger
const int echoPin = D8;  // Define the pin for ultrasonic sensor echo

Servo myServo;  // Create an instance of the Servo object

OneWire oneWire(oneWireBus);  // Create OneWire object to communicate with DS18B20
DallasTemperature sensors(&oneWire);  // Pass the OneWire reference to DallasTemperature

DHT dht(DHTPIN, DHTTYPE);  // Create DHT sensor object

// Wi-Fi credentials
#define WIFI_SSID "INFINITUM07B1"  // Your Wi-Fi SSID
#define WIFI_PASSWORD "BVATGVCY64"  // Your Wi-Fi password

// Firebase API credentials
#define API_KEY "AIzaSyAA1OTjaQzhKp78IZ384ujijLevsQeGWbQ"  // Firebase API Key
#define DATABASE_URL "https://aquacare-638ce-default-rtdb.firebaseio.com/"  // Firebase Realtime Database URL

FirebaseData fbdo;  // Firebase data object

FirebaseAuth auth;  // Firebase authentication object
FirebaseConfig config;  // Firebase configuration object

bool signupOK = false;  // Boolean flag to track Firebase sign-up status

// Token generation callback function
void myTokenStatusCallback(token_info_t tokenInfo) {
  if (tokenInfo.status) {
    Serial.println("Token generation successful");
  } else {
    Serial.println("Token generation failed");
  }
}

void setup() {
  // Set pin modes for sensors, LEDs, and servo
  pinMode(DHTPIN, INPUT);
  pinMode(ledPinD3, OUTPUT);
  pinMode(ledPinD2, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(servoPin);  // Attach the servo to the specified pin
  
  dht.begin();  // Initialize DHT sensor
  sensors.begin();  // Initialize DS18B20 sensor
  Serial.begin(115200);  // Begin serial communication at 115200 baud rate
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");

  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set Firebase configurations
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = myTokenStatusCallback;  // Assign custom callback for token status

  // Try to sign up to Firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Sign-up successful");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);  // Initialize Firebase
  Firebase.reconnectWiFi(true);  // Enable Wi-Fi reconnection
}

void loop() {
  delay(500);  // Delay to avoid overwhelming the loop
  float h = dht.readHumidity();  // Read humidity from DHT11 sensor
  float t = dht.readTemperature();  // Read temperature from DHT11 sensor

  // Send humidity and temperature data to Firebase if the connection is ready
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/humidity", h)) {
      Serial.print("Humidity: ");
      Serial.println(h);
    } else {
      Serial.println("Error sending humidity to Firebase");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", t)) {
      Serial.print("Temperature: ");
      Serial.println(t);
    } else {
      Serial.println("Error sending temperature to Firebase");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }

  Serial.println("______________________________");

  // Read light level from LDR (analog sensor)
  int luz = analogRead(A0);
  int luzPorcentaje = map(luz, 0, 1023, 0, 100);  // Convert LDR reading to percentage

  // Control LEDs based on light intensity
  if (luz > 540) {
    digitalWrite(ledPinD2, HIGH);  
  } else {
    digitalWrite(ledPinD2, LOW);  
  }

  if (luz > 520) {
    digitalWrite(ledPinD3, HIGH);  
  } else {
    digitalWrite(ledPinD3, LOW);  
  }

  if (luz > 500) {
    digitalWrite(ledPinD4, HIGH);  
  } else {
    digitalWrite(ledPinD4, LOW);  
  }

  // Send light data to Firebase
  if (Firebase.RTDB.setInt(&fbdo, "luz", luzPorcentaje)) {
    Serial.println("Light data successfully sent: " + String(luzPorcentaje) + "%");
  } else {
    Serial.println("Error sending light data to Firebase");
    Serial.println("Reason: " + fbdo.errorReason());
  }

  // Read sensor and send value to Firebase
  if (digitalRead(4) == 0) {
    if (Firebase.RTDB.setInt(&fbdo, "Sensor", 0)) {
      Serial.println("Sensor: 0");
    } else {
      Serial.println("Error sending sensor data to Firebase");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  } else {
    if (Firebase.RTDB.setInt(&fbdo, "Sensor", 1)) {
      Serial.println("Sensor: 1");
    } else {
      Serial.println("Error sending sensor data to Firebase");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }
  delay(500);

  // Read temperature from DS18B20 sensor
  sensors.requestTemperatures();  // Request temperature readings from DS18B20
  float temperatureC = sensors.getTempCByIndex(0);  // Get temperature in Celsius
  float temperatureF = sensors.getTempFByIndex(0);  // Get temperature in Fahrenheit

  // Print temperature readings to Serial Monitor
  Serial.print("Temperature (C): ");
  Serial.println(temperatureC);
  Serial.print("Temperature (F): ");
  Serial.println(temperatureF);

  // Send DS18B20 temperature data to Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "DS18B20/temperatureC", temperatureC)) {
    Serial.println("Temperature data sent to Firebase (C)");
  } else {
    Serial.println("Error sending temperature data to Firebase (C)");
    Serial.println("Reason: " + fbdo.errorReason());
  }

  if (Firebase.RTDB.setFloat(&fbdo, "DS18B20/temperatureF", temperatureF)) {
    Serial.println("Temperature data sent to Firebase (F)");
  } else {
    Serial.println("Error sending temperature data to Firebase (F)");
    Serial.println("Reason: " + fbdo.errorReason());
  }

  // Servo motor control - oscillating between 0 and 180 degrees
  myServo.write(0);  // Rotate servo motor to 0 degrees
  delay(1000);  // Wait for 1 second
  myServo.write(180);  // Rotate servo motor to 180 degrees
  delay(1000);  // Wait for 1 second

  // Ultrasonic sensor code to measure distance and control buzzer
  long duration;  // Variable to store the pulse duration
  float distance;  // Variable to store calculated distance

  // Send trigger pulse
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Read echo pulse duration
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert pulse duration to distance in cm

  Serial.print("Distance: ");
  Serial.println(distance);  // Print the distance to the Serial Monitor

  // Control buzzer based on proximity
  if (distance <= 10) {
    digitalWrite(buzzerPin, HIGH);  // Turn on buzzer if distance is less than or equal to 10 cm
    delay(500);  // Wait for 500 ms
    digitalWrite(buzzerPin, LOW);  // Turn off buzzer
  }

  // Send distance data to Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "Ultrasonic/distance", distance)) {
    Serial.println("Distance data sent to Firebase");
  } else {
    Serial.println("Error sending distance data to Firebase");
    Serial.println("Reason: " + fbdo.errorReason());
  }
}

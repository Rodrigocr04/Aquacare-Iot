#include "DHT.h"
#define DHTPIN D1
#define DHTTYPE DHT11

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Servo.h>

const int oneWireBus = D6; 
int LDR = A0;
int ledPinD4 = D4;
int ledPinD3 = D3;  
int ledPinD2 = D2;
int buzzerPin = D0;
int servoPin = D5;  

const int triggerPin = D7;
const int echoPin = D8;

Servo myServo;

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

DHT dht(DHTPIN, DHTTYPE);

// Inserta tus credenciales de red
#define WIFI_SSID "INFINITUM07B1"
#define WIFI_PASSWORD "BVATGVCY64"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAA1OTjaQzhKp78IZ384ujijLevsQeGWbQ"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://aquacare-638ce-default-rtdb.firebaseio.com/"

// Define el objeto FirebaseData
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

void myTokenStatusCallback(token_info_t tokenInfo) {
  if (tokenInfo.status) {
    Serial.println("Generación de token exitosa");
  } else {
    Serial.println("Generación de token fallida");
  }
}

void setup() {
  pinMode(DHTPIN, INPUT);
  pinMode(ledPinD3, OUTPUT);
  pinMode(ledPinD2, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(servoPin);
  
  dht.begin();
  sensors.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  config.token_status_callback = myTokenStatusCallback; // Asigna la función de callback personalizada

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Registro exitoso");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  delay(500);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/humidity", h)) {
      Serial.print("Humedad: ");
      Serial.println(h);
    } else {
      Serial.println("Error al enviar la humedad a Firebase");
      Serial.println("Motivo: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", t)) {
      Serial.print("Temperatura: ");
      Serial.println(t);
    } else {
      Serial.println("Error al enviar la temperatura a Firebase");
      Serial.println("Motivo: " + fbdo.errorReason());
    }
  }

  Serial.println("______________________________");
  
  int luz = analogRead(A0);

  int luzPorcentaje = map(luz, 0, 1023, 0, 100);

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

if (Firebase.RTDB.setInt(&fbdo, "luz", luzPorcentaje)) {
  Serial.println("Datos de luz enviados exitosamente: " + String(luzPorcentaje) + "%");
} else {
  Serial.println("Error al enviar datos de luz a Firebase");
  Serial.println("Motivo: " + fbdo.errorReason());
}

  if (digitalRead(4) == 0) {
    if (Firebase.RTDB.setInt(&fbdo, "Sensor", 0)) {
      Serial.println("Sensor: 0");
    } else {
      Serial.println("Error al enviar datos del sensor a Firebase");
      Serial.println("Motivo: " + fbdo.errorReason());
    }
  } else {
    if (Firebase.RTDB.setInt(&fbdo, "Sensor", 1)) {
      Serial.println("Sensor: 1");
    } else {
      Serial.println("Error al enviar datos del sensor a Firebase");
      Serial.println("Motivo: " + fbdo.errorReason());
    }
  }
  delay(500);

  // Read temperature from DS18B20
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);

  // Print temperature to Serial Monitor
  Serial.print("Temperature (C): ");
  Serial.println(temperatureC);
  Serial.print("Temperature (F): ");
  Serial.println(temperatureF);

  // Send temperature data to Firebase
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

myServo.write(0);  // Gira el servomotor a 180 grados (dirección opuesta)
delay(1000);         // Espera 1 segundo

myServo.write(180);   // Gira el servomotor a 90 grados (dirección opuesta)
delay(1000);         // Espera 1 segundo

  // Código para el ultrasonido y el buzzer
  long duration;
  float distance;

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;  // Calibrar el factor de conversión según tus necesidades
  
  if (distance > 10) {
    // Si la distancia es mayor a 10 cm, activar el buzzer
    digitalWrite(buzzerPin, HIGH);
  } else {
    // Si la distancia es menor o igual a 10 cm, apagar el buzzer
    digitalWrite(buzzerPin, LOW);
  }

  // Enviar la distancia a Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "ultrasonic/distance", distance)) {
    Serial.print("Distancia ultrasónica: ");
    Serial.println(distance);
  } else {
    Serial.println("Error al enviar la distancia ultrasónica a Firebase");
    Serial.println("Motivo: " + fbdo.errorReason());
  }

  delay(5000); // Adjust the delay as needed
}
/*
 * ESP32 MQTT Control Completo para Dashboard IoT
 * Integrado con el proyecto IOT-MQTT-Dashboard-for-ESP32-Plant-Sensor
 * 
 * Características:
 * - Control de LEDs vía MQTT
 * - Sincronización bidireccional de estado
 * - Sensor de temperatura simulado
 * - Reconexión automática
 * - Integración con dashboard web existente
 * - Configuración mediante archivo externo opcional
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>  // Para sensor DHT11/DHT22 (opcional)

// Intentar cargar configuración externa si existe
#ifdef __has_include
  #if __has_include("config_secrets.h")
    #include "config_secrets.h"
  #endif
#endif

// ============================================
// CONFIGURACIÓN WIFI - VALORES POR DEFECTO
// ============================================
#ifndef WIFI_SSID
  #define WIFI_SSID "TU_SSID"
  #define WIFI_PASSWORD "TU_PASSWORD"
#endif

// ============================================
// CONFIGURACIÓN MQTT - COINCIDE CON TU DASHBOARD
// ============================================
#ifndef MQTT_BROKER
  #define MQTT_BROKER "broker.hivemq.com"
  #define MQTT_PORT 1883
#endif

// Tópicos MQTT para cada LED
const char* MQTT_TOPIC_LED1 = "LED1/control";
const char* MQTT_TOPIC_LED2 = "LED2/control";
const char* MQTT_TOPIC_LED3 = "LED3/control";
const char* MQTT_TOPIC_TEMPERATURA = "sensor/temperatura";

// Tópicos de estado (opcional)
const char* MQTT_TOPIC_LED1_STATUS = "LED1/status";
const char* MQTT_TOPIC_LED2_STATUS = "LED2/status";
const char* MQTT_TOPIC_LED3_STATUS = "LED3/status";

#ifndef DEVICE_ID
  #define MQTT_CLIENT_ID "ESP32-PlantSensor-001"
#else
  const char* MQTT_CLIENT_ID = DEVICE_ID;
#endif

// ============================================
// CONFIGURACIÓN DE PINES - 3 LEDs
// ============================================
#ifndef LED1_PIN
  #define LED1_PIN 3          // LED 1 (D3)
#endif

#ifndef LED2_PIN
  #define LED2_PIN 5          // LED 2 (D5)
#endif

#ifndef LED3_PIN
  #define LED3_PIN 7          // LED 3 (D7)
#endif

#ifndef DHT_PIN
  #define DHT_PIN 23         // Pin para sensor DHT (opcional)
#endif

// ============================================
// CONFIGURACIÓN DEL SENSOR DHT (opcional)
// ============================================
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// ============================================
// VARIABLES GLOBALES
// ============================================
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastTempPublish = 0;
const unsigned long TEMP_PUBLISH_INTERVAL = 5000; // 5 segundos
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000; // 5 segundos

// Estados de los 3 LEDs
bool led1State = false;
bool led2State = false;
bool led3State = false;

#ifndef USE_DHT_SENSOR
  #define USE_DHT_SENSOR false  // Cambiar a true si tienes sensor DHT conectado
#endif
bool useDHTSensor = USE_DHT_SENSOR;

// ============================================
// SETUP INICIAL
// ============================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n========================================");
  Serial.println("ESP32 MQTT Control - Iniciando...");
  Serial.println("========================================");
  
  // Configurar pines de los 3 LEDs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  
  // Inicializar sensor DHT si está disponible
  dht.begin();
  delay(1000);
  float testTemp = dht.readTemperature();
  if (!isnan(testTemp)) {
    useDHTSensor = true;
    Serial.println("Sensor DHT detectado y activado");
  } else {
    Serial.println("Sensor DHT no detectado, usando simulación");
  }
  
  // Conectar WiFi
  setupWiFi();
  
  // Configurar MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  Serial.println("========================================");
  Serial.println("Sistema listo. Esperando mensajes MQTT...");
  Serial.println("========================================");
}

// ============================================
// LOOP PRINCIPAL
// ============================================
void loop() {
  // Mantener conexión MQTT activa
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      if (reconnectMQTT()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }
  
  // Publicar temperatura periódicamente
  unsigned long now = millis();
  if (now - lastTempPublish > TEMP_PUBLISH_INTERVAL && mqttClient.connected()) {
    publishTemperature();
    lastTempPublish = now;
  }
  
  delay(100); // Pequeña pausa para no saturar el CPU
}

// ============================================
// CONFIGURACIÓN WIFI
// ============================================
void setupWiFi() {
  Serial.print("Conectando a WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado exitosamente");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    Serial.print("Señal RSSI: ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println("\n❌ Error: No se pudo conectar a WiFi");
    Serial.println("Reiniciando en 5 segundos...");
    delay(5000);
    ESP.restart();
  }
}

// ============================================
// RECONEXIÓN MQTT
// ============================================
bool reconnectMQTT() {
  Serial.print("Intentando conectar al broker MQTT: ");
  Serial.println(MQTT_BROKER);
  
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("✅ Conectado al broker MQTT");

    // Suscribirse a los tópicos de control de los 3 LEDs
    mqttClient.subscribe(MQTT_TOPIC_LED1);
    Serial.print("Suscrito al tópico: ");
    Serial.println(MQTT_TOPIC_LED1);

    mqttClient.subscribe(MQTT_TOPIC_LED2);
    Serial.print("Suscrito al tópico: ");
    Serial.println(MQTT_TOPIC_LED2);

    mqttClient.subscribe(MQTT_TOPIC_LED3);
    Serial.print("Suscrito al tópico: ");
    Serial.println(MQTT_TOPIC_LED3);

    // Publicar estados iniciales
    publishLedStates();

    return true;
  } else {
    Serial.print("❌ Falló conexión MQTT, rc=");
    Serial.println(mqttClient.state());
    Serial.println("Intentando nuevamente en 5 segundos...");
    return false;
  }
}

// ============================================
// CALLBACK MQTT - PROCESA MENSAJES RECIBIDOS
// ============================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  // Procesar comandos de control para cada LED
  if (String(topic) == MQTT_TOPIC_LED1) {
    processControlCommand(message, 1);
  } else if (String(topic) == MQTT_TOPIC_LED2) {
    processControlCommand(message, 2);
  } else if (String(topic) == MQTT_TOPIC_LED3) {
    processControlCommand(message, 3);
  }
}

// ============================================
// PROCESAR COMANDOS DE CONTROL
// ============================================
void processControlCommand(String command, int ledNum) {
  Serial.print("Procesando comando para LED");
  Serial.print(ledNum);
  Serial.print(": ");
  Serial.println(command);

  bool newState = false;

  if (command == "ON" || command == "on") {
    newState = true;
  } else if (command == "OFF" || command == "off") {
    newState = false;
  } else if (command == "TOGGLE" || command == "toggle") {
    // Alternar estado según el LED
    switch(ledNum) {
      case 1: newState = !led1State; break;
      case 2: newState = !led2State; break;
      case 3: newState = !led3State; break;
    }
  } else {
    Serial.println("Comando no reconocido");
    return;
  }

  setLedState(newState, ledNum);
}

// ============================================
// ESTABLECER ESTADO DEL LED INDIVIDUAL
// ============================================
void setLedState(bool state, int ledNum) {
  int pin;
  const char* statusTopic;

  switch(ledNum) {
    case 1:
      led1State = state;
      pin = LED1_PIN;
      statusTopic = MQTT_TOPIC_LED1_STATUS;
      break;
    case 2:
      led2State = state;
      pin = LED2_PIN;
      statusTopic = MQTT_TOPIC_LED2_STATUS;
      break;
    case 3:
      led3State = state;
      pin = LED3_PIN;
      statusTopic = MQTT_TOPIC_LED3_STATUS;
      break;
    default:
      return;
  }

  if (state) {
    digitalWrite(pin, HIGH);
    Serial.print("LED");
    Serial.print(ledNum);
    Serial.println(" encendido");
  } else {
    digitalWrite(pin, LOW);
    Serial.print("LED");
    Serial.print(ledNum);
    Serial.println(" apagado");
  }

  // Publicar el estado del LED específico
  String stateMessage = state ? "ON" : "OFF";
  mqttClient.publish(statusTopic, stateMessage.c_str());
  Serial.print("Estado LED");
  Serial.print(ledNum);
  Serial.print(" publicado: ");
  Serial.println(stateMessage);
}

// ============================================
// PUBLICAR ESTADOS DE TODOS LOS LEDs
// ============================================
void publishLedStates() {
  String state1 = led1State ? "ON" : "OFF";
  String state2 = led2State ? "ON" : "OFF";
  String state3 = led3State ? "ON" : "OFF";

  mqttClient.publish(MQTT_TOPIC_LED1_STATUS, state1.c_str());
  mqttClient.publish(MQTT_TOPIC_LED2_STATUS, state2.c_str());
  mqttClient.publish(MQTT_TOPIC_LED3_STATUS, state3.c_str());

  Serial.println("Estados iniciales publicados:");
  Serial.print("LED1: "); Serial.println(state1);
  Serial.print("LED2: "); Serial.println(state2);
  Serial.print("LED3: "); Serial.println(state3);
}

// ============================================
// PUBLICAR TEMPERATURA
// ============================================
void publishTemperature() {
  float temperatura;
  
  if (useDHTSensor) {
    temperatura = dht.readTemperature();
    if (isnan(temperatura)) {
      Serial.println("Error leyendo sensor DHT, usando simulación");
      temperatura = simulateTemperature();
    }
  } else {
    temperatura = simulateTemperature();
  }
  
  String tempMessage = String(temperatura, 1);
  
  if (mqttClient.publish(MQTT_TOPIC_TEMPERATURA, tempMessage.c_str())) {
    Serial.print("Temperatura publicada: ");
    Serial.print(temperatura, 1);
    Serial.println("°C");
  } else {
    Serial.println("Error al publicar temperatura");
  }
}

// ============================================
// SIMULAR TEMPERATURA (para demo sin sensor)
// ============================================
float simulateTemperature() {
  // Simular temperatura entre 20-30°C con variaciones
  static float baseTemp = 25.0;
  static float lastTemp = baseTemp;
  
  // Variación aleatoria pequeña
  float variation = (random(-10, 11) / 10.0); // -1.0 a +1.0
  float newTemp = lastTemp + variation;
  
  // Mantener dentro de rango realista
  if (newTemp < 20.0) newTemp = 20.0;
  if (newTemp > 30.0) newTemp = 30.0;
  
  lastTemp = newTemp;
  return newTemp;
}

// ============================================
// MANEJO DE CONEXIÓN WIFI PERDIDA
// ============================================
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi desconectado, intentando reconectar...");
      WiFi.reconnect();
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("WiFi reconectado");
      break;
    default:
      break;
  }
}

// Registrar evento de WiFi
WiFiEventId_t eventID = WiFi.onEvent(WiFiEvent);
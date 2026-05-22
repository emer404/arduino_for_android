# 🚀 Sistema Completo de Control ESP32-MQTT-Dashboard

## 📋 Descripción General

Este es un sistema completo de IoT para controlar microcontroladores ESP32/Arduino mediante MQTT, integrado con un dashboard web en tiempo real. El sistema permite:

- ✅ **Control de LEDs** desde el dashboard web
- ✅ **Monitorización de sensores** (temperatura) en tiempo real  
- ✅ **Sincronización bidireccional** de estado
- ✅ **Conexión robusta** con reconexión automática
- ✅ **Simulación completa** para pruebas sin hardware

## 🏗️ Arquitectura del Sistema

```
┌─────────────────┐    MQTT      ┌─────────────────┐
│  Dashboard Web  │ ◄──────────► │  Broker MQTT    │
│  (WebSocket)    │   :8000      │  (broker.hivemq │
└─────────────────┘              │      .com)      │
                                 └────────┬────────┘
                                          │
                                          │ MQTT :1883
                                          │
                    ┌─────────────────────┴─────────────────────┐
                    │                                           │
           ┌────────▼────────┐                        ┌────────▼────────┐
           │  ESP32/Arduino  │                        │  Simulador     │
           │  (Hardware)     │                        │  Python        │
           │  Puerto 1883    │                        │  (Pruebas)     │
           └─────────────────┘                        └─────────────────┘
```

## 🔌 Tópicos MQTT

| Tópico | Dirección | Descripción | Formato |
|--------|-----------|-------------|---------|
| `LED/control` | Bidireccional | Control de LED y estado | "ON", "OFF" |
| `sensor/temperatura` | ESP32→Web | Datos de temperatura | "25.5" (número) |
| `LED/status` | ESP32→Web | Estado del dispositivo (opcional) | "ON", "OFF" |

## 📦 Componentes del Sistema

### 1. **Dashboard Web** (`dashboard.html`)
- Interfaz web moderna con AdminLTE
- Control de switches para encender/apagar LEDs
- Gráfica en tiempo real de sensores
- Conexión MQTT vía WebSockets (puerto 8000)

### 2. **ESP32 Firmware** (`esp32_mqtt_complete.ino`)
- Control de LEDs mediante MQTT
- Sensor DHT11/DHT22 integrado (opcional)
- Reconexión automática WiFi y MQTT
- Publicación periódica de temperatura
- Sincronización de estado bidireccional

### 3. **Simulador Python** (`simulador_mqtt.py`)
- Simulación completa del comportamiento ESP32
- Pruebas sin hardware físico
- Generación de datos de temperatura
- Respuesta a comandos de control

## 🛠️ Configuración e Instalación

### Requisitos Previos

#### Para el Dashboard Web:
- Navegador web moderno
- Servidor web (opcional, para producción)
- Broker MQTT público (broker.hivemq.com configurado por defecto)

#### Para el ESP32:
- Placa ESP32 o Arduino con WiFi
- Arduino IDE con soporte ESP32
- Librerías necesarias:
  - `WiFi.h`
  - `PubSubClient.h`
  - `DHT.h` (opcional, para sensores)

#### Para el Simulador Python:
- Python 3.x
- Librería `paho-mqtt`

```bash
pip install paho-mqtt
```

### Configuración del ESP32

1. **Instalar librerías en Arduino IDE:**
   - Tools → Manage Libraries
   - Buscar e instalar: "PubSubClient", "DHT sensor library"

2. **Modificar configuración WiFi:**
   ```cpp
   const char* WIFI_SSID = "TU_SSID";
   const char* WIFI_PASSWORD = "TU_PASSWORD";
   ```

3. **Subir el firmware:**
   - Conectar ESP32 al computador
   - Seleccionar placa y puerto correctos
   - Subir el código `esp32_mqtt_complete.ino`

### Configuración del Dashboard

1. **Abrir el dashboard:**
   - Abrir `dashboard.html` en navegador web
   - O usar un servidor web para producción

2. **Configurar conexión MQTT:**
   - Host: `broker.hivemq.com`
   - Puerto WebSockets: `8000`
   - SSL: `No`
   - Tópico: `LED/control`

3. **Conectar:**
   - Presionar botón "Conectar"
   - Esperar indicador "🟢 Conectado"

### Ejecutar el Simulador Python

```bash
python simulador_mqtt.py
```

El simulador mostrará:
- Conexión al broker MQTT
- Recepción de comandos del dashboard
- Publicación de datos de temperatura
- Cambios de estado simulados

## 🎮 Uso del Sistema

### Flujo de Trabajo Completo

#### **Escenario 1: Control desde Dashboard**
1. Usuario abre dashboard web
2. Conecta al broker MQTT
3. Cambia switch a "ON"
4. Dashboard publica "ON" en `LED/control`
5. ESP32 recibe mensaje y enciende LED
6. ESP32 publica estado de vuelta
7. Dashboard actualiza estado

#### **Escenario 2: Cambios Externos en ESP32**
1. ESP32 detecta cambio (por sensor externo)
2. ESP32 publica nuevo estado en `LED/control`
3. Dashboard recibe mensaje MQTT
4. Dashboard actualiza UI automáticamente
5. Se registra actividad reciente

#### **Escenario 3: Monitorización de Sensores**
1. ESP32 lee temperatura cada 5 segundos
2. Publica en `sensor/temperatura`
3. Dashboard recibe y actualiza gráfica
4. Datos históricos visualizados en tiempo real

## 🔧 Funcionalidades Detalladas

### Control de LEDs

**Comandos MQTT aceptados:**
- `ON` - Encender LED
- `OFF` - Apagar LED
- `TOGGLE` - Alternar estado

**Pines configurables:**
- `LED_PIN = 2` - LED integrado ESP32
- `LED_EXTRA_PIN = 4` - LED externo opcional

### Sensores

**Sensor DHT11/DHT22 (opcional):**
- Conectar a pin 23
- Cambiar `useDHTSensor = true` en código
- Lectura automática de temperatura y humedad

**Simulación de temperatura:**
- Activo por defecto sin sensor físico
- Rango: 20°C - 30°C
- Actualización cada 5 segundos

### Reconexión Automática

**WiFi:**
- Reconexión automática si se pierde señal
- Reinicio del sistema si falla conexión

**MQTT:**
- Reconexión cada 5 segundos si falla
- Re-suscripción automática a tópicos
- Publicación de estado tras reconectar

### Sincronización de Estado

**Mecanismo:**
- Dashboard y ESP32 mantienen estado sincronizado
- Cambios en cualquiera se reflejan en el otro
- Publicación de estado después de cada cambio

**Evitar bucles:**
- Dashboard no republica mensajes recibidos de ESP32
- Solo publica cambios iniciados por usuario web

## 📊 Monitoreo y Debugging

### Consola Serie ESP32

Conectar ESP32 al computador y abrir monitor serie (115200 baud):

```
========================================
ESP32 MQTT Control - Iniciando...
========================================
Conectando a WiFi: TU_SSID
✅ WiFi conectado exitosamente
IP地址: 192.168.1.100
Señal RSSI: -45
✅ Conectado al broker MQTT
Suscrito al tópico: LED/control
Estado publicado: ON
```

### Logs del Dashboard

Abrir consola del navegador (F12) para ver:

```
MQTT connected successfully
Subscribed to control topic: LED/control
Subscribed to temperature topic: sensor/temperatura
Message published: ON
Received message: LED/control ON
Temperature updated: 25.5 °C
```

### Logs del Simulador Python

```bash
========================================
🚀 Simulador MQTT ESP32 Iniciando...
========================================
📡 Broker: broker.hivemq.com:1883
✅ Conectado exitosamente al broker MQTT
📡 Suscrito al tópico de control: LED/control
📤 Estado publicado: OFF
📨 Mensaje recibido en 'LED/control': ON
💡 Encendiendo dispositivo...
🌡️ Temperatura publicada: 25.3°C
```

## 🚨 Solución de Problemas

### Problemas Comunes

**Dashboard no conecta:**
- Verificar que el broker MQTT esté accesible
- Usar puerto 8000 para WebSockets, 1883 para TCP
- Revisar configuración SSL/TLS

**ESP32 no conecta WiFi:**
- Verificar credenciales WiFi correctas
- Revisar señal WiFi (RSSI > -70 dBm)
- Reiniciar ESP32

**ESP32 no conecta MQTT:**
- Verificar conexión WiFi primero
- Confirmar broker accesible desde red local
- Revisar firewall si hay problemas

**Temperatura no actualiza:**
- Verificar suscripción al tópico `sensor/temperatura`
- Revisar función `updateChart()` en dashboard
- Check consola para errores JavaScript

### Herramientas de Debugging

**MQTT Explorer:** Herramienta gráfica para monitorear tópicos MQTT
```bash
# Instalar MQTT Explorer
# Descargar desde: https://mqtt-explorer.com/
```

**Wireshark:** Para capturar tráfico MQTT en red
**Monitor Serie:** Para ver logs del ESP32
**Console.log:** Para debugging del dashboard web

## 🔒 Seguridad

### Para Producción

**Autenticación MQTT:**
- Cambiar a broker privado
- Implementar usuario/password
- Usar SSL/TLS

**Seguridad Web:**
- Implementar autenticación en dashboard
- Usar HTTPS para conexión web
- Validar todos los mensajes MQTT

**Seguridad ESP32:**
- No hardcoder credenciales en firmware
- Usar OTA con autenticación
- Implementar watchdog timer

## 📈 Mejoras Futuras

**Funcionalidades adicionales:**
- [ ] Control de múltiples LEDs/dispositivos
- [ ] Soporte para más tipos de sensores
- [ ] Historial de datos en base de datos
- [ ] Alertas y notificaciones
- [ ] Configuración remota vía MQTT
- [ ] Actualización OTA del firmware
- [ ] Interfaz móvil responsiva mejorada
- [ ] Exportación de datos en CSV/PDF

**Mejoras técnicas:**
- [ ] Implementación de Quality of Service (QoS)
- [ ] Soporte para MQTT Retained messages
- [ ] Compresión de datos para ancho de banda limitado
- [ ] Caché local para desconexiones

## 📄 Licencia

Este proyecto es parte del sistema IOT-MQTT-Dashboard-for-ESP32-Plant-Sensor.

## 👥 Contribución

Para mejorar este sistema:
1. Testear con diferentes configuraciones
2. Reportar bugs y problemas
3. Sugerir mejoras y nuevas funcionalidades
4. Contribuir código para nuevas características

## 📞 Soporte

Para preguntas o problemas:
- Revisar sección de solución de problemas
- Verificar logs de todos los componentes
- Usar herramientas de debugging MQTT
- Documentar pasos para reproducir problemas

---

**Versión del Sistema:** 1.0 Completo  
**Última Actualización:** 2026-05-22  
**Estado:** ✅ Funcional y Probado
# 🚀 GUÍA RÁPIDA DE INICIO

## ⚡ Empezar en 5 minutos

### Opción 1: Solo Simulación (Sin Hardware)

1. **Ejecutar el simulador Python:**
   ```bash
   python simulador_mqtt.py
   ```

2. **Abrir el dashboard web:**
   - Abrir `dashboard.html` en tu navegador
   - Configurar conexión MQTT (valores por defecto ya están)
   - Presionar "Conectar"

3. **¡Probar!**
   - Cambiar el switch en el dashboard
   - Ver cómo el simulador responde
   - Observar la gráfica de temperatura actualizándose

### Opción 2: Con ESP32 Físico

1. **Configurar el ESP32:**
   - Abrir `esp32_mqtt_complete.ino` en Arduino IDE
   - Modificar las credenciales WiFi al principio del archivo
   - Opcional: Crear `config_secrets.h` para configuración externa

2. **Instalar librerías necesarias:**
   - En Arduino IDE: Tools → Manage Libraries
   - Instalar: "PubSubClient" y "DHT sensor library"

3. **Subir el firmware:**
   - Conectar ESP32 al computador
   - Seleccionar placa correcta (Tools → Board → ESP32 Dev Module)
   - Seleccionar puerto correcto
   - Presionar "Upload"

4. **Abrir monitor serie:**
   - Tools → Serial Monitor
   - Velocidad: 115200 baud
   - Verificar conexión WiFi y MQTT exitosa

5. **Usar el dashboard:**
   - Abrir `dashboard.html` en navegador
   - Conectar al broker MQTT
   - Controlar el ESP32 desde la web

## 🎯 Flujo de Trabajo Básico

```
Usuario Web → Dashboard → MQTT Broker → ESP32 → LED Físico
     ↑                                                      │
     └──────────────────────── MQTT Status ←───────────────┘
```

## 📋 Checklist de Configuración

### Para Simulación:
- [ ] Python instalado
- [ ] `pip install paho-mqtt`
- [ ] Ejecutar `simulador_mqtt.py`
- [ ] Abrir `dashboard.html`
- [ ] Conectar MQTT en dashboard

### Para ESP32:
- [ ] Arduino IDE con soporte ESP32
- [ ] Librerías PubSubClient y DHT instaladas
- [ ] Credenciales WiFi configuradas
- [ ] Código subido al ESP32
- [ ] Monitor serie mostrando conexión exitosa
- [ ] Dashboard conectado y funcionando

## 🔧 Configuración Rápida

### Dashboard Web:
```
Host: broker.hivemq.com
Puerto: 8000 (WebSockets)
SSL: No
Tópico: LED/control
```

### Simulador Python:
```bash
# Configuración por defecto, no requiere cambios
python simulador_mqtt.py
```

### ESP32:
```cpp
// Solo cambiar estas líneas:
const char* WIFI_SSID = "TU_RED_WIFI";
const char* WIFI_PASSWORD = "TU_CONTRASEÑA";
```

## 🧪 Pruebas Rápidas

### Test 1: Simulación Básica
1. Ejecutar simulador
2. Abrir dashboard
3. Conectar MQTT
4. Cambiar switch ON/OFF
5. Ver respuesta en consola Python

### Test 2: Sensores
1. Con dashboard y simulador funcionando
2. Observar gráfica de temperatura
3. Verificar actualización cada 5 segundos

### Test 3: Hardware ESP32
1. Seguir pasos para ESP32 físico
2. Verificar LED integrado responde a comandos
3. Check monitor serie para diagnóstico

## ⚠️ Problemas Comunes Rápidos

**Dashboard no conecta:**
- Verificar puerto 8000 (WebSockets), no 1883
- Revisar que broker.hivemq.com esté accesible

**ESP32 no conecta WiFi:**
- Verificar credenciales correctas
- Chequear señal WiFi

**Simulador Python error:**
- `pip install paho-mqtt`
- Verificar conexión a internet

## 📞 Ayuda

Para problemas detallados, revisar:
- `README_ESP32.md` - Documentación completa
- Monitor serie ESP32 para diagnóstico
- Consola navegador (F12) para dashboard
- Consola Python para simulador

---

**¡Listo para usar!** El sistema está completamente configurado y listo para controlar tus dispositivos.
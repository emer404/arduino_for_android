import paho.mqtt.client as mqtt
import time
import random
import threading

# ==========================================
# CONFIGURACIÓN DEL BROKER (Debe coincidir con el ESP32 y la web)
# ==========================================
MQTT_BROKER = "broker.hivemq.com"
# Nota: Python usa el puerto TCP estándar (1883), mientras que la web usa WebSockets (8000)
MQTT_PORT = 1883 

# Tópicos MQTT (deben coincidir con el ESP32)
MQTT_TOPIC_LED1 = "LED1/control"      # Tópico para comandos de control LED1
MQTT_TOPIC_LED2 = "LED2/control"      # Tópico para comandos de control LED2
MQTT_TOPIC_LED3 = "LED3/control"      # Tópico para comandos de control LED3
MQTT_TOPIC_LED1_STATUS = "LED1/status" # Tópico estado LED1
MQTT_TOPIC_LED2_STATUS = "LED2/status" # Tópico estado LED2
MQTT_TOPIC_LED3_STATUS = "LED3/status" # Tópico estado LED3
MQTT_TOPIC_TEMPERATURA = "sensor/temperatura"  # Tópico para datos de temperatura

# Estados simulados de los 3 LEDs
simulated_led1_state = False  # False = OFF, True = ON
simulated_led2_state = False  # False = OFF, True = ON
simulated_led3_state = False  # False = OFF, True = ON
simulated_temperature = 25.0

# ==========================================
# FUNCIONES CALLBACK MQTT
# ==========================================

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"✅ Conectado exitosamente al broker MQTT ({MQTT_BROKER})")
        # Suscribirse a los tópicos de control de los 3 LEDs
        client.subscribe(MQTT_TOPIC_LED1)
        print(f"📡 Suscrito al tópico de control LED1: {MQTT_TOPIC_LED1}")

        client.subscribe(MQTT_TOPIC_LED2)
        print(f"📡 Suscrito al tópico de control LED2: {MQTT_TOPIC_LED2}")

        client.subscribe(MQTT_TOPIC_LED3)
        print(f"📡 Suscrito al tópico de control LED3: {MQTT_TOPIC_LED3}")

        # Publicar estados iniciales
        publish_led_states()
    else:
        print(f"❌ Error al conectar, código: {rc}")

def on_message(client, userdata, msg):
    global simulated_led1_state, simulated_led2_state, simulated_led3_state
    topic = msg.topic
    payload = msg.payload.decode()

    print(f"📨 Mensaje recibido en '{topic}': {payload}")

    # Procesar comandos de control para cada LED
    if topic == MQTT_TOPIC_LED1:
        process_led_command(payload, 1, simulated_led1_state, MQTT_TOPIC_LED1_STATUS)
    elif topic == MQTT_TOPIC_LED2:
        process_led_command(payload, 2, simulated_led2_state, MQTT_TOPIC_LED2_STATUS)
    elif topic == MQTT_TOPIC_LED3:
        process_led_command(payload, 3, simulated_led3_state, MQTT_TOPIC_LED3_STATUS)
    else:
        print(f"⚠️ Tópico no reconocido: {topic}")

def on_publish(client, userdata, mid):
    # print(f"📤 Mensaje publicado con ID: {mid}")
    pass

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print(f"❌ Desconectado inesperadamente, código: {rc}")

# ==========================================
# FUNCIONES DE PROCESAMIENTO Y PUBLICACIÓN
# ==========================================

def process_led_command(payload, led_num, state_var, status_topic):
    """Procesa comandos para un LED específico"""
    global simulated_led1_state, simulated_led2_state, simulated_led3_state

    if payload.upper() == "ON":
        new_state = True
        print(f"💡 Encendiendo LED{led_num}...")
    elif payload.upper() == "OFF":
        new_state = False
        print(f"💡 Apagando LED{led_num}...")
    elif payload.upper() == "TOGGLE":
        new_state = not state_var
        print(f"💡 Alternando LED{led_num}...")
    else:
        print(f"⚠️ Comando no reconocido: {payload}")
        return

    # Actualizar el estado global correspondiente
    if led_num == 1:
        simulated_led1_state = new_state
    elif led_num == 2:
        simulated_led2_state = new_state
    elif led_num == 3:
        simulated_led3_state = new_state

    # Publicar el estado
    state_message = "ON" if new_state else "OFF"
    client.publish(status_topic, state_message)
    print(f"📤 Estado LED{led_num} publicado: {state_message}")

def publish_led_states():
    """Publica los estados de todos los LEDs"""
    state1 = "ON" if simulated_led1_state else "OFF"
    state2 = "ON" if simulated_led2_state else "OFF"
    state3 = "ON" if simulated_led3_state else "OFF"

    client.publish(MQTT_TOPIC_LED1_STATUS, state1)
    client.publish(MQTT_TOPIC_LED2_STATUS, state2)
    client.publish(MQTT_TOPIC_LED3_STATUS, state3)

    print("📤 Estados iniciales publicados:")
    print(f"   LED1: {state1}")
    print(f"   LED2: {state2}")
    print(f"   LED3: {state3}")

def publish_temperature():
    """Publica datos de temperatura simulados"""
    global simulated_temperature
    
    # Simular variación de temperatura
    variation = round(random.uniform(-0.5, 0.5), 1)
    simulated_temperature = round(simulated_temperature + variation, 1)
    
    # Mantener temperatura en rango realista
    simulated_temperature = max(20.0, min(35.0, simulated_temperature))
    
    client.publish(MQTT_TOPIC_TEMPERATURA, str(simulated_temperature))
    print(f"🌡️ Temperatura publicada: {simulated_temperature}°C")

def simulate_external_changes():
    """Simula cambios externos en el dispositivo (como si fuera el ESP32)"""
    global simulated_led1_state, simulated_led2_state, simulated_led3_state

    while True:
        time.sleep(15)  # Cada 15 segundos

        # Simular cambios aleatorios de estado (ocasionalmente)
        if random.random() < 0.3:  # 30% de probabilidad
            led_num = random.choice([1, 2, 3])
            if led_num == 1:
                simulated_led1_state = not simulated_led1_state
                state_msg = "ON" if simulated_led1_state else "OFF"
                client.publish(MQTT_TOPIC_LED1_STATUS, state_msg)
                print(f"🔄 Cambio externo LED1: {state_msg}")
            elif led_num == 2:
                simulated_led2_state = not simulated_led2_state
                state_msg = "ON" if simulated_led2_state else "OFF"
                client.publish(MQTT_TOPIC_LED2_STATUS, state_msg)
                print(f"🔄 Cambio externo LED2: {state_msg}")
            elif led_num == 3:
                simulated_led3_state = not simulated_led3_state
                state_msg = "ON" if simulated_led3_state else "OFF"
                client.publish(MQTT_TOPIC_LED3_STATUS, state_msg)
                print(f"🔄 Cambio externo LED3: {state_msg}")

        # Simular lectura de temperatura
        publish_temperature()

# ==========================================
# INICIALIZACIÓN DEL CLIENTE MQTT
# ==========================================

# Crear cliente con ID único
client = mqtt.Client(client_id="simulador_esp32_python")
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish
client.on_disconnect = on_disconnect

print("========================================")
print("🚀 Simulador MQTT ESP32 Iniciando...")
print("========================================")
print(f"📡 Broker: {MQTT_BROKER}:{MQTT_PORT}")
print(f"🎯 Tópico Control LED1: {MQTT_TOPIC_LED1}")
print(f"🎯 Tópico Control LED2: {MQTT_TOPIC_LED2}")
print(f"🎯 Tópico Control LED3: {MQTT_TOPIC_LED3}")
print(f"🌡️ Tópico Temperatura: {MQTT_TOPIC_TEMPERATURA}")
print("========================================")

try:
    # Conectar al broker
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    
    # Iniciar el hilo de red en segundo plano
    client.loop_start()
    
    # Iniciar hilo para simulación de cambios externos
    simulation_thread = threading.Thread(target=simulate_external_changes)
    simulation_thread.daemon = True
    simulation_thread.start()
    
    print("✅ Simulador iniciado correctamente")
    print("📋 Comandos disponibles desde el dashboard: ON, OFF, TOGGLE")
    print("   Para cada LED: LED1/control, LED2/control, LED3/control")
    print("⏱️ Simulando cambios externos cada 15 segundos")
    print("⌨️ Presiona Ctrl+C para detener\n")
    
    # Mantener el programa principal ejecutándose
    while True:
        time.sleep(1)
        
except KeyboardInterrupt:
    print("\n🛑 Simulación detenida por el usuario.")
    client.loop_stop()
    client.disconnect()
    print("👋 Conexión cerrada correctamente")

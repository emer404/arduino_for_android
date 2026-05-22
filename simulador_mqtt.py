import paho.mqtt.client as mqtt
import time
import random

# ==========================================
# CONFIGURACIÓN DEL BROKER (Debe coincidir con la web)
# ==========================================
MQTT_BROKER = "broker.hivemq.com"
# Nota: Python usa el puerto TCP estándar (1883), mientras que la web usa WebSockets (8000)
MQTT_PORT = 1883 
MQTT_TOPIC = "LED/control"

# Función que se ejecuta al conectar
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"✅ Conectado exitosamente al broker MQTT ({MQTT_BROKER})")
    else:
        print(f"❌ Error al conectar, código: {rc}")

# Inicializar cliente
client = mqtt.Client(client_id="simulador_arduino_python")
client.on_connect = on_connect

print("Iniciando simulador...")
client.connect(MQTT_BROKER, MQTT_PORT, 60)

# Iniciar el hilo de red en segundo plano
client.loop_start()

print("Enviando datos simulados cada 5 segundos. Presiona Ctrl+C para detener.\n")

try:
    while True:
        # 1. Simular envío de datos numéricos (ej. Temperatura) para tu gráfica
        temperatura = round(random.uniform(22.0, 28.5), 1)
        print(f"📡 Publicando temperatura: {temperatura}°C al tópico '{MQTT_TOPIC}'")
        client.publish(MQTT_TOPIC, str(temperatura))
        
        time.sleep(2) # Pausa de 2 segundos
        
        # 2. Simular un evento donde el Arduino cambia su propio estado ("ON" o "OFF")
        # Esto probará si tu dashboard detecta cambios externos
        estado_aleatorio = random.choice(["ON", "OFF"])
        print(f"💡 Publicando estado del dispositivo: {estado_aleatorio} al tópico '{MQTT_TOPIC}'")
        client.publish(MQTT_TOPIC, estado_aleatorio)
        
        print("-" * 50)
        time.sleep(5) # Esperar 5 segundos antes del siguiente ciclo
        
except KeyboardInterrupt:
    print("\n🛑 Simulación detenida por el usuario.")
    client.loop_stop()
    client.disconnect()

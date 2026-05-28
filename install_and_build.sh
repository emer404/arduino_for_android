#!/bin/bash
# ============================================================
#  Script de instalación completa para construir el APK
#  IoT MQTT Dashboard para ESP32
#  Ejecutar con: bash install_and_build.sh
# ============================================================

set -e   # salir si falla cualquier comando

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANDROID_APP_DIR="$PROJECT_DIR/android-app"
SDK_DIR="$PROJECT_DIR/android-sdk"
CMDLINE_TOOLS_VERSION="11076708"  # versión estable de cmdline-tools

echo ""
echo "╔══════════════════════════════════════════════╗"
echo "║   IoT MQTT Dashboard — Build Script         ║"
echo "╚══════════════════════════════════════════════╝"
echo ""

# ─────────────────────────────────────────────
# PASO 1: Java JDK 17
# ─────────────────────────────────────────────
echo "→ [1/5] Instalando Java JDK 17..."
if java -version 2>&1 | grep -q "17"; then
    echo "   ✅ Java 17 ya está instalado."
else
    sudo apt-get update -qq
    sudo apt-get install -y openjdk-17-jdk
    echo "   ✅ Java 17 instalado."
fi

export JAVA_HOME=$(dirname $(dirname $(readlink -f $(which java))))
echo "   JAVA_HOME=$JAVA_HOME"

# ─────────────────────────────────────────────
# PASO 2: Android Command Line Tools
# ─────────────────────────────────────────────
echo ""
echo "→ [2/5] Descargando Android Command Line Tools..."
if [ ! -d "$SDK_DIR" ]; then
    mkdir -p "$SDK_DIR/cmdline-tools"
    cd /tmp
    wget -q --show-progress -O cmdline-tools.zip \
        "https://dl.google.com/android/repository/commandlinetools-linux-${CMDLINE_TOOLS_VERSION}_latest.zip"
    unzip -q cmdline-tools.zip -d "$SDK_DIR/cmdline-tools"
    mv "$SDK_DIR/cmdline-tools/cmdline-tools" "$SDK_DIR/cmdline-tools/latest"
    rm cmdline-tools.zip
    echo "   ✅ Android Command Line Tools descargados."
else
    echo "   ✅ Android SDK ya existe en $SDK_DIR"
fi

export ANDROID_HOME="$SDK_DIR"
export ANDROID_SDK_ROOT="$SDK_DIR"
export PATH="$PATH:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/build-tools/33.0.2"

# ─────────────────────────────────────────────
# PASO 3: Aceptar licencias e instalar paquetes SDK
# ─────────────────────────────────────────────
echo ""
echo "→ [3/5] Instalando plataformas y build-tools de Android..."
yes | sdkmanager --licenses > /dev/null 2>&1 || true
sdkmanager "platform-tools" "build-tools;33.0.2" "platforms;android-33" 2>&1 | grep -E "Download|Install|done" || true
echo "   ✅ SDK instalado: platform-tools, build-tools;33.0.2, platforms;android-33"

# ─────────────────────────────────────────────
# PASO 4: Agregar plataforma Android a Cordova
# ─────────────────────────────────────────────
echo ""
echo "→ [4/5] Agregando plataforma Android a Cordova..."
cd "$ANDROID_APP_DIR"

if [ ! -d "platforms/android" ]; then
    cordova platform add android@12 2>&1
    echo "   ✅ Plataforma Android agregada."
else
    echo "   ✅ Plataforma Android ya existe."
fi

# ─────────────────────────────────────────────
# PASO 5: Build del APK
# ─────────────────────────────────────────────
echo ""
echo "→ [5/5] Compilando APK (modo debug)..."
cordova build android 2>&1

APK_PATH="$ANDROID_APP_DIR/platforms/android/app/build/outputs/apk/debug/app-debug.apk"

echo ""
if [ -f "$APK_PATH" ]; then
    SIZE=$(du -sh "$APK_PATH" | cut -f1)
    echo "╔══════════════════════════════════════════════════╗"
    echo "║   ✅ APK GENERADO EXITOSAMENTE                  ║"
    echo "╠══════════════════════════════════════════════════╣"
    echo "║   Archivo: app-debug.apk ($SIZE)                ║"
    echo "║   Ruta completa:                                ║"
    echo "║   $APK_PATH"
    echo "╠══════════════════════════════════════════════════╣"
    echo "║   Para instalar en tu dispositivo Android:      ║"
    echo "║   adb install \"$APK_PATH\"  ║"
    echo "║   O copia el archivo a tu teléfono y ábrelo    ║"
    echo "╚══════════════════════════════════════════════════╝"
else
    echo "❌ No se encontró el APK. Revisa los errores arriba."
    exit 1
fi

#!/bin/bash

set -e  # Salir inmediatamente si ocurre un error.

# Colores
TITLE_COLOR='\033[36m'  # Cian para los títulos
RESET='\033[0m'         # Reset para volver al color predeterminado

# Paso 1: Actualizar los repositorios e instalar Prometheus
echo -e "${TITLE_COLOR}------------- Updating and Installing Prometheus... -------------${RESET}"
sudo apt update
sudo apt install -y prometheus

# Paso 2: Crear el archivo prometheus.yml
echo -e "${TITLE_COLOR}------------- Generating prometheus config file... ----------------${RESET}"
cat <<EOF | sudo tee /etc/prometheus/prometheus.yml
global:
  scrape_interval: 15s
  evaluation_interval: 15s

scrape_configs:
  - job_name: 'custom_service'
    scrape_interval: 10s
    static_configs:
      - targets: ['localhost:8000']
EOF

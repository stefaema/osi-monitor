#!/bin/bash

set -e  # Salir inmediatamente si ocurre un error.

# Colores
TITLE_COLOR='\033[36m'  # Cian para los títulos
RESET='\033[0m'         # Reset para volver al color predeterminado

echo -e "${TITLE_COLOR}---------------- Installing Grafana... ----------------${RESET}"
sudo apt-get install -y apt-transport-https software-properties-common wget
sudo mkdir -p /etc/apt/keyrings/
wget -q -O - https://apt.grafana.com/gpg.key | gpg --dearmor | sudo tee /etc/apt/keyrings/grafana.gpg > /dev/null
echo "deb [signed-by=/etc/apt/keyrings/grafana.gpg] https://apt.grafana.com stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list
sudo apt-get update
sudo apt-get install grafana
echo -e "${TITLE_COLOR}Installation completed successfully.${RESET}"

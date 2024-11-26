#!/bin/bash

set -e  # Salir inmediatamente si ocurre un error.

# Colores
TITLE_COLOR='\033[35m'  # Magenta para los títulos
RESET='\033[0m'         # Reset para volver al color predeterminado

echo -e "${TITLE_COLOR}------------------- Plug and play started... --------------------${RESET}"
./install_dependencies.sh
./install_prometheus_server.sh
./install_grafana.sh

echo -e "${TITLE_COLOR}------------------- Starting Grafana... --------------------${RESET}"
sudo systemctl daemon-reload
sudo systemctl start grafana-server

echo -e "${TITLE_COLOR}------------------- Starting Prometheus... --------------------${RESET}"
sudo systemctl restart prometheus

echo -e "${TITLE_COLOR}------------------- Building monitor... --------------------${RESET}"
./build_project.sh

echo -e "${TITLE_COLOR}------------------- Starting monitor --------------------${RESET}"
cd build
./monitor

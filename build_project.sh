#!/bin/bash

set -e  # Salir inmediatamente si ocurre un error.

# Colores
TITLE_COLOR='\033[36m'  # Cian para los títulos
RESET='\033[0m'         # Reset para volver al color predeterminado

echo -e "${TITLE_COLOR}-------------------- Removing existing build directory if it exists... --------------------${RESET}"
rm -rf build

echo -e "${TITLE_COLOR}-------------------- Creating build directory... --------------------${RESET}"
mkdir -p build
cd build

echo -e "${TITLE_COLOR}-------------------- Configuring project with CMake... --------------------${RESET}"
cmake ..

echo -e "${TITLE_COLOR}-------------------- Building project with Make... --------------------${RESET}"
make

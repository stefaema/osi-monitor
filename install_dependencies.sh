#!/bin/bash

set -e  # Terminar inmediatamente si algún comando falla.

# Colores
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
MAGENTA='\033[35m'
CYAN='\033[36m'
RESET='\033[0m'

echo -e "${CYAN}------------------- Updating package list and installing dependencies... --------------------${RESET}"
sudo apt update
sudo apt install -y libmicrohttpd-dev cmake make gcc git

echo -e "${BLUE}------------------ Cloning prometheus-client-c... ------------------${RESET}"
LIB_DIR=$(pwd)/lib
mkdir -p $LIB_DIR

# Verificar si la carpeta prometheus-client-c ya existe y borrarla
if [ -d "$LIB_DIR/prometheus-client-c" ]; then
    echo -e "${YELLOW}Cleaning up existing prometheus-client-c directory...${RESET}"
    rm -rf "$LIB_DIR/prometheus-client-c"
fi

cd $LIB_DIR
git clone https://github.com/digitalocean/prometheus-client-c.git
cd prometheus-client-c

# Aplicar corrección al archivo si es necesario
echo -e "${MAGENTA}-------------------- Fixing, Patching and Tuning repo --------------------${RESET}"
echo -e "${GREEN}Applying patch to promhttp.c...${RESET}"
sed -i 's/&promhttp_handler/(MHD_AccessHandlerCallback)promhttp_handler/' promhttp/src/promhttp.c

# Limpieza de carpetas no necesarias
echo -e "${YELLOW}Cleaning up unnecessary folders...${RESET}"
rm -rf .github autolib docker docs examples promtest vendor

# Build prom library
echo -e "${CYAN}----------- Building prom and promhttp... ---------------${RESET}"
cd prom
mkdir -p build
cd build
cmake ..
make
sudo make install

# Build promhttp library
cd ../../promhttp
mkdir -p build
cd build
cmake ..
make
sudo make install

echo -e "${GREEN}Installation completed successfully.${RESET}"

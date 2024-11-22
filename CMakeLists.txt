# tp1/so-i-24-stefaema-master/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(so-i-24-stefaema-master)


# Add subdirectories
add_subdirectory(lib/prometheus-client-c/prom)
add_subdirectory(lib/prometheus-client-c/promhttp)

set(SOURCES
    src/main.c  # Asegúrate de que main.c esté en la raíz del proyecto
    src/metrics.c
    src/expose_metrics.c
)

# Incluir directorios necesarios para compilación
include_directories(
    include
    lib/prometheus-client-c/prom/include
    lib/prometheus-client-c/promhttp/include
    src
)

# Crear el ejecutable principal
add_executable(so-i-24-stefaema-master ${SOURCES})

# Enlazar librerías necesarias
target_link_libraries(so-i-24-stefaema-master
    prom
    promhttp
)

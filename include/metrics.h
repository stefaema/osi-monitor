/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de archivos /proc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @def MAX_LINE_LENGTH
 * @brief Tamaño del buffer para leer archivos.
 */
#define MAX_LINE_LENGTH 256
/**
 * @def DISKSTATS_PATH
 * @brief Ruta al archivo /proc/diskstats.
 */
#define DISKSTATS_PATH "/proc/diskstats"
/**
 * @def STAT_PATH
 * @brief Ruta al archivo /proc/stats.
 */
#define STAT_PATH "/proc/stat"
/**
 * @def NETDEV_PATH
 * @brief Ruta al archivo /proc/net/dev.
 */
#define NETDEV_PATH "/proc/net/dev"
/**
 * @def MEMORYINFO_PATH
 * @brief Ruta al archivo /proc/meminfo.
 */
#define MEMORYINFO_PATH "/proc/meminfo"
/**
 * @def ERROR
 * @brief Valor de retorno en caso de error.
 */
#define ERROR -1
/**
 * @def ERROR_D
 * @brief Valor de retorno en caso de error.
 */
#define ERROR_D -1.0

/**
 * @struct DiskStat_t
 * @brief Estructura para almacenar las estadísticas de disco.
 *
 * @var DiskStat_t::device_name
 * Nombre del dispositivo de disco.
 *
 * @var DiskStat_t::reads_completed
 * Número de lecturas completadas.
 *
 * @var DiskStat_t::writes_completed
 * Número de escrituras completadas.
 *
 * @var DiskStat_t::io_time
 * Tiempo total de I/O en milisegundos.
 */
typedef struct
{
    char device_name[32];
    unsigned long long reads_completed;
    unsigned long long writes_completed;
    unsigned long long io_time;
} DiskStat_t;

/**
 * @struct CPUstats_t
 * @brief Estructura para almacenar los tiempos de CPU.
 *
 * @var CPUstats_t::user
 * Tiempo de CPU en modo usuario.
 *
 * @var CPUstats_t::nice
 * Tiempo de CPU en modo usuario con prioridad baja.
 *
 * @var CPUstats_t::system
 * Tiempo de CPU en modo sistema.
 *
 * @var CPUstats_t::idle
 * Tiempo de CPU en modo inactivo.
 *
 * @var CPUstats_t::iowait
 * Tiempo de CPU esperando por I/O.
 *
 * @var CPUstats_t::irq
 * Tiempo de CPU atendiendo interrupciones de hardware.
 *
 * @var CPUstats_t::softirq
 * Tiempo de CPU atendiendo interrupciones de software.
 *
 * @var CPUstats_t::steal
 * Tiempo de CPU robado por máquinas virtuales.
 */
typedef struct
{
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
} CPUstats_t;

/**
 * @struct net_info_t
 * @brief Estructura para almacenar las estadísticas de red.
 *
 * @var net_info_t::rx_bytes
 * Número de bytes recibidos.
 *
 * @var net_info_t::rx_packets
 * Número de paquetes recibidos.
 *
 * @var net_info_t::rx_errors
 * Número de errores en la recepción.
 *
 * @var net_info_t::rx_dropped
 * Número de paquetes recibidos y descartados.
 *
 * @var net_info_t::rx_fifo
 * Número de errores FIFO en la recepción.
 *
 * @var net_info_t::rx_frame
 * Número de errores de trama en la recepción.
 *
 * @var net_info_t::rx_compressed
 * Número de paquetes comprimidos recibidos.
 *
 * @var net_info_t::rx_multicast
 * Número de paquetes multicast recibidos.
 *
 * @var net_info_t::tx_bytes
 * Número de bytes transmitidos.
 *
 * @var net_info_t::tx_packets
 * Número de paquetes transmitidos.
 *
 * @var net_info_t::tx_errors
 * Número de errores en la transmisión.
 *
 * @var net_info_t::tx_dropped
 * Número de paquetes transmitidos y descartados.
 *
 * @var net_info_t::tx_fifo
 * Número de errores FIFO en la transmisión.
 *
 * @var net_info_t::tx_colls
 * Número de colisiones en la transmisión.
 *
 * @var net_info_t::tx_carrier
 * Número de errores de portadora en la transmisión.
 *
 * @var net_info_t::tx_compressed
 * Número de paquetes comprimidos transmitidos.
 */
typedef struct
{
    unsigned long long rx_bytes;
    unsigned long long rx_packets;
    unsigned long long rx_errors;
    unsigned long long rx_dropped;
    unsigned long long rx_fifo;
    unsigned long long rx_frame;
    unsigned long long rx_compressed;
    unsigned long long rx_multicast;
    unsigned long long tx_bytes;
    unsigned long long tx_packets;
    unsigned long long tx_errors;
    unsigned long long tx_dropped;
    unsigned long long tx_fifo;
    unsigned long long tx_colls;
    unsigned long long tx_carrier;
    unsigned long long tx_compressed;
} net_info_t;

/**
 * @brief Obtiene el porcentaje de uso de memoria desde /proc/meminfo.
 *
 * Lee los valores de memoria total y disponible desde /proc/meminfo y calcula
 * el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_usage();

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage();

/**
 * @brief Obtiene la cantidad de cambios de contexto desde /proc/stat.
 *
 * Lee la cantidad de cambios de contexto desde /proc/stat y devuelve el valor.
 *
 * @return Cantidad de cambios de contexto, o -1 en caso de error.
 */
long long get_context_switches();

/**
 * @brief Obtiene la cantidad de procesos en ejecución desde /proc/stat.
 *
 * Lee la cantidad de procesos en ejecución desde /proc/stat y devuelve el valor.
 *
 * @return Cantidad de procesos en ejecución, o -1 en caso de error.
 */
long get_processes_running();

/**
 * @brief Obtiene la cantidad de paquetes multicast recibidos desde /proc/net/dev.
 *
 * Lee la cantidad de paquetes multicast recibidos desde /proc/net/dev y devuelve el valor.
 *
 * @return Cantidad de paquetes multicast recibidos, o -1 en caso de error.
 */
long get_rx_multicast_packets();

/**
 * @brief Obtiene la cantidad de paquetes perdidos desde /proc/net/dev.
 *
 * Lee la cantidad de paquetes perdidos desde /proc/net/dev y devuelve el valor.
 *
 * @return Cantidad de paquetes perdidos, o -1 en caso de error.
 */
long get_lost_packets_info();

/**
 * @brief Calcula la salud del disco desde /proc/diskstats.
 *
 * Lee la cantidad de sectores leídos y escritos desde /proc/diskstats y calcula
 * la salud del disco.
 *
 * @param device_name Nombre del dispositivo de almacenamiento.
 * @return Salud del disco como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double calculate_disk_health(const char* device_name);

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_event.h>
#include <driver/spi_master.h>
#include <driver/i2c.h> // TODO: Log warning: This driver is an old driver, please migrate your application code to adapt `driver/i2c_master.h`

/**
 * @file rc522.h
 * @brief Encabezado para el controlador RC522 RFID.
 */

//#define RC522_I2C_ADDRESS (0x28)

/**
 * @brief Intervalo de escaneo predeterminado del RC522 en milisegundos.
 */
#define RC522_DEFAULT_SCAN_INTERVAL_MS (125)

/**
 * @brief Tamaño predeterminado de la pila de la tarea del RC522.
 */
#define RC522_DEFAULT_TASK_STACK_SIZE (4 * 1024)

/**
 * @brief Prioridad predeterminada de la pila de la tarea del RC522.
 */
#define RC522_DEFAULT_TASK_STACK_PRIORITY (4)

/**
 * @brief Velocidad de reloj SPI predeterminada del RC522 en Hz.
 */
#define RC522_DEFAULT_SPI_CLOCK_SPEED_HZ (5000000)

/**
 * @brief Tiempo de espera predeterminado de lectura/escritura I2C del RC522 en milisegundos.
 */
#define RC522_DEFAULT_I2C_RW_TIMEOUT_MS (1000)

/**
 * @brief Velocidad de reloj I2C predeterminada del RC522 en Hz.
 */
#define RC522_DEFAULT_I2C_CLOCK_SPEED_HZ (100000)

/**
 * @brief Base de eventos para el controlador RC522.
 */
ESP_EVENT_DECLARE_BASE(RC522_EVENTS);

/**
 * @brief Manejador de la instancia del RC522.
 */
typedef struct rc522* rc522_handle_t;

/**
 * @brief Configuración del controlador RC522.
 */
typedef struct {
    uint16_t scan_interval_ms; /**< Intervalo de escaneo en milisegundos */
    size_t task_stack_size; /**< Tamaño de la pila de la tarea */
    uint8_t task_priority; /**< Prioridad de la tarea */
    union {
        struct {
            spi_host_device_t host; /**< Host SPI */
            int miso_gpio; /**< GPIO MISO */
            int mosi_gpio; /**< GPIO MOSI */
            int sck_gpio; /**< GPIO SCK */
            int sda_gpio; /**< GPIO SDA */
            int clock_speed_hz; /**< Velocidad de reloj en Hz */
            uint32_t device_flags; /**< Flags del dispositivo SPI */
            /**
             * @brief Indica si el bus SPI está inicializado.
             * @note Esta propiedad será eliminada en el futuro.
             */
            bool bus_is_initialized;
        } spi;
        // Aquí podrían añadirse configuraciones para otros buses (como I2C) si fuera necesario
    };
} rc522_config_t;

/**
 * @brief Eventos del RC522.
 */
typedef enum {
    RC522_EVENT_ANY = ESP_EVENT_ANY_ID, /**< Cualquier evento */
    RC522_EVENT_NONE, /**< Ningún evento */
    RC522_EVENT_TAG_SCANNED, /**< Tarjeta escaneada */
} rc522_event_t;

/**
 * @brief Datos del evento RC522.
 */
typedef struct {
    rc522_handle_t rc522; /**< Instancia del RC522 */
    void* ptr; /**< Puntero a datos del evento */
} rc522_event_data_t;

/**
 * @brief Datos de la etiqueta RFID del RC522.
 */
typedef struct {
    uint64_t serial_number; /**< Número de serie */
} rc522_tag_t;

/**
 * @brief Crea una instancia del controlador RC522.
 * 
 * Para iniciar el escaneo de etiquetas, llame a la función rc522_start.
 * 
 * @param config Configuración del RC522.
 * @param out_rc522 Puntero a la nueva instancia creada.
 * @return ESP_OK en caso de éxito.
 */
esp_err_t rc522_create(rc522_config_t* config, rc522_handle_t* out_rc522);

/**
 * @brief Registra eventos del RC522.
 * 
 * @param rc522 Instancia del RC522.
 * @param event Evento a registrar.
 * @param event_handler Manejador del evento.
 * @param event_handler_arg Argumento del manejador del evento.
 * @return ESP_OK en caso de éxito.
 */
esp_err_t rc522_register_events(rc522_handle_t rc522, rc522_event_t event, esp_event_handler_t event_handler, void* event_handler_arg);

/**
 * @brief Anula el registro de eventos del RC522.
 * 
 * @param rc522 Instancia del RC522.
 * @param event Evento a anular el registro.
 * @param event_handler Manejador del evento a anular el registro.
 * @return ESP_OK en caso de éxito.
 */
esp_err_t rc522_unregister_events(rc522_handle_t rc522, rc522_event_t event, esp_event_handler_t event_handler);

/**
 * @brief Inicia el escaneo de etiquetas.
 * 
 * Si ya está iniciado, simplemente se devuelve ESP_OK.
 * 
 * @param rc522 Instancia del RC522.
 * @return ESP_OK en caso de éxito.
 */
esp_err_t rc522_start(rc522_handle_t rc522);

/**
 * @brief Reanuda el escaneo de etiquetas.
 * 
 * Si ya está en ejecución, simplemente se devuelve ESP_OK.
 * 
 * @param rc522 Instancia del RC522.
 * @return ESP_OK en caso de éxito.
 */
#define rc522_resume(rc522) rc522_start(rc522)

/**
 * @brief Pausa el escaneo de etiquetas.
 * 
 * Si ya está pausado, simplemente se devuelve ESP_OK.
 * 
 * @param rc522 Instancia del RC522.
 * @return ESP_OK en caso de éxito.
 */
esp_err_t rc522_pause(rc522_handle_t rc522);

/**
 * @brief Destruye el controlador RC522 y libera todos los recursos.
 * 
 * No puede llamarse desde un manejador de eventos.
 * 
 * @param rc522 Instancia del RC522.
 * @return ESP_OK en caso de éxito.
 */
esp_err_t rc522_destroy(rc522_handle_t rc522);

#ifdef __cplusplus
}
#endif

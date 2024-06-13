#include <esp_log.h>
#include <inttypes.h>
#include "rc522.h"
#include "mqtt.h"
#include "wifi.h"
#include "wifi_credentials.h"
#include <driver/gpio.h>
#include <string.h>


#define LED_GPIO    2
#define MQTT_URL                        "mqtt://broker.hivemq.com"



static const char* TAG = "Lector de tarjetas";
static bool tarjeta_detectada = false;

static rc522_handle_t scanner;


rc522_config_t config = {
    .spi.host = VSPI_HOST,
    .spi.miso_gpio = 25,
    .spi.mosi_gpio = 23, 
    .spi.sck_gpio = 19,
    .spi.sda_gpio = 22,
    .spi.device_flags= SPI_DEVICE_HALFDUPLEX
};


#define LEN_BUFFER      100
char buffer[LEN_BUFFER]= {0};
// Variable que contiene el número a comparar
const char* tarjeta_valida = "103021093187";
// Cuando detecto una tarjeta genero un evento que se procesa aqui
// Es algo parecido a una interrupción por software (basado en eventos de FreeRTOS)
static void rc522_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    rc522_event_data_t* data = (rc522_event_data_t*) event_data;

    switch(event_id) {
        case RC522_EVENT_TAG_SCANNED: {
                rc522_tag_t* tag = (rc522_tag_t*) data->ptr;
                sprintf(buffer, "Verificar tarjeta [sn: %" PRIu64 "]", tag->serial_number);
                ESP_LOGI(TAG, "%s\n", buffer);

                // Publicar el valor de la tarjeta al tópico grupob_request
                mqtt_publish(buffer, "grupob_request", 2, 0);
            }
            break;
    }
}

// Función para extraer el valor de "resultado" del mensaje JSON recibido
int extract_resultado_from_json(const char* json_message) {
    const char* key = "\"resultado\":";
    const char* start = strstr(json_message, key);
    
    if (start != NULL) {
        start += strlen(key); // Mover el puntero al inicio del valor de resultado
        char* end;
        int resultado = strtol(start, &end, 10); // Convertir la parte numérica a entero
        
        if (start != end) { // Verificar que se haya encontrado un número válido
            return resultado;
        }
    }
    
    return -1; // Valor por defecto si no se encuentra o no es válido
}

static void RFID_reader_init(){
 rc522_create(&config, &scanner);
 rc522_register_events(scanner, RC522_EVENT_ANY, rc522_handler, NULL);
 rc522_start(scanner);
}

// Función para procesar el mensaje recibido desde MQTT
void get_data(const char* data, const char* topic) {
    printf("\n[%s] %s\n", topic, data);
    
    // Comprobar si el mensaje proviene del tópico "grupob_request1"
    if (strcmp(topic, "grupob_request1") == 0) {
        // Mostrar el mensaje recibido por consola
        printf("Mensaje recibido en 'grupob_request1': %s\n", data);
        
        // Extraer el valor de "resultado" del mensaje JSON
        int resultado = extract_resultado_from_json(data);
        
        // Verificar si se pudo extraer correctamente el resultado
        if (resultado != -1) {
            printf("Valor del resultado: %d\n", resultado);
            
            // Encender o apagar el LED según el valor de resultado
            if (resultado == 1) {
                printf("Encendiendo el LED.\n");
                // Encender el LED
                gpio_set_level(LED_GPIO, 1); // Activar para encender el LED
            } else if (resultado == 0) {
                printf("Apagando el LED.\n");
                // Apagar el LED
                gpio_set_level(LED_GPIO, 0); // Activar para apagar el LED
            } else {
                printf("Valor de resultado no reconocido.\n");
            }
        } else {
            printf("No se pudo extraer el valor de resultado del mensaje JSON.\n");
        }
    }
}




static void mqtt_connected(){
    printf("MQTT conectado, suscribiéndose al tópico 'grupob_request1'.\n");
    mqtt_subcribe("grupob_request1", 2); // Suscripción al tópico grupob_request1 con QoS 2
}

static void callback_wifi_connected(){
    printf("WiFi conectada. Iniciando MQTT...\n");
    mqtt_init(MQTT_URL, mqtt_connected, NULL, get_data);
}


static void led_config(){
    gpio_set_direction(LED_GPIO,GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO,0);
}




int app_main()
{
    wifi_connect(WIFI_CREDENTIALS_ID,WIFI_CREDENTIALS_PASS,callback_wifi_connected,NULL);
    //Configurar LED
    led_config();

    RFID_reader_init();

    while(1) {
        if (!tarjeta_detectada) {
            gpio_set_level(LED_GPIO, 0); // Apaga el LED si la tarjeta no está presente
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

   
    return 0;
}
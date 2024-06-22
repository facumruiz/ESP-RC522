#include <esp_log.h>
#include <inttypes.h>
#include "rc522.h"
#include "mqtt.h"
#include "wifi.h"
#include "wifi_credentials.h"
#include <driver/gpio.h>
#include <string.h>
#include "config.h"
#include <stdio.h>


static const char* TAG = "Lector de tarjetas";
static bool tarjeta_detectada = false;

static rc522_handle_t scanner;


rc522_config_t config = {
    .spi.host = VSPI_HOST,
    .spi.miso_gpio = RC522_MISO_GPIO,
    .spi.mosi_gpio = RC522_MOSI_GPIO,
    .spi.sck_gpio = RC522_SCK_GPIO,
    .spi.sda_gpio = RC522_SDA_GPIO,
    .spi.device_flags= SPI_DEVICE_HALFDUPLEX
};


#define LEN_BUFFER      100
char buffer[LEN_BUFFER]= {0};

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
                mqtt_publish(buffer, MQTT_TOPIC_PUB, 2, 0);
            }
            break;
    }
}


static void RFID_reader_init(){
 rc522_create(&config, &scanner);
 rc522_register_events(scanner, RC522_EVENT_ANY, rc522_handler, NULL);
 rc522_start(scanner);
}

// Función para procesar el mensaje recibido desde MQTT
void get_data(const char* data) {
    printf("\nMensaje recibido: %s\n", data);

    // Convertir el mensaje a un entero
    int resultado = atoi(data);

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
}



static void mqtt_connected(){
    printf("MQTT conectado, suscribiéndose al tópico 'grupob_request1'.\n");
    mqtt_subcribe(MQTT_TOPIC_RES, 2); // Suscripción al tópico grupob_request1 con QoS 2
    gpio_set_level(LED_GPIO, 1); // enciendo LED cuando se conecte a MQTT
}

static void callback_wifi_connected(){
    printf("WiFi conectada. Iniciando MQTT...\n");
    mqtt_init(MQTT_URL, mqtt_connected, NULL, get_data);
}


static void led_config(){
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
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
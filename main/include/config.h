/**
 * @file config.h
 * @brief Archivo de configuración para definiciones de pines y parámetros.
 */

#ifndef CONFIG_H
#define CONFIG_H

// Incluir las bibliotecas necesarias aquí, si es necesario

/**
 * @brief GPIO donde está conectado el LED interno del ESP.
 * 
 * Dependiendo del microcontrolador (ESP32 o ESP8266), este valor puede variar.
 * Para ESP32, el LED interno suele estar en GPIO 2.
 * Para ESP8266, puede estar en GPIO 1 o GPIO 2.
 */
#define LED_GPIO    2
#define RC522_MISO_GPIO  25
#define RC522_MOSI_GPIO  23
#define RC522_SCK_GPIO   19
#define RC522_SDA_GPIO   22

// Configuración de MQTT
#define MQTT_URL    "mqtt://broker.hivemq.com"
#define MQTT_TOPIC_PUB  "grupob_request"
#define MQTT_TOPIC_RES  "grupob_request1"

#endif /* CONFIG_H */

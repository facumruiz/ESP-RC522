#ifndef _MQTT_H
#define _MQTT_H

#include "base.h"

/**
 * @file mqtt.h
 * @brief Definiciones y funciones para la comunicación MQTT.
 */

/**
 * @brief Configura e inicia la conexión al servidor MQTT.
 * 
 * Esta función establece la conexión inicial al servidor MQTT especificado por `uri`.
 * Se pueden proporcionar callbacks para manejar eventos de conexión, desconexión y recepción de datos.
 * 
 * @param uri URL del servidor MQTT.
 * @param conn Función de callback llamada al conectarse al servidor.
 * @param disconn Función de callback llamada al desconectarse del servidor.
 * @param data_received Función de callback llamada al recibir datos desde un canal subscripto.
 * @return esp_err_t ESP_OK si la operación se realizó correctamente, de lo contrario un código de error.
 */
esp_err_t mqtt_init(const char* uri, esp_callback_t conn, esp_callback_t disconn, callback_data_t data_received);

/**
 * @brief Publica un mensaje en un tópico MQTT específico.
 * 
 * Esta función publica el mensaje `msg` en el tópico `topic` con el nivel de calidad de servicio (QoS) especificado.
 * 
 * @param msg Mensaje a publicar.
 * @param topic Tópico MQTT donde se publicará el mensaje.
 * @param qos Nivel de calidad de servicio (QoS) para la publicación.
 * @param retain Indicador para retener el mensaje en el broker MQTT.
 * @return esp_err_t ESP_OK si la publicación se realizó correctamente, de lo contrario un código de error.
 */
esp_err_t mqtt_publish(const char* msg, const char* topic, int qos, int retain);

/**
 * @brief Publica un mensaje en un tópico MQTT específico utilizando una cola.
 * 
 * Esta función publica el mensaje `msg` en el tópico `topic` utilizando una cola para manejar la publicación de manera asincrónica.
 * 
 * @param msg Mensaje a publicar.
 * @param topic Tópico MQTT donde se publicará el mensaje.
 * @param qos Nivel de calidad de servicio (QoS) para la publicación.
 * @param retain Indicador para retener el mensaje en el broker MQTT.
 * @return esp_err_t ESP_OK si la publicación en la cola se realizó correctamente, de lo contrario un código de error.
 */
esp_err_t mqtt_publish_on_queue(const char* msg, const char* topic, int qos, int retain);

/**
 * @brief Suscribe el cliente MQTT a un tópico específico.
 * 
 * Esta función suscribe el cliente MQTT al tópico especificado con el nivel de calidad de servicio (QoS) indicado.
 * 
 * @param topic Tópico MQTT al que se desea suscribir.
 * @param qos Nivel de calidad de servicio (QoS) para la suscripción.
 * @return esp_err_t ESP_OK si la suscripción se realizó correctamente, de lo contrario un código de error.
 */
esp_err_t mqtt_subcribe(char* topic, int qos);

#endif /* _MQTT_H */

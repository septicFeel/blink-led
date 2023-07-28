#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"

#define BLINK_LED 12
#define BUZZ 13
#define blinkNumber 4

#include "nvs_flash.h"
#include "nvs.h"
#define RESTART_COUNTER_KEY "restart_count"
#define RESTART_MAGIC_NUMBER 0xDEADBEEF // magic number to check if EEPROM data is valid

static uint32_t restart_count = 0;

void increment_restart_count(){

    nvs_handle_t nvs_handle;
    esp_err_t err;

    // open the non volatile storage -NVS namespace
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK){
        printf("Error opening NVS\n");
        return;
    }

    // read the current restart count
    err = nvs_get_u32(nvs_handle, RESTART_COUNTER_KEY, &restart_count);

    // let's check if data exists in NVS and it matches the magic number
    if (err == ESP_OK && restart_count != 0 && restart_count != RESTART_MAGIC_NUMBER) 
        restart_count++;
    else 
        restart_count = 1;

    // store the now updated restart count in nvs
    err = nvs_set_u32(nvs_handle, RESTART_COUNTER_KEY, restart_count);
    if (err != ESP_OK)
        printf("Error updating NVS\n");

    // commit changes to NVS
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
        printf("Error commiting to NVS\n");

    // now close the NVS handle
    nvs_close(nvs_handle);

}

// reset counter function

void reset_restart_counter(){

    nvs_handle_t nvs_handle;
    esp_err_t err;

    // open NVS namespace
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK){
        printf("Error opening NVS\n");
        return;
    }

    //reset counter to 0
    restart_count = 0;

    //store updated restart count
    err = nvs_set_u32(nvs_handle, RESTART_COUNTER_KEY, restart_count);
    if (err != ESP_OK) 
        printf("Error updating NVS\n");

    // commit changes to NVS
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) printf("Error commiting to NVS\n");

    //close NVS
    nvs_close(nvs_handle);

}

void init (void){

    char *ourTaskName = pcTaskGetName(NULL);
    ESP_LOGI(ourTaskName, "a restart occured\n");

    gpio_reset_pin(BLINK_LED);
    gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

}

void restartOccured (){

// notify a board restart with LED blinkNumber 
    for ( int i = 0; i < blinkNumber; i++){
            gpio_set_level(BLINK_LED, 1);
            vTaskDelay(200/ portTICK_PERIOD_MS);
            gpio_set_level(BLINK_LED, 0);
            vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void blinkLoop (void){

    while (1){
        gpio_set_level(BLINK_LED, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_LED, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}

void app_main(void)
{

    // init LED pin
    init();

    // init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        // this means NVS partition is full, erase it and try again
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // increment the restart count
    increment_restart_count();

    // print the restart count
    printf("restart count is: %ld\n", restart_count);
    
    // notify with LED blinks that the restart occured
    restartOccured();

    if (restart_count >= 10) {
        reset_restart_counter();
    // one short buzz
        gpio_set_level(BUZZ, 1);
        vTaskDelay(200/ portTICK_PERIOD_MS);
        gpio_set_level(BUZZ, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    while (restart_count != 3 ) {
        
        blinkLoop();
    // one longer buzz
        gpio_set_level(BUZZ, 1);
        vTaskDelay(500/ portTICK_PERIOD_MS);
        gpio_set_level(BUZZ, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    gpio_set_level(BLINK_LED, 1);
}
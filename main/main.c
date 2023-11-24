#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//error squiggles are disabled yianni

void app_main(void)
{
    while(1){
        xTaskAbortDelay(10/portTICK_PERIOD_MS);
    }
}

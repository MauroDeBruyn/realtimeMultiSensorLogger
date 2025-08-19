#include "cyhal.h"
#include "cybsp.h"
#include <stdio.h>

#define ALS_PIN P10_0  // Check in Device Configurator voor jouw board

int main(void)
{
    cybsp_init();
    __enable_irq();

    cyhal_adc_t adc;
    cyhal_adc_channel_t adc_chan;

    // Init ADC
    cyhal_adc_init(&adc, ALS_PIN, NULL);

    // Init ALS channel
    cyhal_adc_channel_config_t chan_config = {
        .enable_averaging = false,
        .min_acquisition_ns = 1000,
        .enabled = true
    };
    cyhal_adc_channel_init_diff(&adc_chan, &adc, ALS_PIN, CYHAL_ADC_VNEG, &chan_config);

    for (;;)
    {
        int32_t raw = cyhal_adc_read_u16(&adc_chan);
        float voltage = (raw / 65535.0f) * 3.3f;
        printf("ALS raw: %ld, Voltage: %.3f V\r\n", raw, voltage);
        cyhal_system_delay_ms(500);
    }
}

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "tft_task.h"
#include "FreeRTOS.h"
#include "task.h"

/* Pins mapping for the ST7789v display */
const mtb_st7789v_pins_t tft_pins =
{
    .db08 = CYBSP_J2_2,
    .db09 = CYBSP_J2_4,
    .db10 = CYBSP_J2_6,
    .db11 = CYBSP_J2_10,
    .db12 = CYBSP_J2_12,
    .db13 = CYBSP_D7,
    .db14 = CYBSP_D8,
    .db15 = CYBSP_D9,
    .nrd  = CYBSP_D10,
    .nwr  = CYBSP_D11,
    .dc   = CYBSP_D12,
    .rst  = CYBSP_D13
};

void tft_task(void *arg)
{
    cy_rslt_t result;

    // Initialize user button as input with pullup
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                             CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    // Initialize user LED as output, off initially
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    // Initialize the ST7789v display controller
    result = mtb_st7789v_init8(&tft_pins);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    // Initialize emWin GUI library
    GUI_Init();

    while(1)
    {
        GUI_Clear();  // Clear screen

        // Set font and text color
        GUI_SetFont(&GUI_Font24_ASCII);
        GUI_SetColor(GUI_RED);

        // Draw a rectangle border
        GUI_DrawRect(0, 0, 317, 30);

        // Display custom text inside the rectangle
        GUI_DispStringAt("Sensor Data", 100, 2);

        // Blink user LED as a simple heartbeat indicator
        cyhal_gpio_toggle(CYBSP_USER_LED);

        // Delay 1 second (adjust for your needs)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define UART_TIMEOUT_MS   (50u)
#define INPUT_TIMEOUT_MS  (60000u)   /* 1 minuut om tijd in te voeren */
#define STRING_BUFFER_SIZE (80)
#define TM_YEAR_BASE      (1900u)

/* RTC object */
static cyhal_rtc_t rtc_obj;

/* -------------------- Helpers -------------------- */
static cy_rslt_t fetch_time_data(char *buffer, uint32_t timeout_ms)
{
    uint32_t idx = 0;
    uint8_t ch;
    memset(buffer, 0, STRING_BUFFER_SIZE);

    while (idx < STRING_BUFFER_SIZE-1)
    {
        cy_rslt_t rslt = cyhal_uart_getc(&cy_retarget_io_uart_obj, &ch, UART_TIMEOUT_MS);
        if (rslt == CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
        {
            timeout_ms -= UART_TIMEOUT_MS;
            if (timeout_ms == 0) return CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT;
            continue;
        }

        if (ch == '\r' || ch == '\n') break;
        buffer[idx++] = (char)ch;
    }
    return CY_RSLT_SUCCESS;
}

/* Vraag gebruiker om tijd in HH MM SS dd mm yyyy */
static void ask_and_set_time(void)
{
    char buf[STRING_BUFFER_SIZE];
    printf("RTC not initialized. Enter time as \"HH MM SS dd mm yyyy\":\r\n");

    if (fetch_time_data(buf, INPUT_TIMEOUT_MS) == CY_RSLT_SUCCESS)
    {
        int hh, mm, ss, dd, mo, yy;
        if (6 == sscanf(buf, "%d %d %d %d %d %d",
                        &hh,&mm,&ss,&dd,&mo,&yy))
        {
            struct tm t = {0};
            t.tm_hour = hh; t.tm_min = mm; t.tm_sec = ss;
            t.tm_mday = dd; t.tm_mon = mo - 1;
            t.tm_year = yy - TM_YEAR_BASE;
            cyhal_rtc_write(&rtc_obj, &t);
            printf("RTC set to: %04d-%02d-%02d %02d:%02d:%02d\r\n",
                   yy, mo, dd, hh, mm, ss);
        }
        else
        {
            printf("Invalid format, try reset and enter again.\r\n");
        }
    }
    else
    {
        printf("Timeout waiting for time input. Using default 2000-01-01 00:00:00\r\n");
        struct tm t = {0};
        t.tm_year = 2000 - TM_YEAR_BASE;
        t.tm_mon  = 0; t.tm_mday = 1;
        cyhal_rtc_write(&rtc_obj, &t);
    }
}

/* -------------------- Main -------------------- */
int main(void)
{
    cybsp_init();
    __enable_irq();
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);

    printf("=== RTC Logger Example ===\r\n");

    if (CY_RSLT_SUCCESS != cyhal_rtc_init(&rtc_obj))
    {
        printf("RTC init failed!\r\n");
        CY_ASSERT(0);
    }

    /* Check of RTC al loopt, anders tijd vragen */
    if (!cyhal_rtc_is_enabled(&rtc_obj))
    {
        ask_and_set_time();
    }

    /* Elke seconde de tijd loggen op nieuwe regel */
    struct tm now;
    char line[64];
    for(;;)
    {
        cyhal_rtc_read(&rtc_obj, &now);
        strftime(line, sizeof(line), "%Y-%m-%d %H:%M:%S", &now);
        printf("%s\r\n", line);
        cyhal_system_delay_ms(1000);
    }
}

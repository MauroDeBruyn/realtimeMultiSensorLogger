#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#define UART_BAUD             (115200)
#define PCM_SAMPLE_RATE_HZ    (16000)
#define DECIMATION_RATE       (64)
#define PCM_WORD_LEN_BITS     (16)
#define FRAME_SAMPLES         (256)
#define PDM_LEFT_GAIN_DB      (12)
#define PDM_RIGHT_GAIN_DB     (12)

#define PDM_DATA_PIN          (P10_5)   // DATA van mic (shield)
#define PDM_CLK_PIN           (P10_4)   // CLK naar mic  (shield)

static cyhal_pdm_pcm_t pdm;
static int16_t pcm_buf[FRAME_SAMPLES];

static void pdm_init(void)
{
    cy_rslt_t rslt;
    const cyhal_pdm_pcm_cfg_t cfg = {
        .sample_rate     = PCM_SAMPLE_RATE_HZ,
        .decimation_rate = DECIMATION_RATE,
        .mode            = CYHAL_PDM_PCM_MODE_RIGHT, // mic gedraagt zich als RIGHT
        .word_length     = PCM_WORD_LEN_BITS,
        .left_gain       = PDM_LEFT_GAIN_DB,
        .right_gain      = PDM_RIGHT_GAIN_DB
    };

    rslt = cyhal_pdm_pcm_init(&pdm, PDM_DATA_PIN, PDM_CLK_PIN, NULL, &cfg);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("PDM init failed: 0x%08lx\r\n", (unsigned long)rslt);
        CY_ASSERT(0);
    }
    rslt = cyhal_pdm_pcm_start(&pdm);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("PDM start failed: 0x%08lx\r\n", (unsigned long)rslt);
        CY_ASSERT(0);
    }
}

static size_t pdm_read_blocking(int16_t *dst, size_t want)
{
    size_t filled = 0;
    while (filled < want) {
        size_t n = want - filled;
        cy_rslt_t r = cyhal_pdm_pcm_read(&pdm, &dst[filled], &n);
        if (r != CY_RSLT_SUCCESS) {
            printf("read err: 0x%08lx\r\n", (unsigned long)r);
            break;
        }
        filled += n;
    }
    return filled;
}

int main(void)
{
    cy_rslt_t rslt = cybsp_init();
    if (rslt != CY_RSLT_SUCCESS) { CY_ASSERT(0); }
    __enable_irq();

    rslt = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, UART_BAUD);
    if (rslt != CY_RSLT_SUCCESS) { CY_ASSERT(0); }

    printf("\r\nPDM->PCM demo start (mono RIGHT, 16kHz)\r\n");
    pdm_init();

    for (;;) {
        size_t filled = pdm_read_blocking(pcm_buf, FRAME_SAMPLES);
        printf("PCM[%u]: ", (unsigned)filled);
        size_t show = filled < 12 ? filled : 12;
        for (size_t i = 0; i < show; ++i) printf("%d ", (int)pcm_buf[i]);
        printf("...\r\n");
        cyhal_system_delay_ms(2);
    }
}

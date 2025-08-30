#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

// -------- BMI160 basis ------------
#define IMU_ADDR            0x68     // 0x69 als SDO hoog is
#define REG_CHIP_ID         0x00     // verwacht 0xD1 (BMI160)
#define REG_CMD             0x7E
#define CMD_SOFTRESET       0xB6
#define CMD_ACC_NORMAL      0x11
#define CMD_GYR_NORMAL      0x15
#define REG_ACC_RANGE       0x41     // 0x03 = ±2g
#define REG_GYR_RANGE       0x43     // 0x00 = ±2000 dps
#define REG_GYR_DATA        0x0C     // 6 bytes: gx_l..gz_h
#define REG_ACC_DATA        0x12     // 6 bytes: ax_l..az_h

// Schaal (standaard bij ±2g en ±2000 dps)
#define ACC_LSB_PER_G       16384.0f
#define GYR_LSB_PER_DPS     16.4f

static cyhal_i2c_t i2c;

static cy_rslt_t i2c_write_u8(uint8_t dev, uint8_t reg, uint8_t val) {
    uint8_t b[2] = {reg, val};
    return cyhal_i2c_master_write(&i2c, dev, b, 2, 0, true);
}

static cy_rslt_t i2c_read(uint8_t dev, uint8_t reg, uint8_t *buf, size_t len) {
    cy_rslt_t r = cyhal_i2c_master_write(&i2c, dev, &reg, 1, 0, false);
    if (r != CY_RSLT_SUCCESS) return r;
    return cyhal_i2c_master_read(&i2c, dev, buf, len, 0, true);
}

int main(void)
{
    cy_rslt_t r = cybsp_init();
    CY_ASSERT(r == CY_RSLT_SUCCESS);
    __enable_irq();

    // UART → KitProg3 USB-UART (115200 8N1)
    r = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    CY_ASSERT(r == CY_RSLT_SUCCESS);
    printf("\nBMI160 HAL demo start\n");

    // I2C op Arduino D14/D15 => P6.1 (SDA), P6.0 (SCL)
    r = cyhal_i2c_init(&i2c, P6_1, P6_0, NULL);
    CY_ASSERT(r == CY_RSLT_SUCCESS);
    cyhal_i2c_cfg_t cfg = { .is_slave=false, .address=0, .frequencyhal_hz=100000 }; // begin op 100 kHz
    r = cyhal_i2c_configure(&i2c, &cfg);
    CY_ASSERT(r == CY_RSLT_SUCCESS);

    // CHIP_ID check
    uint8_t id = 0;
    r = i2c_read(IMU_ADDR, REG_CHIP_ID, &id, 1);
    if (r != CY_RSLT_SUCCESS) { printf("CHIP_ID read fail (0x%08lX)\n",(unsigned long)r); for(;;); }
    printf("CHIP_ID = 0x%02X (verwacht 0xD1)\n", id);

    // Soft reset + power modes
    i2c_write_u8(IMU_ADDR, REG_CMD, CMD_SOFTRESET);
    cyhal_system_delay_ms(100);

    i2c_write_u8(IMU_ADDR, REG_CMD, CMD_ACC_NORMAL);
    cyhal_system_delay_ms(5);
    i2c_write_u8(IMU_ADDR, REG_CMD, CMD_GYR_NORMAL);
    cyhal_system_delay_ms(80);

    // Ranges
    i2c_write_u8(IMU_ADDR, REG_ACC_RANGE, 0x03); // ±2g
    i2c_write_u8(IMU_ADDR, REG_GYR_RANGE, 0x00); // ±2000 dps

    printf("IMU configured (±2g, ±2000 dps)\n");

    // Lees-lus
    for (;;)
    {
        uint8_t ga[6], aa[6];

        if (i2c_read(IMU_ADDR, REG_GYR_DATA, ga, 6) != CY_RSLT_SUCCESS) { printf("gyro read err\n"); continue; }
        if (i2c_read(IMU_ADDR, REG_ACC_DATA, aa, 6) != CY_RSLT_SUCCESS) { printf("acc  read err\n"); continue; }

        int16_t gx = (int16_t)((ga[1]<<8) | ga[0]);
        int16_t gy = (int16_t)((ga[3]<<8) | ga[2]);
        int16_t gz = (int16_t)((ga[5]<<8) | ga[4]);

        int16_t ax = (int16_t)((aa[1]<<8) | aa[0]);
        int16_t ay = (int16_t)((aa[3]<<8) | aa[2]);
        int16_t az = (int16_t)((aa[5]<<8) | aa[4]);

        printf("A[g]=[% .3f % .3f % .3f]  G[dps]=[% .1f % .1f % .1f]\n",
               ax/ACC_LSB_PER_G, ay/ACC_LSB_PER_G, az/ACC_LSB_PER_G,
               gx/GYR_LSB_PER_DPS, gy/GYR_LSB_PER_DPS, gz/GYR_LSB_PER_DPS);

        cyhal_system_delay_ms(500);
    }
}

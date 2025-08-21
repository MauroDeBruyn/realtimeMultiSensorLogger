#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyhal_utils.h"   // defines CY_RETURN_IF_ERROR(...)


// ---- MPU6050 basics ----
#define MPU6050_ADDR_AD0_LOW   (0x68)
#define MPU6050_ADDR_AD0_HIGH  (0x69)
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_SMPLRT_DIV 0x19
#define MPU6050_REG_CONFIG     0x1A
#define MPU6050_REG_GYRO_CFG   0x1B
#define MPU6050_REG_ACCEL_CFG  0x1C
#define MPU6050_REG_WHO_AM_I   0x75
#define MPU6050_REG_ACCEL_XOUT 0x3B

static cyhal_i2c_t i2c;
static const uint8_t mpu_addr = MPU6050_ADDR_AD0_LOW; // change to HIGH if AD0 = 1

static cy_rslt_t i2c_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    return cyhal_i2c_master_write(&i2c, mpu_addr, buf, 2, 0, true);
}

static cy_rslt_t i2c_read(uint8_t reg, uint8_t *data, size_t len) {
    // Write reg, no stop, then read
    cy_rslt_t r = cyhal_i2c_master_write(&i2c, mpu_addr, &reg, 1, 0, false);
    if (r != CY_RSLT_SUCCESS) return r;
    return cyhal_i2c_master_read(&i2c, mpu_addr, data, len, 0, true);
}

static cy_rslt_t mpu6050_init(void)
{
    cy_rslt_t r;

    r = i2c_write(MPU6050_REG_PWR_MGMT_1, 0x01); if (r != CY_RSLT_SUCCESS) return r; // wake + PLL
    r = i2c_write(MPU6050_REG_CONFIG,       0x03); if (r != CY_RSLT_SUCCESS) return r; // DLPF = 3
    r = i2c_write(MPU6050_REG_SMPLRT_DIV,   9);    if (r != CY_RSLT_SUCCESS) return r; // 100 Hz
    r = i2c_write(MPU6050_REG_GYRO_CFG,     0x00); if (r != CY_RSLT_SUCCESS) return r; // ±250 dps
    r = i2c_write(MPU6050_REG_ACCEL_CFG,    0x00); if (r != CY_RSLT_SUCCESS) return r; // ±2 g

    return CY_RSLT_SUCCESS;
}


int main(void)
{
    cybsp_init();
    __enable_irq();

    // UART for prints (KitProg3)
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    // I2C init: use the BSP-defined pins (mapped in Device Configurator)
    cyhal_i2c_cfg_t cfg = {
        .is_slave = false,
        .address  = 0,
        .frequencyhal_hz = 400000
    };
    if (CY_RSLT_SUCCESS != cyhal_i2c_init(&i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL)) {
        printf("I2C init failed\r\n");
        for(;;);
    }
    cyhal_i2c_configure(&i2c, &cfg);

    // Probe WHO_AM_I
    uint8_t who = 0;
    if (CY_RSLT_SUCCESS != i2c_read(MPU6050_REG_WHO_AM_I, &who, 1)) {
        printf("WHO_AM_I read failed\r\n");
        for(;;);
    }
    printf("WHO_AM_I = 0x%02X (expect 0x68 or 0x69)\r\n", who);

    // Configure device
    if (CY_RSLT_SUCCESS != mpu6050_init()) {
        printf("MPU6050 init failed\r\n");
        for(;;);
    }
    printf("MPU6050 configured.\r\n");

    // Read 14 bytes: accel(6) + temp(2) + gyro(6)
    for (;;)
    {
    	//gravity/acceleration = A
    	//gyroscope angular rate (deg/s)= G
        uint8_t raw[14];
        if (CY_RSLT_SUCCESS == i2c_read(MPU6050_REG_ACCEL_XOUT, raw, sizeof raw)) {
            int16_t ax = (raw[0] << 8) | raw[1];
            int16_t ay = (raw[2] << 8) | raw[3];
            int16_t az = (raw[4] << 8) | raw[5];
            int16_t t  = (raw[6] << 8) | raw[7];
            int16_t gx = (raw[8] << 8) | raw[9];
            int16_t gy = (raw[10] << 8) | raw[11];
            int16_t gz = (raw[12] << 8) | raw[13];

            // Scale for FS=±2g and ±250 dps
            float ax_g = ax / 16384.0f;
            float ay_g = ay / 16384.0f;
            float az_g = az / 16384.0f;
            float gx_dps = gx / 131.0f;
            float gy_dps = gy / 131.0f;
            float gz_dps = gz / 131.0f;
            float temp_c = (t / 340.0f) + 36.53f;

            printf("A[g]=[%0.3f %0.3f %0.3f]  G[dps]=[%0.2f %0.2f %0.2f]  T=%0.2fC\r\n",
                   ax_g, ay_g, az_g, gx_dps, gy_dps, gz_dps, temp_c);
        }
        cyhal_system_delay_ms(100);
    }
}

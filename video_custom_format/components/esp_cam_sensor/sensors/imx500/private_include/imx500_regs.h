/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define IMX500_REG_DELAY            0xeeee
#define IMX500_REG_END              0xffff
#define IMX500_REG_SENSOR_ID_H      0x300a
#define IMX500_REG_SENSOR_ID_L      0x300b
#define IMX500_REG_SLEEP_MODE       0x0100
#define IMX500_REG_MIPI_CTRL00      0x4800
#define IMX500_REG_FRAME_OFF_NUMBER 0x4202
#define OV5640_REG_PAD_OUT          0x300d

#ifdef __cplusplus
}
#endif

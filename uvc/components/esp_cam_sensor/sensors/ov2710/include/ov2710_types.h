/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * OV2710 camera sensor register type definition.
 */
typedef struct {
    uint16_t reg;
    uint8_t val;
} ov2710_reginfo_t;

#ifdef __cplusplus
}
#endif

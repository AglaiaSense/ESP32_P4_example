/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#pragma once

#include "esp_cam_sensor_types.h"
#include "imx500_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Custom register list */

static imx500_reginfo_t imx501_2028x1520_crop_60fps[] = {
    // Power on sequence
    {0x0004, 0x00},
    {0x0007, 0x80},
    {0x0007, 0x00},
    {0x0005, 0x01},
    {0x0004, 0x01},
    {0x0004, 0x03},
    {0x0004, 0x07},
    {0x0005, 0x12},
    // {IMX501_TABLE_WAIT_MS, IMX501_WAIT_MS_STREAM},
    // common register
    {0xA700, 0x01},
    {0x0136, 0x1B},
    {0x0137, 0x00},
    {0x0305, 0x02},
    {0x0306, 0x00},
    {0x0307, 0x9B},
    {0x030D, 0x02},
    {0x030E, 0x00},
    {0x030F, 0x9B},
    {0x0340, 0x18},
    {0x0341, 0x2C},
    {0x0820, 0x10},
    {0x0821, 0x59},
    {0x0822, 0x00},
    {0x0823, 0x00},
    {0x3607, 0x01},
    {0x3E34, 0x00},
    {0x3E35, 0x01},
    {0x3E36, 0x01},
    {0x3E37, 0x00},
    {0x3E38, 0x01},
    {0x3E39, 0x01},
    {0x3E3A, 0x01},
    {0x3E3B, 0x00},
    {0x3F56, 0x00},
    {0x3F57, 0xCA},
    {0x4BC0, 0x16},
    {0x7BA8, 0x00},
    {0x7BA9, 0x00},
    {0x886B, 0x00},
    {0x0342, 0x23},
    {0x0343, 0x40},
    {0x0114, 0x01},
    {0x0202, 0x11},
    {0x0203, 0x94},
    {0x0204, 0x03},
    {0x0205, 0x56},
    {0xBCF1, 0x02},
    // H2V2 Binning
    {0x0342, 0x16}, // Line Length: 5762
    {0x0343, 0x28},
    {0x3F56, 0x00},
    {0x3F57, 0xB8},
    {0x0340, 0x39}, // Frame Length:14772
    {0x0341, 0xB4},
    {0x0900, 0x01},
    {0x0901, 0x22},
    {0x034C, 0x07},
    {0x034D, 0xEC},
    {0x034E, 0x05},
    {0x034F, 0xF0},
    {0x040C, 0x07},
    {0x040D, 0xEC},
    {0x040E, 0x05},
    {0x040F, 0xF0},
    {0xffff, 0x00}};

/* 如果你想使用底板的ISP，请提供ISP信息 */
static const esp_cam_sensor_isp_info_t imx500_fmt_isp_60_fps = {
    .isp_v1_info = {
        .version = SENSOR_ISP_INFO_VERSION_DEFAULT, // ISP信息版本  9398
        .pclk = 744000000,                          // 像素时钟频率
        .vts = 2028,                                // 垂直总尺寸
        .hts = 1520,                                // 水平总尺寸
        .bayer_type = ESP_CAM_SENSOR_BAYER_GBRG,    // Bayer格式类型
    }};

/* 提供初始化列表的描述。
 * 注意，格式的描述必须是`static const`类型 */
static const esp_cam_sensor_format_t custom_format_info = {
    .name = "MIPI_2lane_24Minput_RAW8_2028x1520_56fps",   // 格式名称
    .format = ESP_CAM_SENSOR_PIXFORMAT_RAW10,             // 像素格式
    .port = ESP_CAM_SENSOR_MIPI_CSI,                      // 传感器接口类型
    .xclk = 24000000,                                     // 外部时钟频率
    .width = 2028,                                        // 图像宽度
    .height = 1520,                                       // 图像高度
    .regs = imx501_2028x1520_crop_60fps,                  // 寄存器配置数组
    .regs_size = ARRAY_SIZE(imx501_2028x1520_crop_60fps), // 寄存器配置数组大小
    .fps = 56,                                            // 帧率
    .isp_info = &imx500_fmt_isp_60_fps,                   // ISP信息指针
    .mipi_info = {
        .mipi_clk = (100000000ULL * 4), // MIPI时钟频率
        .lane_num = 2,                  // MIPI通道数
        .line_sync_en = true,           // 行同步使能
    },
    .reserved = NULL, // 保留字段
};
#ifdef __cplusplus
}
#endif

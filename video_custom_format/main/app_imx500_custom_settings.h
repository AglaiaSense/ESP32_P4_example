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
 
 
 static imx500_reginfo_t imx501_1920x1080_crop_30fps[] = {
    // {0xffff, 0x00},

      // Power on sequence
#if 0
      {0x0004, 0x00},
      {0x0007, 0x80},
      {0x0007, 0x00},
      {0x0005, 0x01},
      {0x0004, 0x01},
      {0x0004, 0x03},
      {0x0004, 0x07},
      {0x0005, 0x12},
#endif
      // {IMX501_TABLE_WAIT_MS, IMX501_WAIT_MS_STREAM},
      // common register
      {0xA700, 0x01},
      {0x0136, 0x1B},
      {0x0137, 0x00},
      {0x0305, 0x02},  // IVT PREDIV 
      {0x0306, 0x00},  // IVT MPY
      {0x0307, 0x9B},  // IVT MPY  2092.5M
      {0x030D, 0x02},  // IOP PREDIV
      {0x030E, 0x00},  // IOP MPY
      {0x030F, 0x4E},  // IOP MPY  1053M
      {0x0340, 0x18},
      {0x0341, 0x2C},
      // {0x0820, 0x10},  // MIPI RATE
      // {0x0821, 0x59},  // MIPI RATE
      {0x0820, 0x08},  // MIPI RATE   27 / 2 * 0x4E * 2
      {0x0821, 0x3A},  // MIPI RATE
      {0x0822, 0x00},
      {0x0823, 0x00},
      {0x3607, 0x01},
#if 0
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
#endif
      //{0x0342, 0x23},
      //{0x0343, 0x40},
      {0x0112, 0x0A}, // 0x0A: 10bit, 0x08: 8bit
      {0x0113, 0x0A}, // 0x0A: 10bit, 0x08: 8bit
      {0x0202, 0x11},
      {0x0203, 0x94},
      {0x0204, 0x03},
      {0x0205, 0x56},
      {0xBCF1, 0x00},  // EBD size
      // H2V2 Binning
      {0x0342, 0x24}, // Line Length: 9400
      {0x0343, 0xB8},
      {0x3F56, 0x00},
      {0x3F57, 0xB8},
      {0x0340, 0x0B}, // Frame Length:2980
      {0x0341, 0xA4},
      {0x0900, 0x01},
      {0x0901, 0x22},
      {0x034C, 0x07}, // Width: 1920
      {0x034D, 0x80},
      {0x034E, 0x04}, // Height: 1080
      {0x034F, 0x38},
      {0x040C, 0x07}, // Width: 1920
      {0x040D, 0x80},
      {0x040E, 0x04}, // Height: 1080
      {0x034F, 0x38},
      {0xffff, 0x00}  // End of register list
 };
 
 /* 如果你想使用底板的ISP，请提供ISP信息 */
 static const esp_cam_sensor_isp_info_t imx500_fmt_isp_60_fps = {
     .isp_v1_info = {
         .version = SENSOR_ISP_INFO_VERSION_DEFAULT, // ISP信息版本  9398
         .pclk = 210600000,                          // 像素时钟频率
         .vts = 2980,                                // 垂直总尺寸
         .hts = 4730,                                // 水平总尺寸
         .bayer_type = ESP_CAM_SENSOR_BAYER_GBRG,    // Bayer格式类型
     }};
 
 /* 提供初始化列表的描述。
  * 注意，格式的描述必须是`static const`类型 */
 static const esp_cam_sensor_format_t custom_format_info = {
     .name = "MIPI_2lane_24Minput_RAW8_1920x1080_30fps",   // 格式名称
     .format = ESP_CAM_SENSOR_PIXFORMAT_RAW10,             // 像素格式
     .port = ESP_CAM_SENSOR_MIPI_CSI,                      // 传感器接口类型
     .xclk = 27000000,                                     // 外部时钟频率
     .width = 1920,                                        // 图像宽度
     .height = 1080,                                       // 图像高度
     .regs = imx501_1920x1080_crop_30fps,                  // 寄存器配置数组
     .regs_size = ARRAY_SIZE(imx501_1920x1080_crop_30fps), // 寄存器配置数组大小
     .fps = 30,                                            // 帧率
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
 
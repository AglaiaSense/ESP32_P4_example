/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: ESPRESSIF MIT
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_video_init.h"
#include "esp_video_ioctl.h"
#include "linux/videodev2.h"
#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>

// #include "app_sc2336_custom_settings.h"
#include "app_ov5647_custom_settings.h"
// #include "app_imx500_custom_settings.h"

#define MEMORY_TYPE V4L2_MEMORY_MMAP
#define BUFFER_COUNT 2
#define CAPTURE_SECONDS 3

static const char *TAG = "example";

static const esp_video_init_csi_config_t csi_config[] = {
    {
        .sccb_config = {
            .init_sccb = true,
            .i2c_config = {
                .port = CONFIG_EXAMPLE_MIPI_CSI_SCCB_I2C_PORT,
                .scl_pin = CONFIG_EXAMPLE_MIPI_CSI_SCCB_I2C_SCL_PIN,
                .sda_pin = CONFIG_EXAMPLE_MIPI_CSI_SCCB_I2C_SDA_PIN,
            },
            .freq = CONFIG_EXAMPLE_MIPI_CSI_SCCB_I2C_FREQ,
        },
        .reset_pin = CONFIG_EXAMPLE_MIPI_CSI_CAM_SENSOR_RESET_PIN,
        .pwdn_pin = CONFIG_EXAMPLE_MIPI_CSI_CAM_SENSOR_PWDN_PIN,
    },
};

static const esp_video_init_config_t cam_config = {

    .csi = csi_config,

};

static esp_err_t camera_capture_stream(void) {
    int fd;
    esp_err_t ret;
    int fmt_index = 0;
    uint32_t frame_size;
    uint32_t frame_count;
    struct v4l2_buffer buf;
    uint8_t *buffer[BUFFER_COUNT];
    struct v4l2_format init_format;
    struct v4l2_requestbuffers req;
    struct v4l2_capability capability;

    const int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("%s(%d)\n", __func__, __LINE__);

    // 打开视频设备
    fd = open("/dev/video0", O_RDONLY);
    if (fd < 0) {
        ESP_LOGI(TAG, "failed to open device");
        return ESP_FAIL;
    }
    printf("%s(%d)\n", __func__, __LINE__);

    // 查询设备能力
    if (ioctl(fd, VIDIOC_QUERYCAP, &capability)) {
        ESP_LOGI(TAG, "failed to get capability");
        ret = ESP_FAIL;
        goto exit_0;
    }
    printf("%s(%d)\n", __func__, __LINE__);

    // 打印设备能力信息
    ESP_LOGI(TAG, "version: %d.%d.%d", (uint16_t)(capability.version >> 16),
             (uint8_t)(capability.version >> 8),
             (uint8_t)capability.version);
    ESP_LOGI(TAG, "driver:  %s", capability.driver);
    ESP_LOGI(TAG, "card:    %s", capability.card);
    ESP_LOGI(TAG, "bus:     %s", capability.bus_info);
    ESP_LOGI(TAG, "capabilities:");

    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
    }
    if (capability.capabilities & V4L2_CAP_READWRITE) {
        ESP_LOGI(TAG, "\tREADWRITE");
    }
    if (capability.capabilities & V4L2_CAP_ASYNCIO) {
        ESP_LOGI(TAG, "\tASYNCIO");
    }
    if (capability.capabilities & V4L2_CAP_STREAMING) {
        ESP_LOGI(TAG, "\tSTREAMING");
    }
    if (capability.capabilities & V4L2_CAP_META_OUTPUT) {
        ESP_LOGI(TAG, "\tMETA_OUTPUT");
    }
    if (capability.capabilities & V4L2_CAP_DEVICE_CAPS) {
        ESP_LOGI(TAG, "device capabilities:");
        if (capability.device_caps & V4L2_CAP_VIDEO_CAPTURE) {
            ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
        }
        if (capability.device_caps & V4L2_CAP_READWRITE) {
            ESP_LOGI(TAG, "\tREADWRITE");
        }
        if (capability.device_caps & V4L2_CAP_ASYNCIO) {
            ESP_LOGI(TAG, "\tASYNCIO");
        }
        if (capability.device_caps & V4L2_CAP_STREAMING) {
            ESP_LOGI(TAG, "\tSTREAMING");
        }
        if (capability.device_caps & V4L2_CAP_META_OUTPUT) {
            ESP_LOGI(TAG, "\tMETA_OUTPUT");
        }
    }

    printf("-----------------custom_format_info-----------------\n");

    // 设置自定义寄存器配置
    if (ioctl(fd, VIDIOC_S_SENSOR_FMT, &custom_format_info) != 0) {
        ret = ESP_FAIL;
        printf("%s(%d)\n", __func__, __LINE__);

        goto exit_0;
    }

    // 获取视频格式
    memset(&init_format, 0, sizeof(struct v4l2_format));
    init_format.type = type;
    if (ioctl(fd, VIDIOC_G_FMT, &init_format) != 0) {
        ESP_LOGI(TAG, "failed to get format");
        ret = ESP_FAIL;
        printf("%s(%d)\n", __func__, __LINE__);

        goto exit_0;
    }

    ESP_LOGI(TAG, "\twidth:  %" PRIu32, init_format.fmt.pix.width);
    ESP_LOGI(TAG, "\theight: %" PRIu32, init_format.fmt.pix.height);

    struct v4l2_fmtdesc fmtdesc = {
        .index = fmt_index++,
        .type = type,
    };

    ESP_LOGI(TAG, "Capture %s format frames for %d seconds:", (char *)fmtdesc.description, CAPTURE_SECONDS);
    printf("%s(%d)\n", __func__, __LINE__);

    printf("-----------------size-----------------\n");

    // 请求缓冲区
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = type;
    req.memory = MEMORY_TYPE;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) != 0) {
        ESP_LOGI(TAG, "failed to require buffer");
        ret = ESP_FAIL;
        printf("%s(%d)\n", __func__, __LINE__);

        goto exit_0;
    }

    // 映射缓冲区并将其加入队列
    for (int i = 0; i < BUFFER_COUNT; i++) {
        struct v4l2_buffer buf;
        printf("%s(%d)\n", __func__, __LINE__);

        memset(&buf, 0, sizeof(buf));
        buf.type = type;
        buf.memory = MEMORY_TYPE;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) != 0) {
            ESP_LOGI(TAG, "failed to query buffer");
            ret = ESP_FAIL;
            goto exit_0;
        }

        buffer[i] = (uint8_t *)mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fd, buf.m.offset);
        if (!buffer[i]) {
            ESP_LOGI(TAG, "failed to map buffer");
            ret = ESP_FAIL;

            goto exit_0;
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf) != 0) {
            ESP_LOGI(TAG, "failed to queue video frame");
            ret = ESP_FAIL;
            goto exit_0;
        }
    }

    // 开始视频流
    if (ioctl(fd, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGI(TAG, "failed to start stream");
        ret = ESP_FAIL;
        goto exit_0;
    }

    printf("%s(%d)\n", __func__, __LINE__);

    // 捕获视频帧
    frame_count = 0;
    frame_size = 0;
    int64_t start_time_us = esp_timer_get_time();
    while (esp_timer_get_time() - start_time_us < (CAPTURE_SECONDS * 1000 * 1000)) {

        memset(&buf, 0, sizeof(buf));

        buf.type = type;
        buf.memory = MEMORY_TYPE;

        // 从视频设备中取出一个已填充的视频缓冲区
        if (ioctl(fd, VIDIOC_DQBUF, &buf) != 0) {
            ESP_LOGI(TAG, "failed to receive video frame");
            ret = ESP_FAIL;
            goto exit_0;
        }

        frame_size += buf.bytesused;
        // 将处理完的缓冲区重新排队
        if (ioctl(fd, VIDIOC_QBUF, &buf) != 0) {
            ESP_LOGI(TAG, "failed to queue video frame");
            ret = ESP_FAIL;
            goto exit_0;
        }

        // printf("frame_size: %" PRIu32 "\n", frame_size);

        printf("frame_count:  %" PRIu32 "\n", frame_count);

        frame_count++;
    }

    // 停止视频流
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) != 0) {
        ESP_LOGI(TAG, "failed to stop stream");
        ret = ESP_FAIL;
        goto exit_0;
    }
    printf("%s(%d)\n", __func__, __LINE__);

    // 打印捕获的帧信息
    ESP_LOGI(TAG, "\twidth:  %" PRIu32, init_format.fmt.pix.width);
    ESP_LOGI(TAG, "\theight: %" PRIu32, init_format.fmt.pix.height);
    ESP_LOGI(TAG, "\tsize:   %" PRIu32, frame_size / frame_count);
    ESP_LOGI(TAG, "\tFPS:    %" PRIu32, frame_count / CAPTURE_SECONDS);

    ret = ESP_OK;

exit_0:
    printf("%s(%d)\n", __func__, __LINE__);

    close(fd);
    return ret;
}

void app_main(void) {
    esp_err_t ret = ESP_OK;

    // 初始化摄像头
    printf("-----------------esp_video_init-----------------\n");
    ret = esp_video_init(&cam_config);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Camera init failed with error 0x%x", ret);
        return;
    }
    printf("-----------------camera_capture_stream-----------------\n");

    // 开始摄像头捕获流
    ret = camera_capture_stream();
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Camera capture stream failed with error 0x%x", ret);
        return;
    }
}
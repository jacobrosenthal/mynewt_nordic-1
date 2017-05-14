/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include <hal/hal_bsp.h>
#include <assert.h>
#include <os/os.h>
#include <bsp/cmsis_nvic.h>

/* Mynewt PWM driver */
#include <pwm/pwm.h>

/* Mynewt Nordic driver */
#include "pwm_nrf51/pwm_nrf51.h"

/* Nordic headers */
#include "nrf.h"
#include "app_util_platform.h"
#include "nrf_drv_pwm.h"
#include "nrf_pwm.h"






static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
// static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);
// static nrf_drv_pwm_t m_pwm2 = NRF_DRV_PWM_INSTANCE(2);

static struct pwm_dev *global_pwm_dev;

static nrf_drv_pwm_config_t *global_pwm_config;
static struct nrf51_pwm_dev_cfg *init_pwm_config;




/**
 * Open the NRF51 PWM device
 *
 * This function locks the device for access from other tasks.
 *
 * @param odev The OS device to open
 * @param wait The time in MS to wait.  If 0 specified, returns immediately
 *             if resource unavailable.  If OS_WAIT_FOREVER specified, blocks
 *             until resource is available.
 * @param arg  Argument provided by higher layer to open, in this case
 *             it can be a nrf_drv_pwm_config_t, to override the default
 *             configuration.
 *
 * @return 0 on success, non-zero on failure.
 */
static int
nrf51_pwm_open(struct os_dev *odev, uint32_t wait, void *arg)
{
    struct pwm_dev *dev;
    nrf_drv_pwm_config_t *cfg;
    int rc;

    dev = (struct pwm_dev *) odev;

    if (os_started()) {
        rc = os_mutex_pend(&dev->pwm_lock, wait);
        if (rc != OS_OK) {
            goto err;
        }
    }

    /* Initialize the device */
    cfg = (nrf_drv_pwm_config_t *)arg;
    rc = nrf_drv_pwm_init(&m_pwm0, cfg, NULL);
    if (rc != 0) {
        goto err;
    }

    global_pwm_dev = dev;
    global_pwm_config = arg;

    return (0);
err:
    return (rc);
}


/**
 * Close the NRF51 PWM device.
 *
 * This function unlocks the device.
 *
 * @param odev The device to close.
 */
static int
nrf51_pwm_close(struct os_dev *odev)
{
    struct pwm_dev *dev;

    dev = (struct pwm_dev *) odev;

    nrf_drv_pwm_uninit(&m_pwm0);

    global_pwm_dev = NULL;
    global_pwm_config = NULL;

    if (os_started()) {
        os_mutex_release(&dev->pwm_lock);
    }

    return (0);
}


/**
 * Configure an PWM channel on the Nordic PWM.
 *
 * @param dev The PWM device to configure
 * @param cnum The channel on the PWM device to configure
 * @param cfgdata An opaque pointer to channel config, expected to be
 *                a nrf_drv_adc_channel_config_t
 *
 * @return 0 on success, non-zero on failure.
 */
static int
nrf51_pwm_configure_channel(struct pwm_dev *dev, uint8_t cnum,
                            void *cfgdata)
{


    return (0);
// err:
//     return (rc);
}

/**
 * Callback to initialize an pwm_dev structure from the os device
 * initialization callback.  This sets up a nrf51_pwm_device(), so that
 * subsequent lookups to this device allow us to manipulate it.
 */
int
nrf51_pwm_dev_init(struct os_dev *odev, void *arg)
{
    struct pwm_dev *dev;
    struct pwm_driver_funcs *af;

    dev = (struct pwm_dev *) odev;

    os_mutex_init(&dev->pwm_lock);

    dev->pwm_chan_count = NRF_PWM_CHANNEL_COUNT;

    OS_DEV_SETHANDLERS(odev, nrf51_pwm_open, nrf51_pwm_close);

    assert(init_pwm_config == NULL || init_pwm_config == arg);
    init_pwm_config = arg;

    af = &dev->pwm_funcs;

    af->pwm_configure_channel = nrf51_pwm_configure_channel;

    return (0);
}



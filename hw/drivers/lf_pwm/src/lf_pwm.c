
#include <hal/hal_bsp.h>
#include <assert.h>
#include <os/os.h>
#include <bsp/cmsis_nvic.h>

/* Nordic headers */
#include <nrf.h>
#include <app_timer.h>
#include <nrf_drv_clock.h>
#include <app_util_platform.h>
#include <low_power_pwm.h>

#include "lf_pwm/lf_pwm.h"

/*Timer initialization parameters*/   
#define OP_QUEUES_SIZE          3
#define APP_TIMER_PRESCALER     0 

/*Ticks before change duty cycle of each LED*/
#define TICKS_BEFORE_CHANGE_0   500
#define TICKS_BEFORE_CHANGE_1   400

static low_power_pwm_t low_power_pwm_0;
static low_power_pwm_t low_power_pwm_1;
static low_power_pwm_t low_power_pwm_2;


#define LED_1          	21
#define LED_2          	25
#define LED_3          	23
#define LED_4          	19
#define LED_5          	18
#define LED_6          	12
#define LED_7          	10
#define LED_8          	9
#define LED_9          	30
#define LED_10         	28
#define LED_11         	29
#define LED_12         	22

#define BSP_LED_1_MASK    (1<<LED_1)
#define BSP_LED_2_MASK    (1<<LED_2)
#define BSP_LED_3_MASK    (1<<LED_3)
#define BSP_LED_4_MASK    (1<<LED_4)
#define BSP_LED_5_MASK    (1<<LED_5)
#define BSP_LED_6_MASK    (1<<LED_6)
#define BSP_LED_7_MASK    (1<<LED_7)
#define BSP_LED_8_MASK    (1<<LED_8)
#define BSP_LED_9_MASK    (1<<LED_9)
#define BSP_LED_10_MASK   (1<<LED_10)
#define BSP_LED_11_MASK   (1<<LED_11)
#define BSP_LED_12_MASK   (1<<LED_12)


#define ALL_MASK      (BSP_LED_1_MASK | BSP_LED_2_MASK | BSP_LED_3_MASK |
						BSP_LED_4_MASK | BSP_LED_5_MASK | BSP_LED_6_MASK |
						BSP_LED_7_MASK | BSP_LED_8_MASK | BSP_LED_9_MASK |
						BSP_LED_10_MASK | BSP_LED_11_MASK | BSP_LED_12_MASK)

#define ODD_MASK      (BSP_LED_1_MASK  | BSP_LED_3_MASK  BSP_LED_5_MASK |
						BSP_LED_7_MASK |  BSP_LED_9_MASK | BSP_LED_11_MASK)

#define EVEN_MASK      (BSP_LED_2_MASK  | BSP_LED_4_MASK  BSP_LED_6_MASK |
						BSP_LED_8_MASK |  BSP_LED_10_MASK | BSP_LED_12_MASK)

/**
 * @brief Function to be called in timer interrupt.
 *
 * @param[in] p_context     General purpose pointer (unused).
 */
void pwm_handler(void * p_context)
{
    uint8_t new_duty_cycle;
    static uint16_t led_0, led_1;
    uint32_t err_code;
    UNUSED_PARAMETER(p_context);
    
    low_power_pwm_t * pwm_instance = (low_power_pwm_t*)p_context;
    
    if(pwm_instance->bit_mask == ODD_MASK)
    {
        led_0++;
        
        if(led_0 > TICKS_BEFORE_CHANGE_0)
        {
            new_duty_cycle = pwm_instance->period - pwm_instance->duty_cycle;
            err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
            led_0 = 0;
            APP_ERROR_CHECK(err_code);
        }
    }
    else if(pwm_instance->bit_mask == EVEN_MASK)
    {
        led_1++;

        if(led_1 > TICKS_BEFORE_CHANGE_1)
        {
            new_duty_cycle = pwm_instance->period - pwm_instance->duty_cycle;
            err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
            led_1 = 0;
            APP_ERROR_CHECK(err_code);
        }
    }
    else
    {
        /*empty else*/
    }
}

/**
 * @brief Function to initalize low_power_pwm instances.
 *
 */

void low_power_init(void)
{
    uint32_t err_code;
    low_power_pwm_config_t low_power_pwm_config;
	
    APP_TIMER_DEF(lpp_timer_0);
    low_power_pwm_config.active_high = false;
    low_power_pwm_config.period = 220;
    low_power_pwm_config.bit_mask = EVEN_MASK;
    low_power_pwm_config.p_timer_id = &lpp_timer_0;
    
    err_code = low_power_pwm_init((&low_power_pwm_0), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_0, 127);
    APP_ERROR_CHECK(err_code);
    
    // APP_TIMER_DEF(lpp_timer_1);
    // low_power_pwm_config.active_high = false;
    // low_power_pwm_config.period = 200;
    // low_power_pwm_config.bit_mask = EVEN_MASK;
    // low_power_pwm_config.p_timer_id = &lpp_timer_1;
    
    // err_code = low_power_pwm_init((&low_power_pwm_1), &low_power_pwm_config, pwm_handler);
    // APP_ERROR_CHECK(err_code);
    // err_code = low_power_pwm_duty_set(&low_power_pwm_1, 150);
    // APP_ERROR_CHECK(err_code);
    
    // APP_TIMER_DEF(lpp_timer_2);
    // low_power_pwm_config.active_high = false;
    // low_power_pwm_config.period = 100;
    // low_power_pwm_config.bit_mask = ALL_MASK;
    // low_power_pwm_config.p_timer_id = &lpp_timer_2;
    
    // err_code = low_power_pwm_init((&low_power_pwm_2), &low_power_pwm_config, pwm_handler);
    // APP_ERROR_CHECK(err_code);
    // err_code = low_power_pwm_duty_set(&low_power_pwm_2, 20);
    // APP_ERROR_CHECK(err_code);
    
    err_code = low_power_pwm_start((&low_power_pwm_0), low_power_pwm_0.bit_mask);
    APP_ERROR_CHECK(err_code);
    // err_code = low_power_pwm_start((&low_power_pwm_1), low_power_pwm_1.bit_mask);
    // APP_ERROR_CHECK(err_code);
    // err_code = low_power_pwm_start((&low_power_pwm_2), low_power_pwm_2.bit_mask);
    // APP_ERROR_CHECK(err_code);
}

static void lfclk_init(void)
{
    uint32_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/**
 * @brief Function for application main entry.
 */
int lf_pwm_init(void)
{
    uint8_t new_duty_cycle;
    uint32_t err_code;

    lfclk_init();

    // Start APP_TIMER to generate timeouts.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, OP_QUEUES_SIZE, NULL);

    /*Initialize low power PWM for all 3  channels of RGB or 3 channels of leds on pca10028*/
    low_power_init();
    
    /* Duty cycle can also be changed from main context. */
    // new_duty_cycle = 127;
    // err_code = low_power_pwm_duty_set(&low_power_pwm_2, new_duty_cycle);
    // APP_ERROR_CHECK(err_code);
}
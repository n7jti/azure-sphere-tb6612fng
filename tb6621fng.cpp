#include "tb6621fng.h"

#include <applibs/gpio.h>

int tb6621fng::init(int pwm_fd, PWMChannelId pwm_ch, int in1_gpio_fd, int in2_gpio_fd)
{
    _fdPwm = pwm_fd;
    _pwm_ch = pwm_ch;
    _fdIn1 = in1_gpio_fd;
    _fdIn2 = in2_gpio_fd;
    _speed = 0; 

    PwmState pwmState;
    pwmSttate.period_nsec = 20000;
    pwmState.dutyCycle_nsec = 0; 
    pwmState.pwmPolarity = PWM_Polarity_Normal;
    pwmState.enabled = false;

    int ret = PWM_Apply(_fdPwm, _pwm_ch, &pwmState);

    return ret;
}

int tb6621fng::move(int32_t speed)
{
    // speed it 255..0 CW
    //          0..-255 CCW
    // 255 is max speed, 0 is stop

    // get the magnitude
    uint32_t magnitude = 0;
    if (speed < 0) {
        magnitude =  -speed 
    }
    else {
        magnitude = speed;
    }

    // clamp at full speed
    if (magnitude > 255){
        magnitude = 255; 
    }

    unsigned int dutyCycle = 20000 / 255 * magnitude; 
    int ret = 0;

    PwmState pwmState;
    pwmState.period_nsec = 20000;
    pwmState.dutyCycle_nsec = dutyCycle; 
    pwmState.pwmPolarity = PWM_Polarity_Normal;
    pwmState.enabled = false;

    int ret = PWM_Apply(_fdPwm, _pwm_ch, &pwmState);

    if (ret == 0){
        if (speed >= 0){
            // CW is HIGH LOW
            ret = GPIO_SetValue(_fdIn1, GPIO_Value_High);

            if (ret == 0) {
                GPIO_SetValue(_fdIn2, GPIO_Value_Low; 
            }
        }
        else {
            // CCW is LOW HIGH
            ret = GPIO_SetValue(_fdIn1, GPIO_Value_Low);

            if (ret == 0) {
                GPIO_SetValue(_fdIn2, GPIO_Value_High; 
            }
        }
    }

    return ret;
}

int tb6621fng::apply_break()
{
    int ret = 0; 
    // IN1 high, IN2 high
    ret = GPIO_SetValue(_fdIn1, GPIO_Value_High);

    if (ret == 0) {
        GPIO_SetValue(_fdIn2, GPIO_Value_High); 
    }

    return ret;
}

int tb6621fng::stop()
{
    int ret = 0; 
    // IN1 low, IN2 low
    ret = GPIO_SetValue(_fdIn1, GPIO_Value_Low);

    if (ret == 0) {
        GPIO_SetValue(_fdIn2, GPIO_Value_Low); 
    }

}
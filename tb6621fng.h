#pragma once

#include <applibs/pwm.h>

class tb6621fng
{
public: 
    int init(int pwm_fd, PWMChannelId pwm_ch, int in1_gpio_fd, int in2_gpio_fd);
    int move(int32_t speed); //-255 to +255 clamping when out of range, 0 is "stop" in1 and in1 based on sign, 
    int apply_break(); //in1 and in2 both high
    int stop(); //in1 and in2 both low

private:
    int _fdPwm = -1;
    PWMChannelId _pwm_ch=0; 
    int _fdIn1=-1;
    int _fdIn2=-1;
    int32 _speed=0;
};
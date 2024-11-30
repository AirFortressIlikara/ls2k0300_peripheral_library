#include "pwm_gtim.h"
#include "iostream"
int main()
{
    int gpio, ch, per, duty;
    std::cin >> gpio >> ch >> per >> duty;
    PWM_GTIM test(gpio, 0b11, ch, per, duty);
    test.enable();
    while (1)
    {
        std::cin >> per >> duty;
        test.setPeriod(per);
        test.setDutyCycle(duty);
    }
    return 0;
}
#include "LED.hpp"
void LED::On(Color c)
{
    switch(c)
    {
        case RED:
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET);
            break;
        case GREEN:
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);
            break;
        case BLUE:
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}

void LED::Off(Color c)
{
    switch(c)
    {
        case RED:
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);
            break;
        case GREEN:
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);
            break;
        case BLUE:
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void LED::Toggle(Color c)
{
    switch(c)
    {
        case RED:
            HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_12);
            break;
        case GREEN:
            HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_11);
            break;
        case BLUE:
            HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_10);
            break;
        default:
            break;
    }
}

void LED::OffAll()
{
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);   
}
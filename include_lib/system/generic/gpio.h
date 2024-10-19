#ifndef ASM_GPIO_H
#define ASM_GPIO_H


#include "asm/gpio.h"


int gpio_direction_input(unsigned int gpio);
/* 
 * 将指定的GPIO引脚配置为输入模式。
 * 参数:
 *   gpio - 需要配置为输入模式的GPIO引脚编号。
 * 返回值:
 *   成功返回0，失败返回负值。
 */

int gpio_direction_output(unsigned int gpio, int value);
/* 
 * 将指定的GPIO引脚配置为输出模式，并设置初始输出电平。
 * 参数:
 *   gpio  - 需要配置为输出模式的GPIO引脚编号。
 *   value - 输出的初始电平值（0或1）。
 * 返回值:
 *   成功返回0，失败返回负值。
 */

int gpio_set_pull_up(unsigned int gpio, int value);
/* 
 * 为指定的GPIO引脚配置上拉电阻。
 * 参数:
 *   gpio  - 需要配置上拉电阻的GPIO引脚编号。
 *   value - 上拉电阻的设置（0表示禁用，1表示启用）。
 * 返回值:
 *   成功返回0，失败返回负值。
 */

int gpio_set_pull_down(unsigned int gpio, int value);
/* 
 * 为指定的GPIO引脚配置下拉电阻。
 * 参数:
 *   gpio  - 需要配置下拉电阻的GPIO引脚编号。
 *   value - 下拉电阻的设置（0表示禁用，1表示启用）。
 * 返回值:
 *   成功返回0，失败返回负值。
 */

int gpio_set_hd(unsigned int gpio, int value);
/* 
 * 配置指定GPIO引脚的高驱动能力。
 * 参数:
 *   gpio  - 需要配置高驱动能力的GPIO引脚编号。
 *   value - 高驱动能力的设置（0表示禁用，1表示启用）。
 * 返回值:
 *   成功返回0，失败返回负值。
 */

int gpio_set_die(unsigned int gpio, int value);
/* 
 * 配置指定GPIO引脚的输入特性。
 * 参数:
 *   gpio  - 需要配置输入特性的GPIO引脚编号。
 *   value - 输入特性的设置（0表示禁用，1表示启用）。
 * 返回值:
 *   成功返回0，失败返回负值。
 */

int gpio_set_output_clk(unsigned int gpio, int clk);
/* 
 * 为指定的GPIO引脚配置输出时钟信号。
 * 参数:
 *   gpio - 需要配置输出时钟的GPIO引脚编号。
 *   clk  - 时钟信号的频率值。
 * 返回值:
 *   成功返回0，失败返回负值。
 */




int gpio_read(unsigned int gpio);








#endif

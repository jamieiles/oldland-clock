#include <stdio.h>

#include "gpio_defines.h"
#include "irq_defines.h"
#include "timer_defines.h"

#define BANK(n) ((n) * 0x10)

static struct clock_time {
	unsigned int hours;
	unsigned int minutes;
	unsigned int seconds;
} time;

extern void init_vectors(void);

static const unsigned char hex_to_seg(unsigned char v)
{
	static const unsigned char lut[16] = {
		[0x0] = 0b0111111,
		[0x1] = 0b0000110,
		[0x2] = 0b1011011,
		[0x3] = 0b1001111,
		[0x4] = 0b1100110,
		[0x5] = 0b1101101,
		[0x6] = 0b1111101,
		[0x7] = 0b0100111,
		[0x8] = 0b1111111,
		[0x9] = 0b1100111,
		[0xa] = 0b1110111,
		[0xb] = 0b1111100,
		[0xc] = 0b0111001,
		[0xd] = 0b1011110,
		[0xe] = 0b1111001,
		[0xf] = 0b1110001
	};

	return lut[v & 0xf];
}

static inline void writel(unsigned long v, unsigned long addr)
{
	volatile unsigned long *p = (volatile unsigned long *)addr;

	*p = v;
}

static inline unsigned long readl(unsigned long addr)
{
	volatile unsigned long *p = (volatile unsigned long *)addr;

	return *p;
}

static void init_gpio(void)
{
	writel(0x7f7f7f7f, GPIO_ADDRESS + BANK(0) + GPIO_OUTPUT_ENABLE_REG_OFFS);
	writel(0x00007f7f, GPIO_ADDRESS + BANK(1) + GPIO_OUTPUT_ENABLE_REG_OFFS);
	writel(0x7f7f7f7f, GPIO_ADDRESS + BANK(0) + GPIO_SET_REG_OFFS);
	writel(0x00007f7f, GPIO_ADDRESS + BANK(1) + GPIO_SET_REG_OFFS);
}

static void output_time(const struct clock_time *time)
{
	writel(0x7f7f7f7f, GPIO_ADDRESS + BANK(0) + GPIO_SET_REG_OFFS);
	writel(0x00007f7f, GPIO_ADDRESS + BANK(1) + GPIO_SET_REG_OFFS);

	writel(hex_to_seg(time->seconds % 10) << 0,
	       GPIO_ADDRESS + BANK(0) + GPIO_CLEAR_REG_OFFS);
	writel(hex_to_seg(time->seconds / 10) << 8,
	       GPIO_ADDRESS + BANK(0) + GPIO_CLEAR_REG_OFFS);
	writel(hex_to_seg(time->minutes % 10) << 16,
	       GPIO_ADDRESS + BANK(0) + GPIO_CLEAR_REG_OFFS);
	writel(hex_to_seg(time->minutes / 10) << 24,
	       GPIO_ADDRESS + BANK(0) + GPIO_CLEAR_REG_OFFS);
	writel(hex_to_seg(time->hours % 10) << 0,
	       GPIO_ADDRESS + BANK(1) + GPIO_CLEAR_REG_OFFS);
	writel(hex_to_seg(time->hours / 10) << 8,
	       GPIO_ADDRESS + BANK(1) + GPIO_CLEAR_REG_OFFS);
}

void handle_irq(void)
{
	writel(~0, TIMER_ADDRESS + TIMER_EOI_REG_OFFS);

	time.seconds++;
	if (time.seconds == 60) {
		time.minutes++;
		time.seconds = 0;
	}
	if (time.minutes == 60) {
		time.hours++;
		time.minutes = 0;
	}
	if (time.hours == 24)
		time.hours = 0;
}

void bad_vector(void)
{
	printf("bad vector\n");
}

static void init_timer(void)
{
	writel(50000000, TIMER_ADDRESS + TIMER_RELOAD_REG_OFFS);
	writel(TIMER_ENABLED_MASK | TIMER_IRQ_ENABLE_MASK | TIMER_PERIODIC_MASK,
	       TIMER_ADDRESS + TIMER_CONTROL_REG_OFFS);
	writel(1 << 0, IRQ_ADDRESS + IRQ_CTRL_ENABLE_REG_OFFS);
}

int main(void)
{
	init_vectors();
	init_gpio();
	init_timer();

	scanf("%u:%u:%u", &time.hours, &time.minutes, &time.seconds);

	for (;;) {
		asm volatile("" ::: "memory");
		output_time(&time);
	}
}

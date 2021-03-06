/*
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/keyreset.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c/atmel_mxt_ts.h>
#include <linux/platform_data/mms_ts.h>
#include <asm/mach-types.h>
#include <plat/omap4-keypad.h>

#include "board-tuna.h"
#include "mux.h"

#define GPIO_TOUCH_EN		19
#define GPIO_TOUCH_IRQ		46

/* touch is on i2c3 */
#define GPIO_TOUCH_SCL	130
#define GPIO_TOUCH_SDA	131

static const int tuna_keymap[] = {
	KEY(1, 1, KEY_VOLUMEDOWN),
	KEY(2, 1, KEY_VOLUMEUP),
};

static struct omap_device_pad keypad_pads[] = {
	{	.name   = "kpd_col1.kpd_col1",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "kpd_col1.kpd_col1",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "kpd_col2.kpd_col2",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "kpd_col3.kpd_col3",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "kpd_col4.kpd_col4",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "kpd_col5.kpd_col5",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "gpmc_a23.kpd_col7",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "gpmc_a22.kpd_col6",
		.enable = OMAP_WAKEUP_EN | OMAP_MUX_MODE1,
	},
	{	.name   = "kpd_row0.kpd_row0",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "kpd_row1.kpd_row1",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "kpd_row2.kpd_row2",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "kpd_row3.kpd_row3",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "kpd_row4.kpd_row4",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "kpd_row5.kpd_row5",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "gpmc_a18.kpd_row6",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
	{	.name   = "gpmc_a19.kpd_row7",
		.enable = OMAP_PULL_ENA | OMAP_PULL_UP | OMAP_WAKEUP_EN |
			OMAP_MUX_MODE1 | OMAP_INPUT_EN,
	},
};

static struct matrix_keymap_data tuna_keymap_data = {
	.keymap			= tuna_keymap,
	.keymap_size		= ARRAY_SIZE(tuna_keymap),
};

static struct omap4_keypad_platform_data tuna_keypad_data = {
	.keymap_data		= &tuna_keymap_data,
	.rows			= 3,
	.cols			= 2,
};

static struct omap_board_data keypad_data = {
	.id	    		= 1,
	.pads	 		= keypad_pads,
	.pads_cnt       	= ARRAY_SIZE(keypad_pads),
};

static struct gpio_event_direct_entry tuna_gpio_keypad_keys_map_high[] = {
	{
		.code	= KEY_POWER,
		.gpio	= 3,
	},
};

static struct gpio_event_input_info tuna_gpio_keypad_keys_info_high = {
	.info.func = gpio_event_input_func,
	.type = EV_KEY,
	.keymap = tuna_gpio_keypad_keys_map_high,
	.keymap_size = ARRAY_SIZE(tuna_gpio_keypad_keys_map_high),
	.flags = GPIOEDF_ACTIVE_HIGH,
};

static struct gpio_event_direct_entry tuna_gpio_keypad_keys_map_low[] = {
	{
		.code	= KEY_VOLUMEDOWN,
		.gpio	= 8,
	},
	{
		.code	= KEY_VOLUMEUP,
		.gpio	= 30,
	},
};

static struct gpio_event_input_info tuna_gpio_keypad_keys_info_low = {
	.info.func = gpio_event_input_func,
	.type = EV_KEY,
	.keymap = tuna_gpio_keypad_keys_map_low,
	.keymap_size = ARRAY_SIZE(tuna_gpio_keypad_keys_map_low),
};

static struct gpio_event_info *tuna_gpio_keypad_info[] = {
	&tuna_gpio_keypad_keys_info_high.info,
	&tuna_gpio_keypad_keys_info_low.info,
};

static struct gpio_event_platform_data tuna_gpio_keypad_data = {
	.name = "tuna-gpio-keypad",
	.info = tuna_gpio_keypad_info,
	.info_count = ARRAY_SIZE(tuna_gpio_keypad_info)
};

static struct platform_device tuna_gpio_keypad_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data	= &tuna_gpio_keypad_data,
	},
};

static struct mxt_platform_data atmel_mxt_ts_pdata = {
	.x_line		= 19,
	.y_line		= 11,
	.x_size		= 1024,
	.y_size		= 1024,
	.blen		= 0x21,
	.threshold	= 0x28,
	.voltage	= 2800000,              /* 2.8V */
	.orient		= MXT_DIAGONAL,
	.irqflags	= IRQF_TRIGGER_FALLING,
};

static struct i2c_board_info __initdata tuna_i2c3_boardinfo_pre_lunchbox[] = {
	{
		I2C_BOARD_INFO("atmel_mxt_ts", 0x4a),
		.platform_data = &atmel_mxt_ts_pdata,
	},
};

static int melfas_mux_fw_flash(bool to_gpios)
{
	/* TOUCH_EN is always an output */
	if (to_gpios) {
		gpio_direction_output(GPIO_TOUCH_IRQ, 0);
		omap_mux_set_gpio(OMAP_PIN_INPUT | OMAP_MUX_MODE3,
				  GPIO_TOUCH_IRQ);

		gpio_direction_output(GPIO_TOUCH_SCL, 0);
		omap_mux_set_gpio(OMAP_PIN_INPUT | OMAP_MUX_MODE3,
				  GPIO_TOUCH_SCL);

		gpio_direction_output(GPIO_TOUCH_SDA, 0);
		omap_mux_set_gpio(OMAP_PIN_INPUT | OMAP_MUX_MODE3,
				  GPIO_TOUCH_SDA);
	} else {
		gpio_direction_output(GPIO_TOUCH_IRQ, 1);
		gpio_direction_input(GPIO_TOUCH_IRQ);
		omap_mux_set_gpio(OMAP_PIN_INPUT_PULLUP | OMAP_MUX_MODE3,
				  GPIO_TOUCH_IRQ);

		gpio_direction_output(GPIO_TOUCH_SCL, 1);
		gpio_direction_input(GPIO_TOUCH_SCL);
		omap_mux_set_gpio(OMAP_PIN_INPUT | OMAP_MUX_MODE0,
				  GPIO_TOUCH_SCL);

		gpio_direction_output(GPIO_TOUCH_SDA, 1);
		gpio_direction_input(GPIO_TOUCH_SDA);
		omap_mux_set_gpio(OMAP_PIN_INPUT | OMAP_MUX_MODE0,
				  GPIO_TOUCH_SDA);
	}

	return 0;
}

static struct mms_ts_platform_data mms_ts_pdata = {
	.max_x		= 720,
	.max_y		= 1280,
	.mux_fw_flash	= melfas_mux_fw_flash,
	.gpio_resetb	= GPIO_TOUCH_IRQ,
	.gpio_vdd_en	= GPIO_TOUCH_EN,
	.gpio_scl	= GPIO_TOUCH_SCL,
	.gpio_sda	= GPIO_TOUCH_SDA,
};

static struct i2c_board_info __initdata tuna_i2c3_boardinfo_final[] = {
	{
		I2C_BOARD_INFO("mms_ts", 0x48),
		.flags = I2C_CLIENT_WAKE,
		.platform_data = &mms_ts_pdata,
	},
};

void __init omap4_tuna_input_init(void)
{
	gpio_request(GPIO_TOUCH_IRQ, "tsp_int_n");
	gpio_direction_input(GPIO_TOUCH_IRQ);
	omap_mux_init_gpio(GPIO_TOUCH_IRQ, OMAP_PIN_INPUT_PULLUP);

	if (omap4_tuna_final_gpios()) {
		gpio_request(GPIO_TOUCH_EN, "tsp_en");
		gpio_direction_output(GPIO_TOUCH_EN, 1);
		omap_mux_init_gpio(GPIO_TOUCH_EN, OMAP_PIN_OUTPUT);
		gpio_request(GPIO_TOUCH_SCL, "ap_i2c3_scl");
		gpio_request(GPIO_TOUCH_SDA, "ap_i2c3_sda");

		tuna_i2c3_boardinfo_final[0].irq = gpio_to_irq(GPIO_TOUCH_IRQ);

		i2c_register_board_info(3, tuna_i2c3_boardinfo_final,
			ARRAY_SIZE(tuna_i2c3_boardinfo_final));
	}

	if (omap4_tuna_get_revision() == TUNA_REV_PRE_LUNCHBOX) {
		tuna_i2c3_boardinfo_pre_lunchbox[0].irq = gpio_to_irq(GPIO_TOUCH_IRQ);

		i2c_register_board_info(3, tuna_i2c3_boardinfo_pre_lunchbox,
			ARRAY_SIZE(tuna_i2c3_boardinfo_pre_lunchbox));

		omap_mux_init_signal("kpd_row1.kpd_row1", OMAP_PIN_INPUT_PULLUP);
		omap_mux_init_signal("kpd_row2.kpd_row2", OMAP_PIN_INPUT_PULLUP);
		omap_mux_init_signal("kpd_col1.kpd_col1", OMAP_PIN_OUTPUT);
		omap4_keyboard_init(&tuna_keypad_data, &keypad_data);
		tuna_gpio_keypad_data.info_count = 1;
	} else {
		omap_mux_init_gpio(8, OMAP_PIN_INPUT);
		omap_mux_init_gpio(30, OMAP_PIN_INPUT);
	}

	platform_device_register(&tuna_gpio_keypad_device);
}

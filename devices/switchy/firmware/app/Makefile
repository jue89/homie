# a minimal application Makefile
APPLICATION = homie-switchy
BOARD ?= homie-switchy

USEMODULE += ztimer_usec
USEMODULE += ztimer_msec
USEMODULE += evtimer_on_ztimer
USEMODULE += periph_gpio_irq
USEMODULE += periph_pwm

RIOTBASE ?= $(CURDIR)/../../../../firmware/RIOT
EXTERNAL_BOARD_DIRS += $(CURDIR)/../board
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../../firmware/drivers/hand_counter
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../../firmware/drivers/dimmable_led
include $(RIOTBASE)/Makefile.include
# a minimal application Makefile
APPLICATION = homie-switchy
BOARD ?= homie-switchy

USEMODULE += saul_default
USEMODULE += hdp
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_ipv6_default

DEVELHELP := 1

RIOTBASE ?= $(CURDIR)/../../../../firmware/RIOT
EXTERNAL_BOARD_DIRS += $(CURDIR)/../board
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../../firmware/drivers/hand_counter
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../../firmware/drivers/dimmable_led
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../../../firmware/sys/hdp
include $(RIOTBASE)/Makefile.include
APPLICATION = netif-bridge
BOARD ?= homie-picap

USEMODULE += dose
USEMODULE += stdio_ethos
USEMODULE += gnrc_pktbuf_static
USEMODULE += ztimer ztimer_usec

# Enable if you want to enable the watchdog timer
#USEMODULE += periph_wdt

# Debugging helpers
#USEMODULE += ps
#USEMODULE += schedstatistics
#DEVELHELP ?= 1

RIOTBASE ?= $(CURDIR)/../../../../firmware/RIOT
EXTERNAL_BOARD_DIRS ?= $(CURDIR)/../board
include $(RIOTBASE)/Makefile.include

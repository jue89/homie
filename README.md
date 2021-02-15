# Homie

Homie is an attempt to create wired smart-home devices.
Those devices are connected using [DOSE](https://riot-os.org/api/group__drivers__dose.html).

## Bus Interface

Every device has two *4P4C* jacks. This enables daisy-chaining the devices into one serial bus. The first and last device have to be terminated using a 120Ω resistor.

The bus interface:

| Pin | Description |
| --- | ----------- |
| 1   | GND         |
| 2   | CAN LOW     |
| 3   | CAN HIGH    |
| 4   | +5V         |


## Debug Interface

The debug interface features:
- Accessing board RESET and BOOT selector line
- Debug UART
- SWD Interface
- Power supply

The pin layout is designed to tolerate wrongly-connected pin header.

All data lines (SWDIO, SWCLK, RX, TX) hat to be protected against short-circuit on the debug port. (E.g. series resistors, ...)

| Description                                                                 | Name  | Pin    | Pin    | Name  | Description                                                                               |
| --------------------------------------------------------------------------- | :---: | :----: | :----: | :---: | ----------------------------------------------------------------------------------------- |
| Selectes the boot mode. Pulled with a on-board resistor to the right level. | BOOT  |  **1** | **2**  | RX    | Debug UART RX (Debug Adapter -> Board)                                                    |
| Voltage for all IO pins                                                     | VIO   |  **3** | **4**  | TX    | Debug UART TX (Board -> Debug Adapter)                                                    |
|                                                                             | NC    |  **5** | **6**  | VIN   | Supply for the borad. The debug adapter has to protect this current direct using a diode. |
| SWD data line                                                               | SWDIO |  **7** | **8**  | SWCLK | SWD clock line                                                                            |
| Board reset. Low active.                                                    | RESET |  **9** | **10** | GND   | Ground                                                                                    |
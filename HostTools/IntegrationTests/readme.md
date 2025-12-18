# Collection of scripts for manual and automated integration tests

## Harwdare in the Loop

### STM32 - ESP8266 AT - MQTT
Tests are required to verify the correct behavior of the communication handler of Robotto.
Robotto UART can be connected to the host system instead of the ESP, and script "at_emulator.py" provides the emulation of the AT responsed.



AT+UART_DEF=9600,8,1,0,0
AT+UART_DEF=115200,8,1,0,0
AT+CWJAP="SOPPALCO","  "[,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<scan_mode>][,<jap_timeout>][,<pmf>]

AT+UART_CUR=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>
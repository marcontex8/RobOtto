
# Notes about ESP8266 - AT configuration and MQTT connection

## ESP-01S Firmware download
It seems that cytron firmware is fitting my device:
https://github.com/CytronTechnologies/esp-at-binaries?tab=readme-ov-file

I'm having a strange behavior when connecting (garbage in terminal)
Somehow it was having inwanted restart, but currently it's not happening anymore.
It would be better to switch to more modern SoC because this one is not supported anymore

## Firmware flashing
cd /media/marco/DATI/EmbeddedDev/esp-at-binaries-main
esptool.py -p /dev/ttyUSB0 -b 115200 write_flash -ff 26m -fm dio -fs detect 0 Cytron_ESP-01S_AT_Firmware_V2.2.0.bin

## Connecting to WIFI
Any serial terminal should work. I used:
picocom --omap crcrlf -b 115200 -q /dev/ttyUSB0

AT
>
>OK

go to station mode
AT+CWMODE=1
>
>OK

connect to wifi
AT+CWJAP="SSID","PWD"
>WIFI CONNECTED
>WIFI GOT IP
>
>OK

AT+MQTTUSERCFG=0,1,"RobOTTO","","",0,0,""
>
>OK

AT+MQTTCONN=0,"192.168.1.140",1884,0
>+MQTTCONNECTED:0,1,"192.168.1.140","1884","",0
>
>OK



AT+MQTTPUB=0,"test/topic","hello from esp01",0,0
>
>OK

in case
AT+RESTORE
AT+CWLAP //scan wifi
AT+SYSLOG=1
AT+SYSMSG=1


# Flash AT firmware to ESP32

# Install esptool

# Flash command
- 'cd Esp32/ESP32-C3-MINI-1-AT-V4.1.1.0/ESP32-C3-MINI-1-AT-V4.1.1.0'
- First connect ESP32 SuperMini with USB-C cable.
- Than press the "boot" button and, while keeping it pressed, press once the "rst" button. Finally release the "boot" one too.
- Now run: 'esptool read-mac'. It should list the ESP32-C connected to one of the ports (likely /dev/ttyACM0)
- If so, run the flashing command:
esptool.py --chip auto --port /dev/ttyACM0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0x0 bootloader/bootloader.bin 0x60000 esp-at.bin 0x8000 partition_table/partition-table.bin 0xd000 ota_data_initial.bin 0x1e000 at_customize.bin 0x1f000 customized_partitions/mfg_nvs.bin

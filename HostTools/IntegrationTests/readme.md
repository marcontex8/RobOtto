# Collection of scripts for manual and automated integration tests

## Run all automated tests

```bash
cd /home/marco/STM32CubeIDE/workspace_1.19.0/Robotto
./.venv/bin/python -m unittest discover -s HostTools/IntegrationTests -p "test_*.py"
```

## Manual harwdare in the Loop
STM32 - ESP32 AT - MQTT
Tests are required to verify the correct behavior of the communication handler of Robotto.
 - Robotto UART can be connected to the host system instead of the ESP, and script "at_emulator.py" provides the emulation of the AT responsed.
 - "at_response_test.py" allow user to read transmissions from serial and send messages. It can be used as a mock for Robotto to verify ESP32 or the other way around. Also it can be connected in between, getting data from one device and transmitting the the second.

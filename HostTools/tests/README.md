# HostTools Python Tests

## Run all tests

```bash
cd /home/marco/STM32CubeIDE/workspace_1.19.0/Robotto
./.venv/bin/python -m unittest discover -s HostTools/tests -p "test_*.py"
```

## Run a single test file

```bash
cd /home/marco/STM32CubeIDE/workspace_1.19.0/Robotto
./.venv/bin/python -m unittest HostTools.tests.test_telemetry_decode
```

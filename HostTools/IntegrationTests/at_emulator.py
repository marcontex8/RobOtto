from dotenv import load_dotenv
import os
import serial
import time

load_dotenv()

PORT = "/dev/ttyUSB0"
BAUD = 115200

ssid = os.getenv("WIFI_SSID")
pwd  = os.getenv("WIFI_PWD")

responses = {
    "AT": (["OK"], 9.0),
    "ATE0": (["OK"], 9.0),
    "AT+CWMODE=1": (["OK"], 5.0),
    f'AT+CWJAP="{ssid}","{pwd}"': (["WIFI CONNECTED", "WIFI GOT IP", "OK"], 15.5),
    'AT+MQTTUSERCFG=0,1,"RobOTTO","","",0,0,""': (["OK"], 0.4),
    'AT+MQTTCONN=0,"192.168.1.140",1884,0': (['+MQTTCONNECTED:0,1,"192.168.1.140","1884","",0', "OK"], 2),
    'AT+MQTTPUB=0,"test/topic","hello from esp01",0,0': (["OK"], 0.4),
}

def main():
    print(responses.keys())
    print(f"Opening {PORT} at {BAUD} baud...")
    ser = serial.Serial(PORT, BAUD, timeout=0.5)

    buffer = ""

    while True:
        data = ser.read().decode(errors="ignore")
        if data:
            if data in ("\n", "\r"):
                line = buffer.strip()
                buffer = ""
                if not line:
                    continue
                timestamp = time.strftime("%H:%M:%S", time.localtime())
                print(f"{timestamp} | > {line}")

                # Get responses and total delay
                resp_data = responses.get(line)
                if resp_data:
                    resp_list, total_delay = resp_data
                    delay_per = total_delay / len(resp_list) if resp_list else 0
                    for r in resp_list:
                        time.sleep(delay_per)
                        timestamp = time.strftime("%H:%M:%S", time.localtime())
                        ser.write((r + "\r\n").encode())
                        print(f"{timestamp} | < {r}")
                else:
                    print("  (unknown command)")
                    # Default for unknown: OK with 0.2 total delay
                    time.sleep(0.2)
                    ser.write(b"OK\r\n")
                    print("> OK")

            else:
                buffer += data


if __name__ == "__main__":
    main()

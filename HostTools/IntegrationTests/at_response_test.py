import serial
import time
import threading

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

def serial_reader():
    while True:
        if ser.in_waiting > 0:
            data = ser.readline()#.decode('utf-8', errors='ignore').strip()
            if data:
                print("Received:", data)

reader_thread = threading.Thread(target=serial_reader, daemon=True)
reader_thread.start()

while True:
    command = input("Enter AT command: ")
    if command.lower() == 'exit':
        break
    
    formatted_command = command + '\r\n'
    ser.write(formatted_command.encode())

ser.close()

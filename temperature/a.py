import time
import random

def read_temperature():
    # Simulate reading temperature from a sensor
    return round(random.uniform(20.0, 30.0), 2)

def log_temperature(temperature):
    with open('temperature_log.txt', 'a') as file:
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
        file.write(f'{timestamp}: {temperature}°C\n')

def main():
    print("Starting temperature logging...")
    try:
        while True:
            temperature = read_temperature()
            log_temperature(temperature)
            print(f'Temperature logged: {temperature}°C')
            time.sleep(60)  # Wait for 60 seconds before reading the temperature again
    except KeyboardInterrupt:
        print("Temperature logging stopped.")

if __name__ == '__main__':
    main()
from machine import Pin, ADC
import time

# Définir le port GPIO
water_level_sensor = ADC(Pin(26))

while True:
    # Lire la valeur analogique
    value = water_level_sensor.read_u16()
    print("Valeur analogique: ", value)
    
    # Pause
    time.sleep(0.1)
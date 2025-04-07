#include <Wire.h>
#include "rgb_lcd.h"

// Def water_level_sensor
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

#define NO_TOUCH            0xFE
#define THRESHOLD           100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR    0x77

// Variables écran LCD
rgb_lcd lcd;
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// Variables Transistor
const int T1 = 8;
const int T2 = 9;

// ----- Début de programme pour le water_level_sensor ----- //

void getHigh12SectionValue(void)
{
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());

  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  delay(10);
}

void getLow8SectionValue(void)
{
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());

  for (int i = 0; i < 8 ; i++) {
    low_data[i] = Wire.read(); // Reçu d'un octet sous forme de caractère
  }
  delay(10);
}

void check()
{
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;
  while (1)
  {
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    low_count = 0;
    high_count = 0;
    getLow8SectionValue();
    getHigh12SectionValue();

    Serial.println("low 8 sections value = ");
    for (int i = 0; i < 8; i++)
    {
      Serial.print(low_data[i]);
      Serial.print(".");
      if (low_data[i] >= sensorvalue_min && low_data[i] <= sensorvalue_max)
      {
        low_count++;
      }
      if (low_count == 8)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }
    Serial.println("  ");
    Serial.println("  ");
    Serial.println("high 12 sections value = ");
    for (int i = 0; i < 12; i++)
    {
      Serial.print(high_data[i]);
      Serial.print(".");

      if (high_data[i] >= sensorvalue_min && high_data[i] <= sensorvalue_max)
      {
        high_count++;
      }
      if (high_count == 12)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }

    Serial.println("  ");
    Serial.println("  ");

    for (int i = 0 ; i < 8; i++) {
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;

      }
    }
    for (int i = 0 ; i < 12; i++) {
      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }

    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }

    // Affichage des informations sur le port série
    SERIAL.print("water level = ");
    SERIAL.print(trig_section * 5);
    SERIAL.println("% ");
    SERIAL.println(" ");
    SERIAL.println("*********************************************************");

    // Affichage des informations à l'écran
    lcd.clear();
    lcd.print("Niveau d'eau :");
    lcd.setCursor(0, 1);
    lcd.print(trig_section * 5);

    int pompage = trig_section * 5;

    // Contrôle de la pompe de remplissage
    if (pompage <= 10) {
      digitalWrite(T1, HIGH);
      SERIAL.println("Pompe allumé ! > Remplissage");
    }
    else {
      digitalWrite(T1, LOW);
    }

    // Contrôle de la pompe d'aspiration
    if (pompage >= 20) {
      digitalWrite(T2, HIGH);
      SERIAL.println("Pompe allumé ! > Aspiration");
    }
    else {
      digitalWrite(T2, LOW);
    }

    delay(500);
  }
}

// ----- Fin de programme pour le water_level_sensor ----- //

void setup() {
  // Setup de l'écran (def, color & txt)
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);

  pinMode(T1, OUTPUT);
  pinMode(T2, OUTPUT);
  
  // Setup du capteur
  SERIAL.begin(9600);
  Wire.begin();

}

void loop() {
  // Lecture du niveau d'eau
  check();

  // Délai de 100ms
  delay(100);
}

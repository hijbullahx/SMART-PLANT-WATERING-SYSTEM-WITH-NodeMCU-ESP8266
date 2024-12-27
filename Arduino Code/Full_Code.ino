#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "Enter your Auth token";
char ssid[] = "Enter your WIFI name";
char pass[] = "Enter your WIFI password";

BlynkTimer timer;
bool Relay = 0;

#define sensor A0
#define waterPump D3

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
  lcd.init();
  lcd.backlight();

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Display Title during initialization
  char text[] = "Title";
  for (int i = 0; i < 5; i++) {
    lcd.setCursor(i, 0);
    lcd.print(text[i]);
    delay(500);
  }
  lcd.clear();

  // Display loading animation
  lcd.setCursor(1, 0);
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();

  // Call the function
  timer.setInterval(100L, soilMoistureSensor);
}

// Get the button value
BLYNK_WRITE(V1) {
  Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(waterPump, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON ");
  } else {
    digitalWrite(waterPump, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF");
  }
}

// Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(sensor);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  Blynk.virtualWrite(V0, value);
  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(value);
  lcd.print(" ");

  // Check moisture level and control motor accordingly
  if (value < 10 || Relay == 1) {
    digitalWrite(waterPump, HIGH); // Turn on the motor
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON ");
  } else if (value >= 30) {
    digitalWrite(waterPump, LOW); // Turn off the motor
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF");
  }
}

void loop() {
  Blynk.run(); // Run the Blynk library
  timer.run(); // Run the Blynk timer
}

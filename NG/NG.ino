#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

// Inisialisasi LCD dengan alamat I2C 0x27 dan ukuran 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins HX711
const int HX711_dout = 15; // MCU > HX711 DOUT
const int HX711_sck = 5;   // MCU > HX711 SCK
const int trigPin = 19;
const int echoPin = 18;

// HX711 Constructor
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAddress = 0;
unsigned long t = 0;
float calibrationValue;

#define TOUCH_PIN_MODE T9
#define TOUCH_PIN_TARE T8

int threshold = 65; // for touchpin
long sensorHeight = 179;

void calibrate() {
  Serial.println("Masukkan berat yang diketahui dalam kg (contoh: 1.5):");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input weight kg:");
  lcd.setCursor(0, 1);

  while (Serial.available() == 0) {
    delay(100); // Tunggu input
  }
  
  float knownWeight = Serial.parseFloat(); // Baca berat yang diketahui
  Serial.println(knownWeight);
  Serial.println("Tunggu, mengkalibrasi...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  
  float rawValue = LoadCell.getData(); // Ambil nilai mentah dari sensor
  float newCalFactor = rawValue / knownWeight; // Hitung faktor kalibrasi baru
  
  Serial.print("Kalibrasi selesai! Faktor kalibrasi: ");
  Serial.println(newCalFactor);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration Done");
  lcd.clear();
  

  // Simpan ke EEPROM jika diperlukan
  #if defined(ESP8266)|| defined(ESP32)
    EEPROM.begin(512);
  #endif
  EEPROM.put(calVal_eepromAddress, newCalFactor);
  #if defined(ESP8266)|| defined(ESP32)
    EEPROM.commit();
  #endif
  LoadCell.setCalFactor(newCalFactor);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(TOUCH_PIN_MODE, INPUT_PULLDOWN);
  pinMode(TOUCH_PIN_TARE, INPUT_PULLDOWN);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  
  LoadCell.begin();
  EEPROM.get(calVal_eepromAddress, calibrationValue);
  if (calibrationValue == 0 || isnan(calibrationValue)) { // Jika EEPROM kosong
    calibrationValue = 20.90; // Gunakan nilai default
  }

  Serial.print("Nilai Kalibrasi dari EEPROM: ");
  Serial.println(calibrationValue);
  
  unsigned long stabilizingtime = 2000; 
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check wiring!");
    lcd.setCursor(0, 1);
    lcd.print("Error: Wiring!");
    while (1);
  } else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup complete");
    lcd.setCursor(0, 0);
    lcd.print("Scale Ready!");
    lcd.setCursor(0, 1);
    lcd.print("Weight: 0.000kg");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NutriGrowth");
  lcd.setCursor(0, 1);
  lcd.print("V 1.0");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Bagian touch sensor tetap sama
  int modeTouch = touchRead(TOUCH_PIN_MODE);
  int tareTouch = touchRead(TOUCH_PIN_TARE);

  if (modeTouch < threshold) {
    sensorHeight = (sensorHeight == 179) ? 100 : 179;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print((sensorHeight == 100) ? "Mode Balita" : "Mode Dewasa");
    delay(500);
    lcd.clear();
  }

  if (tareTouch < threshold) {
    LoadCell.tare();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tare Selesai");
    delay(1000);
    lcd.clear();
  }

  // Bagian sensor ultrasonic tetap sama
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  long height = sensorHeight - distance;

  lcd.setCursor(0, 0);
  lcd.print("TB: ");
  lcd.print(height);
  lcd.print(" cm");

  Serial.println("===========================");
  Serial.print("HC04 Output: ");
  Serial.println(distance);
  Serial.println("TB:");
  Serial.print(height);


  static boolean newDataReady = 0;
  const int serialPrintInterval = 300; // Interval update tampilan

  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float weight = LoadCell.getData() / 1000.0; // Konversi ke kg

      Serial.println(" cm");
      Serial.print("Load_cell output: ");
      Serial.println(weight);
      Serial.println("===========================");
     
      lcd.setCursor(0, 1);
      lcd.print("BB: ");
      lcd.print(weight, 3);
      lcd.print(" kg ");
  
      newDataReady = 0;
      t = millis();
    }
  }

  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell.tareNoDelay();
      lcd.setCursor(0, 1);
      lcd.print("Taring...      ");
    } else if (inByte == 'c') {
      calibrate();
    }
    lcd.clear();
  }

  if (LoadCell.getTareStatus()) {
    Serial.println("Tare complete");
    lcd.setCursor(0, 1);
    lcd.print("BB: 0.000kg     ");
    lcd.clear();
  }
  delay(200);
}

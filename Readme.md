Below is the English documentation you can include in your GitHub README.md file:

---

# NG: Weight and Height Measurement System

NG is a microcontroller-based system (compatible with Arduino, ESP8266, ESP32, or AVR boards) that integrates multiple sensors to measure weight and height. The system utilizes:

- **Load Cell with HX711 ADC** for weight measurement.
- **Ultrasonic Sensor** (e.g., HC-SR04) for height measurement.
- **I2C LCD (16x2)** to display real-time information.
- **Touch Sensors** to change measurement modes and perform tare (zeroing).

## Key Features

- **Weight Measurement**:
  - Uses a load cell amplified by an HX711 module.
  - Supports tare functionality for accurate weight readings.

- **Height Measurement**:
  - Employs an ultrasonic sensor to measure distance.
  - Offers two measurement modes:
    - **Toddler Mode**: Uses a sensor height of 100 cm.
    - **Adult Mode**: Uses a sensor height of 179 cm.

- **LCD Interface**:
  - Displays system status, weight (BB) in kg, height (TB) in cm, and other system messages.

- **Load Cell Calibration**:
  - Allows calibration by entering a known weight via the Serial Monitor.
  - Stores the calibration factor in EEPROM to retain settings after resets.

- **Touch Sensor Control**:
  - One touch sensor toggles between the two height measurement modes.
  - Another touch sensor performs the tare function.

## Hardware Requirements

- **Microcontroller**: Arduino, ESP8266, ESP32, or AVR-based board.
- **Load Cell & HX711 Module**: For weight measurement.
- **Ultrasonic Sensor**: Such as the HC-SR04 for measuring distance/height.
- **I2C LCD 16x2**: For displaying measurements and messages.
- **Touch Sensors**: Using touch-capable pins (e.g., T8 and T9 for ESP32).
- **Connecting Wires & Breadboard**: For prototyping the circuit.

> **Note:** Ensure the pin configuration matches your specific board. For example, touch sensor pins (T8 and T9) are specific to the ESP32—adjust if using a different platform.

## Required Libraries

Make sure to install the following libraries via the Arduino Library Manager or manually:

- [Wire](https://www.arduino.cc/reference/en/language/functions/communication/wire/)
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
- [HX711_ADC](https://github.com/olkal/HX711_ADC)
- [EEPROM](https://www.arduino.cc/reference/en/language/functions/eeprom/) (included with many boards like ESP8266, ESP32, and AVR)

## Pin Configuration

- **HX711**:
  - **DOUT**: Pin 15
  - **SCK**: Pin 5

- **Ultrasonic Sensor**:
  - **Trig**: Pin 19
  - **Echo**: Pin 18

- **Touch Sensors**:
  - **Mode Toggle**: `TOUCH_PIN_MODE` (e.g., T9)
  - **Tare**: `TOUCH_PIN_TARE` (e.g., T8)

> **Tip:** Adjust the pins based on your hardware setup and board specifications.

## Setup and Operation

1. **Hardware Assembly**:
   - Connect the load cell to the HX711 module and attach it to the designated pins.
   - Wire the ultrasonic sensor to pins 19 (Trig) and 18 (Echo).
   - Connect the I2C LCD (default address 0x27) and ensure it is powered.
   - Attach the touch sensors to the appropriate pins (e.g., T8 and T9 for ESP32).

2. **Library Installation**:
   - Install the required libraries using the Arduino Library Manager.

3. **Uploading the Code**:
   - Compile and upload the code to your microcontroller.

4. **Calibrating the Load Cell**:
   - Open the Serial Monitor at a baud rate of 115200.
   - Type `c` in the Serial Monitor to initiate the calibration process.
   - Enter a known weight (e.g., `1.5` for 1.5 kg) and press enter.
   - The system calculates a new calibration factor and saves it to EEPROM.

5. **Using the System**:
   - **Measurements**:
     - The LCD displays **TB** (height in cm) measured via the ultrasonic sensor.
     - It also displays **BB** (weight in kg) measured by the load cell.
   - **Mode Toggle**:
     - Touch the mode sensor (`TOUCH_PIN_MODE`) to switch between **Toddler Mode** (sensor height = 100 cm) and **Adult Mode** (sensor height = 179 cm). The LCD will display the active mode.
   - **Tare Function**:
     - Touch the tare sensor (`TOUCH_PIN_TARE`) or send the character `t` via Serial to perform a tare (reset weight measurement). The LCD will confirm with a "Tare Complete" message.

## Code Explanation

### `calibrate()` Function
- **Purpose**: Calibrate the load cell using a known weight.
- **Process**:
  1. Prompts the user via the LCD and Serial Monitor to input a known weight.
  2. Reads the raw data from the load cell.
  3. Calculates a new calibration factor using the formula:  
     `newCalFactor = rawValue / knownWeight`
  4. Displays the new calibration factor.
  5. Saves the new calibration factor to EEPROM.
  6. Updates the HX711 module with the new calibration factor.

### `setup()` Function
- Initializes Serial communication, the LCD, and all sensors (HX711, ultrasonic sensor, touch sensors).
- Retrieves the calibration factor from EEPROM (or uses a default value if not found).
- Starts the HX711 with a stabilization period and performs an initial tare.
- Displays system startup messages on the LCD.

### `loop()` Function
- **Touch Sensor Handling**:
  - **Mode Toggle**: Reads the mode touch sensor to switch between toddler and adult modes, updating `sensorHeight` accordingly.
  - **Tare**: Reads the tare touch sensor to trigger a tare and display a confirmation message.
  
- **Ultrasonic Sensor**:
  - Sends a trigger pulse, measures the echo duration, and calculates the distance.
  - Computes the object’s height as the difference between `sensorHeight` and the measured distance.
  - Displays the height (TB) on the LCD and outputs the value to Serial.

- **Load Cell Measurement**:
  - Updates the load cell reading periodically.
  - Converts the raw data to kg and displays the weight (BB) on the LCD at set intervals.
  
- **Serial Commands**:
  - **`t`**: Initiates the tare function.
  - **`c`**: Starts the calibration process.
  
- **Error Handling**:
  - Checks for a tare timeout (e.g., due to wiring issues) and displays an error message if needed.

## Serial Commands
- **`t`**: Perform a tare (zeroing) on the load cell.
- **`c`**: Initiate the calibration process for the load cell.

## Troubleshooting

- **Tare Timeout**:
  - If you see "Timeout, check wiring!" on the Serial Monitor or LCD, verify the wiring connections to the load cell and HX711 module.
- **Calibration Accuracy**:
  - Ensure the known weight entered during calibration is precise to calculate an accurate calibration factor.
- **Touch Sensor Sensitivity**:
  - If the touch sensors are too sensitive or unresponsive, adjust the `threshold` value (default is 65) accordingly.

## License

--

## Acknowledgments

Special thanks to the contributors of the [HX711_ADC](https://github.com/olkal/HX711_ADC) and [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C) libraries, as well as all those who contributed to the development of this project.

---

This documentation should help you understand, set up, and expand the NG system. Happy building!
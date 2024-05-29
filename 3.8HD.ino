#include <Servo.h>
#include "DHT.h"

#define DHTPIN 2 // Digital pin for DHT22
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define MOISTURE_PIN A3 // Analog pin for Soil Moisture Sensor
#define GREEN_LED 5 // Green LED pin
#define RED_LED 6 // Red LED pin
#define YELLOW_LED 7 // Yellow LED pin
#define BUZZER_PIN 3 // Buzzer pin
#define SERVO_PIN 9 // Servo motor pin

DHT dht(DHTPIN, DHTTYPE);
Servo myservo; // Create servo object to control a servo
bool smartIrrigationMode = false;
bool sprinklerActive = false;
bool programRunning = true; // Flag to control the main loop

// Function Declarations
void printMenu();
void handleCommand(String command);
void checkMoisture();
void buzzerTest();
void ledTest();
void toggleSprinkler();
void toggleSmartIrrigationMode();
void tempTest();
void humidityTest();
void automatedIrrigation();
void automatedSprinkler();
void blinkLED(int ledPin, bool activateBuzzer, bool moveServo);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the LED pins as outputs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize the servo
  myservo.attach(SERVO_PIN);

  // Initialize the soil moisture sensor pin
  pinMode(MOISTURE_PIN, INPUT);

  // Print a message to the serial monitor to indicate setup is complete
  Serial.println("Setup complete.");
  printMenu();
}

void loop() {
  while (programRunning) {
    if (Serial.available()) {
      String command = Serial.readStringUntil('\n');
      command.trim();

      Serial.print("Received command: ");
      Serial.println(command);

      handleCommand(command);

      // Clear the buffer to avoid any leftover data
      while (Serial.available()) {
        Serial.read();
      }
    }

    if (smartIrrigationMode) {
      automatedIrrigation();
      delay(2000);
    }

    if (sprinklerActive) {
      automatedSprinkler();
      delay(100);
    }
  }
}

// User Interaction Functions

void printMenu() {
  Serial.println("\033[32mMenu For Smart Irrigation System.");
  Serial.println("1: LED test");
  Serial.println("2: Buzzer test");
  Serial.println("3: Check moisture");
  Serial.println("4: Temperature test");
  Serial.println("5: Humidity test");
  Serial.println("6: Toggle Sprinkler");
  Serial.println("7: Toggle Smart Irrigation Mode");
  Serial.println("8: Exit");
  Serial.println("Enter your choice: \033[0m"); // Resets the color back to default
}

void handleCommand(String command) {
  if (command == "1") {
    ledTest();
    Serial.println("LED test complete.");
  } else if (command == "2") {
    buzzerTest();
    Serial.println("Buzzer test complete.");
  } else if (command == "3") {
    checkMoisture();
    Serial.println("Moisture check complete.");
  } else if (command == "4") {
    tempTest();
    Serial.println("Temperature check complete.");
  } else if (command == "5") {
    humidityTest();
    Serial.println("Humidity check complete.");
  } else if (command == "6") {
    toggleSprinkler();
  } else if (command == "7") {
    toggleSmartIrrigationMode();
  } else if (command == "8") {
    Serial.println("Exiting program...");
    programRunning = false;
  } else {
    Serial.println("Unknown command.");
  }

  // Re-display the menu after handling the command if the program is still running and the sprinkler is not active
  if (programRunning && !sprinklerActive) {
    printMenu();
  }
}

// Sensor Handling Functions

void checkMoisture() {
  int sensorValue = analogRead(MOISTURE_PIN); // Read the value from the sensor
  int moistureLevel = map(sensorValue, 0, 1023, 100, 0); // Map it to a 0-100% range

  Serial.print("Moisture Level: ");
  Serial.print(moistureLevel);
  Serial.println("%");
}

void tempTest() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
}

void humidityTest() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed to read humidity from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
}

// LED and Buzzer Functions

void buzzerTest() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, LOW); // Activate the buzzer (low-level trigger)
    delay(500);
    digitalWrite(BUZZER_PIN, HIGH); // Deactivate the buzzer
    delay(500);
  }
}

void ledTest() {
  digitalWrite(GREEN_LED, HIGH);
  delay(500); // Green LED on for 0.5 seconds
  digitalWrite(GREEN_LED, LOW);
  delay(500); // Green LED off for 0.5 seconds

  digitalWrite(YELLOW_LED, HIGH);
  delay(500); // Yellow LED on for 0.5 seconds
  digitalWrite(YELLOW_LED, LOW);
  delay(500); // Yellow LED off for 0.5 seconds

  digitalWrite(RED_LED, HIGH);
  delay(500); // Red LED on for 0.5 seconds
  digitalWrite(RED_LED, LOW);
  delay(500); // Red LED off for 0.5 seconds
}

// Sprinkler Control Functions

void toggleSprinkler() {
  sprinklerActive = !sprinklerActive;
  Serial.print("Sprinkler (Manually) ");
  Serial.println(sprinklerActive ? "Started." : "\033[31mStopped.\033[0m\n");
}

void automatedSprinkler() {
  myservo.write(90);
  delay(1000);
  myservo.write(180);
  delay(1000);
  myservo.write(0);
  delay(1000);
  // Sweep from 0 to 180 degrees:
  for (int angle = 0; angle <= 180; angle += 1) {
    myservo.write(angle);
    delay(15);
  }
  // Sweep back from 180 to 0 degrees:
  for (int angle = 180; angle >= 0; angle -= 1) {
    myservo.write(angle);
    delay(15);
  }
  delay(1000);
}

// Smart Irrigation Mode Functions

void toggleSmartIrrigationMode() {
  smartIrrigationMode = !smartIrrigationMode;
  Serial.print("Smart Irrigation Mode ");
  Serial.println(smartIrrigationMode ? "Activated." : "\033[31mDeactivated.\033[0m\n");
}

void automatedIrrigation() {
  // Read temperature and humidity from the DHT22 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Read moisture level from the Capacitive Soil Moisture Sensor
  int sensorValue = analogRead(MOISTURE_PIN); // Read the value from the sensor
  int moistureLevel = map(sensorValue, 0, 1023, 100, 0); // Map it to a 0-100% range

  // Print the sensor readings to the serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.print("°C  Moisture Level: ");
  Serial.print(moistureLevel);
  Serial.println("%");

  // Implement logic to control the LEDs and servo motor based on moisture level
  if (moistureLevel > 70) {
    blinkLED(GREEN_LED, false, false); // Blink green LED
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
  } else if (moistureLevel >= 50 && moistureLevel <= 69) {
    blinkLED(YELLOW_LED, false, false); // Blink yellow LED
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } else {
    blinkLED(RED_LED, true, true); // Blink red LED, sound the buzzer, and move the servo continuously
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
  }
}

void blinkLED(int ledPin, bool activateBuzzer, bool moveServo) {
  for (int i = 0; i < 2; i++) {
    digitalWrite(ledPin, HIGH);
    if (activateBuzzer) {
      digitalWrite(BUZZER_PIN, LOW); // Activate the buzzer (assuming it's active low)
    }
    if (moveServo) {
      myservo.write(180); // Move the servo to 180 degrees
    }
    delay(500);
    digitalWrite(ledPin, LOW);
    if (activateBuzzer) {
      digitalWrite(BUZZER_PIN, HIGH); // Deactivate the buzzer
    }
    if (moveServo) {
      myservo.write(0); // Move the servo back to 0 degrees
    }
    delay(500);
  }
}



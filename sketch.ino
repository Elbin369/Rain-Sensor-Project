const int RAIN_SENSOR = PA0;
const int LED_PIN = PB12;
const int BUZZER_PIN = PB13;
const int BUTTON_PIN = PB14; // Connected to button (other side to GND)

const int RAIN_THRESHOLD = 200;

bool systemState = false;      // Tracks whether the system is ON or OFF
bool lastButtonState = HIGH;   // Keeps track of the previous button state

// Variables for non-blocking timing
unsigned long lastRainCheck = 0;
const unsigned long rainCheckInterval = 300;

// Variables for non-blocking "beep beep" sound
unsigned long lastBuzzerToggle = 0;
bool buzzerOn = false;
const unsigned long beepOnTime = 120;  // Beep duration in ms
const unsigned long beepOffTime = 120; // Pause between beeps in ms

int rainValue = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor for the button

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

// Function to check the button at any time
void checkButton() {
  int buttonState = digitalRead(BUTTON_PIN);

  // Detect transition from HIGH to LOW (button pressed)
  if (buttonState == LOW && lastButtonState == HIGH) {
    systemState = !systemState; // Flip state (ON to OFF, or OFF to ON)
    
    if (systemState) {
      Serial.println("System turned ON");
    } else {
      Serial.println("System turned OFF");
      // Turn off outputs immediately
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      buzzerOn = false;
    }
    
    delay(200); // Debounce delay
  }
  lastButtonState = buttonState;
}

void loop() {
  // Always check button responsiveness
  checkButton();

  // Run sensor logic if system is ON
  if (systemState) {
    unsigned long currentMillis = millis();

    // 1. Read rain sensor periodically
    if (currentMillis - lastRainCheck >= rainCheckInterval) {
      lastRainCheck = currentMillis;
      rainValue = analogRead(RAIN_SENSOR);
      
      Serial.print("Rain value: ");
      Serial.println(rainValue);
    }

    // 2. Handle LED and Non-blocking Beep Beep Sound
    if (rainValue > RAIN_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);

      // Pulse buzzer on/off every 120ms without stopping the loop
      if (buzzerOn && (currentMillis - lastBuzzerToggle >= beepOnTime)) {
        noTone(BUZZER_PIN);
        buzzerOn = false;
        lastBuzzerToggle = currentMillis;
      } 
      else if (!buzzerOn && (currentMillis - lastBuzzerToggle >= beepOffTime)) {
        tone(BUZZER_PIN, 2500);
        buzzerOn = true;
        lastBuzzerToggle = currentMillis;
      }
    } else {
      // Rain is below threshold
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      buzzerOn = false;
    }

  } else {
    // System is OFF
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
    buzzerOn = false;
  }
}

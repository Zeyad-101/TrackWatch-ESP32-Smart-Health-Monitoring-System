#include <Wire.h>
#include <U8g2lib.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// --------------------------------------------------------------------------------
// 1. ANIMATION BITMAPS
// --------------------------------------------------------------------------------
const unsigned char runner_A[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0xFC, 0x01, 
  0xFC, 0x00, 0x0C, 0x00, 0x4C, 0x06, 0xCC, 0x19, 0x80, 0x30, 0x00, 0x30, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char runner_B[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x00, 0x3E, 0x00, 
  0x18, 0x00, 0x18, 0x00, 0x18, 0x0C, 0x2C, 0x0C, 0x06, 0x0C, 0x06, 0x06, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// --------------------------------------------------------------------------------
// 2. CONFIGURATION
// --------------------------------------------------------------------------------
// Time
int hours = 23;   
int minutes = 37; 
int seconds = 0; 

// Temp Sensor
#define ONE_WIRE_BUS 4  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float currentTempC = 0.0;
long lastTempTime = 0;

// Accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
int stepCount = 0;
float threshold = 15.0; 
long lastStepTime = 0;
bool animState = false; 

// --------------------------------------------------------------------------------
// 3. HARDWARE SETUP
// --------------------------------------------------------------------------------
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
MAX30105 particleSensor;

// --------------------------------------------------------------------------------
// 4. VARIABLES
// --------------------------------------------------------------------------------
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;

long lastDisplayTime = 0; 
long lastClockTick = 0;   

// --- NEW VARIABLES FOR WAVE ANIMATION ---
int waveBuffer[128]; // Store Y coordinates for the whole screen width
int wavePhase = 0;   // Where are we in the heartbeat spike? (0 = flatline)

// --------------------------------------------------------------------------------
// 5. SETUP
// --------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 

  sensors.begin();
  u8g2.begin();

  // Initialize Wave Buffer to middle of screen (Y=45)
  for(int i=0; i<128; i++) {
    waveBuffer[i] = 45;
  }

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not found.");
  }
  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  if(!accel.begin()) {
    Serial.println("No ADXL345 detected");
  } else {
    accel.setRange(ADXL345_RANGE_16_G);
  }
}

// --------------------------------------------------------------------------------
// 6. MAIN LOOP
// --------------------------------------------------------------------------------
void loop() {
  // --- A. CLOCK COUNTING ---
  if (millis() - lastClockTick >= 1000) {
    lastClockTick = millis();
    seconds++;
    if (seconds > 59) { seconds = 0; minutes++; }
    if (minutes > 59) { minutes = 0; hours++; }
    if (hours > 23) { hours = 0; }
  }

  // --- B. STEP COUNTING ---
  sensors_event_t event; 
  accel.getEvent(&event);
  float totalAccel = sqrt(pow(event.acceleration.x, 2) + 
                          pow(event.acceleration.y, 2) + 
                          pow(event.acceleration.z, 2));
                          
  if (totalAccel > threshold && (millis() - lastStepTime > 300)) {
    stepCount++;
    lastStepTime = millis();
    animState = !animState; 
  }

  // --- C. SENSOR & SCREEN LOGIC ---
  long irValue = particleSensor.getIR();

  if (irValue < 50000) {
    // ===========================
    // MODE: IDLE (Clock)
    // ===========================
    beatAvg = 0; 
    rates[0] = 0;
    
    // Reset wave to flat when finger removed
    wavePhase = 0; 

    if (millis() - lastTempTime > 2000) {
      lastTempTime = millis();
      sensors.requestTemperatures(); 
      currentTempC = sensors.getTempCByIndex(0);
    }
    
    if (millis() - lastDisplayTime > 100) {
      drawClockScreen();
      lastDisplayTime = millis();
    }

  } else {
    // ===========================
    // MODE: HEART RATE (Wave)
    // ===========================
    if (checkForBeat(irValue) == true) {
      long delta = millis() - lastBeat;
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++) beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }

      // TRIGGER THE WAVE SPIKE
      if (wavePhase == 0) {
        wavePhase = 1; // Start the animation sequence
      }
    }
    
    // Refresh fast (40ms) so the wave looks smooth
    if (millis() - lastDisplayTime > 40) {
      drawHeartRateScreen();
      lastDisplayTime = millis();
    }
  }
}

// --------------------------------------------------------------------------------
// HELPER FUNCTIONS
// --------------------------------------------------------------------------------

void drawClockScreen() {
  u8g2.clearBuffer();
  
  if (animState) u8g2.drawXBMP(5, 0, 16, 16, runner_A);
  else u8g2.drawXBMP(5, 0, 16, 16, runner_B);

  u8g2.setFont(u8g2_font_ncenB08_tr);
  char stepStr[15];
  sprintf(stepStr, "%d Steps", stepCount);
  int stepWidth = u8g2.getStrWidth(stepStr);
  u8g2.drawStr(125 - stepWidth, 12, stepStr);
  
  u8g2.drawLine(0, 18, 128, 18);

  int displayHour = hours;
  const char* am_pm = "AM";
  if (hours >= 12) {
    am_pm = "PM";
    if (hours > 12) displayHour -= 12;
  }
  if (displayHour == 0) displayHour = 12;

  u8g2.setFont(u8g2_font_helvB18_tf); 
  char timeStr[10];
  sprintf(timeStr, "%02d:%02d", displayHour, minutes);
  int timeWidth = u8g2.getStrWidth(timeStr);
  u8g2.drawStr((128 - timeWidth) / 2, 45, timeStr);

  u8g2.setFont(u8g2_font_ncenB08_tr);
  char tempStr[10];
  if (currentTempC == -127.00) sprintf(tempStr, "Err");
  else sprintf(tempStr, "%.1f C", currentTempC);
  u8g2.drawStr(5, 62, tempStr);
  u8g2.drawStr(105, 62, am_pm);

  u8g2.sendBuffer();
}

void drawHeartRateScreen() {
  u8g2.clearBuffer();

  // 1. UPDATE WAVE DATA
  // Shift all data to the left
  for (int i = 0; i < 127; i++) {
    waveBuffer[i] = waveBuffer[i+1];
  }

  // Calculate new pixel at the end (index 127)
  int baseLine = 45; // The vertical middle of the wave area
  int newY = baseLine;

  if (wavePhase > 0) {
    // Create the "Pumping" shape (QRS Complex)
    // We change the height based on the phase
    switch(wavePhase) {
      case 1: newY = baseLine - 5; break;  // Dip
      case 2: newY = baseLine + 5; break;  // Small Rise
      case 3: newY = baseLine - 20; break; // BIG SPIKE UP
      case 4: newY = baseLine + 15; break; // Big Drop Down
      case 5: newY = baseLine - 5; break;  // Recovery
      case 6: newY = baseLine; break;      // Back to normal
    }
    wavePhase++;
    if (wavePhase > 6) wavePhase = 0; // End of pulse
  } else {
    // No pulse: Draw flat line with tiny random noise
    newY = baseLine + random(-2, 2); 
  }
  
  waveBuffer[127] = newY;

  // 2. DRAW WAVE
  // Loop through buffer and draw lines connecting dots
  for (int i = 0; i < 127; i++) {
    u8g2.drawLine(i, waveBuffer[i], i+1, waveBuffer[i+1]);
  }

  // 3. DRAW BPM TEXT (Top)
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 12, "Heart Rate");
  
  char buffer[10];
  if (beatAvg > 0) sprintf(buffer, "%d BPM", beatAvg);
  else sprintf(buffer, "---");
  
  // Draw BPM on the top right
  int w = u8g2.getStrWidth(buffer);
  u8g2.drawStr(128 - w, 12, buffer);

  u8g2.sendBuffer();
}
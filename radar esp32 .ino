#include <ESP32Servo.h>                             // Servo library

#define TRIG_PIN   5                                // Ultrasonic trigger
#define ECHO_PIN   18                               // Ultrasonic echo
#define SERVO_PIN  13                               // Servo signal
#define BUZZER_PIN 14                               // Buzzer pin

#define MAX_DISTANCE 200                            // Max range cm
#define DETECT_RANGE 100                            // Detection range
#define PWM_CHANNEL  0                              // LEDC channel
#define PWM_FREQ     2000                           // Base buzz freq
#define PWM_RES      8                              // PWM resolution

Servo radarServo;                                   // Servo object
int angle = 0;                                      // Current angle
int sweepDir = 1;                                   // Sweep direction

long measureDistance() {                            // Distance function
  digitalWrite(TRIG_PIN, LOW);                      // Clear trigger
  delayMicroseconds(2);                             // Short pause
  digitalWrite(TRIG_PIN, HIGH);                     // Send pulse
  delayMicroseconds(10);                            // Pulse width
  digitalWrite(TRIG_PIN, LOW);                      // End pulse
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);   // Read echo
  if (duration == 0) return MAX_DISTANCE;           // Timeout fallback
  return duration * 0.034 / 2;                      // Convert to cm
}

void buzzByDistance(long dist) {                    // Buzzer function
  if (dist >= DETECT_RANGE) {                       // Out of range
    ledcWrite(PWM_CHANNEL, 0);                      // Buzzer off
    return;
  }
  int volume = map(dist, 0, DETECT_RANGE, 255, 20); // Map to volume
  int freq = map(dist, 0, DETECT_RANGE, 4000, 500); // Map to freq
  ledcChangeFrequency(PWM_CHANNEL, freq, PWM_RES);  // Set frequency
  ledcWrite(PWM_CHANNEL, volume);                   // Set duty cycle
  delay(80);                                        // Tone duration
  ledcWrite(PWM_CHANNEL, 0);                        // Brief silence
  delay(map(dist, 0, DETECT_RANGE, 50, 400));       // Gap by distance
}

void setup() {
  Serial.begin(115200);                             // Start serial
  pinMode(TRIG_PIN, OUTPUT);                        // Trig as output
  pinMode(ECHO_PIN, INPUT);                         // Echo as input
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);        // Setup LEDC
  ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);           // Attach buzzer
  radarServo.attach(SERVO_PIN);                     // Attach servo
  radarServo.write(0);                              // Start position
  delay(500);                                       // Settle time
}

void loop() {
  radarServo.write(angle);                          // Move servo
  delay(30);                                        // Settle servo

  long distance = measureDistance();                // Get distance

  Serial.print("ANGLE:");                           // Print label
  Serial.print(angle);                              // Print angle
  Serial.print(",DIST:");                           // Print label
  Serial.println(distance);                         // Print distance

  if (distance < DETECT_RANGE) {                    // Object found
    buzzByDistance(distance);                       // Buzz buzzer
  }

  angle += sweepDir;                                // Step angle
  if (angle >= 180) sweepDir = -1;                  // Reverse right
  if (angle <= 0)   sweepDir =  1;                  // Reverse left
}

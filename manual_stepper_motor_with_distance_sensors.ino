// controlling a stepper motor directly with just an H-bridge

// STEPPER MOTOR
#define STEP_SIZE_DEG 1.8

//bool enabled = false;
#define ENABLE_pin 11
bool enabled = false;

// H-BRIDGE
#define pin_ENA 3
#define pin_IN1 6
#define pin_IN2 7
#define pin_IN3 8
#define pin_IN4 9
#define pin_ENB 5
#define pin_ENA2 13
#define pin_ENB2 12


// Distanced Sensors

#define pin_trig_1 2
//#define pin_echo_1 4
#define pin_trig_2 1
//#define pin_echo_2 10
#define pin_echo 4

#define TOTAL_DISTANCE 80

#define pin_output_to_leader 10

float timing = 0.0;
float distance_1 = 0.0;

// float timing_2 = 0.0;
float distance_2 = 0.0;

float distance_sum = 0;
float prev_distance_sum = 0;
flaot prev_2_distance_sum = 0;

// ———————————————————————————————————————————————————————————————————————————————————
// MODE SELECTION

//#define SIN_DRIVE
//#define RUN_CONTINUOUSLY
#define RATCHET

// #ifdef RUN_CONTINUOUSLY
//   #define STEP_PERIOD 1
// #endif

#ifdef RATCHET
  #define NUM_POSITIONS 4
  int positions[4] = {0, 60, 120, 180};
  int pos_i = 0;
  int set_position = positions[pos_i];
  int position = 0;
#endif

// #ifdef SIN_DRIVE
//   #define FREQ 60
//   unsigned long lastTime = 0;
//   unsigned long thisTime = 0;

//   #define MICROSTEPS 4
//   const int STEPS_PER_REV = 360/ STEP_SIZE_DEG;
//   const int TOTAL_MICROSTEPS = STEPS_PER_REV * MICROSTEPS;

//   uint8_t sinRiseTable[MICROSTEPS];

//   int currentMicrostep = 0;
//   int targetMicrostep = 0;

//   //#define PROPORTIONAL_CONTROL
//   #define SMOOTH_VEL_CONTROL

//   #ifdef PROPORTIONAL_CONTROL
//     // proportional speed control
//     #define Kp 5
//     #define MAX_PERIOD 100000
//     #define MIN_PERIOD 250
//   #endif

//   #ifdef SMOOTH_VEL_CONTROL
//     // min period: 1000
//     // max period: 50000
//     #define MAX_PERIOD 500000
//     #define MIN_PERIOD 200
//     int period = MAX_PERIOD;
//     #define Kp 5
    
//   #endif

//   // #define NUM_POSITIONS 3
//   // float scale_factor = TOTAL_MICROSTEPS / 360.0;
//   // int setPositions[3] = {0, (int)(90.0 * scale_factor), (int)(180.0 * scale_factor)};
//   #define NUM_POSITIONS 4
//   int setPositions[4] = {0, 120, 240, 360};
//   int pos_i = 0;
// #endif

// ———————————————————————————————————————————————————————————————————————————————————
// SEQUENCE SELECTION

//#define BASIC_SEQUENCE
// #define BASIC_SEQUENCE

// #ifdef BASIC_SEQUENCE

   #define SEQUENCE_LENGTH 4
//   #ifdef SIN_DRIVE
//     #define SIN_RISING 1
//     #define SIN_FALLING 0
//     const bool sequence[4][6] = {
//       {SIN_FALLING,  1, 0, 0, 1, SIN_RISING},  // +A Fall -B Rise
//       {SIN_RISING,   0, 1, 0, 1, SIN_FALLING}, // -A Rise -B Fall
//       {SIN_FALLING,  0, 1, 1, 0, SIN_RISING},  // -A Fall +B Rise
//       {SIN_RISING,   1, 0, 1, 0, SIN_FALLING}, // +A Rise +B Fall
//     };

//   #else
    const bool sequence[4][4] = {
      {1, 0, 1, 0}, // A+   B+
      {1, 0, 0, 1}, // A+   B-
      {0, 1, 0, 1}, // A-   B-
      {0, 1, 1, 0}, // A-   B+
    };
//   #endif
// #endif

// #ifdef FULL_SEQUENCE
//   // needs support for PWM on the enable lines to smooth the transitions
//   #define SEQUENCE_LENGTH 8
//   const bool sequence[8][4] = {
//     {1, 0, 1, 0}, // A+   B+
//     {1, 0, 0, 0}, // A+   B0
//     {1, 0, 0, 1}, // A+   B-
//     {0, 0, 0, 1}, // A0   B-
//     {0, 1, 0, 1}, // A-   B-
//     {0, 1, 0, 0}, // A-   B0
//     {0, 1, 1, 0}, // A-   B+
//     {0, 0, 1, 0}  // A0   B+
//   };
// #endif

// ———————————————————————————————————————————————————————————————————————————————————
// INTERFACE

// // POT
// #define POT_PIN A0
// int potVal = 0;

// BUTTON
#define BUTTON_PIN 2
volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;

#define DEBOUNCE_MS 150 // 50ms debounce

#if defined(RATCHET) || defined(SIN_DRIVE)
  void handleButtonPress() {
    //Serial.println("button pressed!");
    unsigned long currentTime = millis();
    // ignore if last interrupt within deboucne window
    if (currentTime - lastInterruptTime > DEBOUNCE_MS) {
      buttonPressed = true;
      lastInterruptTime = currentTime;
    }
  }
#endif

// ———————————————————————————————————————————————————————————————————————————————————

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pin_ENA, OUTPUT);
  pinMode(pin_ENB, OUTPUT);
  pinMode(pin_IN1, OUTPUT);
  pinMode(pin_IN2, OUTPUT);
  pinMode(pin_IN3, OUTPUT);
  pinMode(pin_IN4, OUTPUT);

  pinMode(pin_ENA2, OUTPUT);
  pinMode(pin_ENB2, OUTPUT);

  pinMode(ENABLE_pin, INPUT);


  #if defined(RATCHET) || defined(RUN_CONTINUOUSLY)
    digitalWrite(pin_ENA, HIGH);
    digitalWrite(pin_ENA2, HIGH);
    digitalWrite(pin_ENB, HIGH);
    digitalWrite(pin_ENB2, HIGH);
  #endif

  #ifdef RATCHET
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);
  #endif

  // #ifdef SIN_DRIVE
  //   // compute sin table for a single arm
  //   for (int i = 0; i < MICROSTEPS; i++) {
  //     float x = (float)i / (MICROSTEPS - 1);
  //     float angle = x * (PI / 2.0);
  //     sinRiseTable[i] = (uint8_t)(255.0 * sin(angle));
  //   }

  //   pinMode(BUTTON_PIN, INPUT_PULLUP);
  //   attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);
  // #endif

  // distance sensor
  pinMode(pin_trig_1, OUTPUT);
  pinMode(pin_trig_2, OUTPUT);
  // pinMode(pin_echo_1, INPUT);
  // pinMode(pin_echo_2, INPUT);
  pinMode(pin_echo, INPUT);
  pinMode(pin_output_to_leader, OUTPUT);
}

// loop logic variables
int i = 0;
int j = 0;



void loop() {
  // digitalWrite(pin_ENA, 1);
  // digitalWrite(pin_ENA2, 1);
  // digitalWrite(pin_IN1, 1);
  // digitalWrite(pin_IN2, 0);
  // digitalWrite(pin_IN3, 1);
  // digitalWrite(pin_IN4, 0);
  // digitalWrite(pin_ENB, 1);
  // digitalWrite(pin_ENB2, 1);
  // while(1);
  //step_forward();
  //delay(500);
  // delay(5000);
  enabled = digitalRead(ENABLE_pin);
  if (enabled) {

    // #ifdef SIN_DRIVE
    //   thisTime = micros();

    //   #ifdef PROPORTIONAL_CONTROL
    //     int error = computeError();
    //     unsigned long period = computeMicrostepPeriod(abs(error));
    //     if (period && (thisTime - lastTime) >= period) {
    //       lastTime = thisTime;
    //       if (error > 0) microstep_forward();
    //       if (error < 0) microstep_backward();
    //     }
    //   #endif


    //   #ifdef SMOOTH_VEL_CONTROL
    //     int acceleration = 150;
    //     // define velocity as degrees/microstep / period
    //     // min period: 1000
    //     // max period: 50000
    //     // velocity = STEP_SIZE_DEG / MICROSTEPS / period;
    //     int error = computeError();
    //     if (error != 0) {
    //       unsigned long set_period = computeMicrostepPeriod(error);
    //       if (period > set_period) {
    //         period -= acceleration;
    //       } else if (period < set_period) {
    //         period += acceleration;
    //       }
    //       if ((thisTime - lastTime) >= abs(period)) {
    //         lastTime = thisTime;
    //         if (error > 0) microstep_forward();
    //         if (error < 0) microstep_backward();
    //       }
    //     } else {
    //       period = MAX_PERIOD;
    //     }
        
    //   #endif

    //   // update set position by cycling through loaded points
    //   if (buttonPressed) {
    //     buttonPressed = false;
    //     pos_i = ++pos_i % NUM_POSITIONS;
    //     targetMicrostep = setPositions[pos_i];
    //   }

    //   // debug
    //   Serial.println(currentMicrostep);
    // #endif

    #ifdef RATCHET
      // if button has rising edge 
      if (buttonPressed) {
        buttonPressed = false;
        Serial.println("Reload Triggered");
        // increment set position
        pos_i = ++pos_i % NUM_POSITIONS;
        // update set position
        set_position = positions[pos_i];
      }

      
      int error = set_position - position;
      // if (error > 360 / 2)
      //       error -= 360;
      //   if (error < -360 / 2)
      //       error += 360;

      // Serial.print("Current pos: ");
      // Serial.print(position);
      // Serial.print(", set pos: ");
      // Serial.print(set_position);
      // Serial.print(", error: ");
      // Serial.println(error);

      // if the position is farther from the set position by more than one step
      if (abs(error) >= STEP_SIZE_DEG) {
        // step forward if position is below set
        if (error < 0) step_backward();
        // step backward if position is above set
        else step_forward();
        // time for step
        delay(30);
      }
    #endif

    // #ifdef RUN_CONTINUOUSLY
    //   potVal = map(analogRead(POT_PIN), 0, 1023, 50, 1000);
    //   // put your main code here, to run repeatedly:
    //   delay(potVal);
    //   Serial.print("Sequence step: ");
    //   Serial.print(i);
    //   Serial.print(", with delay of: ");
    //   Serial.println(potVal);
    //   forward_step();
    // #endif
  }

  if (!enabled) {

    // distance sensors:
    digitalWrite(pin_trig_1, LOW);
    delay(2);
    digitalWrite(pin_trig_1, HIGH);
    delay(10);
    digitalWrite(pin_trig_1, LOW);
    
    timing = pulseIn(pin_echo, HIGH);
    distance_1 = (timing * 0.034) / 2;

    digitalWrite(pin_trig_2, LOW);
    delay(2);
    digitalWrite(pin_trig_2, HIGH);
    delay(10);
    digitalWrite(pin_trig_2, LOW);
    
    timing = pulseIn(pin_echo, HIGH);
    distance_2 = (timing * 0.034) / 2;


    Serial.print("Distance 1: ");
    Serial.print(distance_1);
    Serial.print("Distance 2: ");
    Serial.println(distance_2);

    prev_2_distance_sum = prev_distance_sum;
    prev_distance_sum = distance_sum;
    distance_sum = distance_1 + distance_2;


    if (distance_sum > 70 && distance_sum < 90) {
      if (prev_distance_sum > prev_2_distance_sum &&
          prev_distance_sum > distance_sum) {
        digitalWrite(pin_output_to_leader, HIGH);
        delay(200);
        digitalWrite(pin_output_to_leader, LOW);
      }  
    }
  }
}



// ———————————————————————————————————————————————————————————————————————————————————
// STEP MOTOR FUNCTIONS

void step_forward() {
  i = (i + 1) % SEQUENCE_LENGTH;
  const bool* output = sequence[i];
  digitalWrite(pin_IN1, output[0]);
  digitalWrite(pin_IN2, output[1]);
  digitalWrite(pin_IN3, output[2]);
  digitalWrite(pin_IN4, output[3]);

  #ifdef RATCHET
    position += STEP_SIZE_DEG;
    if (position > 360) {
      position -= 360;
    }
  #endif
}

void step_backward() {
  i = (i - 1 + SEQUENCE_LENGTH) % SEQUENCE_LENGTH;
  const bool* output = sequence[i];
  digitalWrite(pin_IN1, output[0]);
  digitalWrite(pin_IN2, output[1]);
  digitalWrite(pin_IN3, output[2]);
  digitalWrite(pin_IN4, output[3]);

  #ifdef RATCHET
    position -= STEP_SIZE_DEG;
    if (position < 0) {
      position += 360;
    }
  #endif
}

//   #ifdef SIN_DRIVE
//   void microstep_forward() {
//     // run after a full microstep cycle
//     if (++i >= MICROSTEPS) {
//       i = 0; 
//       j = (j + 1) % 4; // 4 phases of the sin wave
//     }
//     const bool* output = sequence[j];
//     digitalWrite(pin_IN1, output[1]);
//     digitalWrite(pin_IN2, output[2]);
//     digitalWrite(pin_IN3, output[3]);
//     digitalWrite(pin_IN4, output[4]);

//     int duty_A = sinRiseTable[output[0] ? i : MICROSTEPS - 1 - i];
//     int duty_B = sinRiseTable[output[5] ? i : MICROSTEPS - 1 - i];

//     analogWrite(pin_ENA, duty_A);
//     analogWrite(pin_ENA2, duty_A);
//     analogWrite(pin_ENB, duty_B);
//     analogWrite(pin_ENB2, duty_A);
    
//     currentMicrostep = (currentMicrostep + 1) % (int)TOTAL_MICROSTEPS;
//   }

//   void microstep_backward() {
//     // wrap i if it goes below 0
//     if (--i < 0) {
//         i = MICROSTEPS - 1;
//         j = (j - 1 + 4) % 4; // keep j above 0
//     }
//     const bool* output = sequence[j];
//     digitalWrite(pin_IN1, output[1]);
//     digitalWrite(pin_IN2, output[2]);
//     digitalWrite(pin_IN3, output[3]);
//     digitalWrite(pin_IN4, output[4]);

//     int duty_A = sinRiseTable[output[0] ? i : MICROSTEPS - 1 - i];
//     int duty_B = sinRiseTable[output[5] ? i : MICROSTEPS - 1 - i];

//     analogWrite(pin_ENA, duty_A);
//     analogWrite(pin_ENA2, duty_A);
//     analogWrite(pin_ENB, duty_B);
//     analogWrite(pin_ENB2, duty_A);

//     currentMicrostep = (currentMicrostep - 1 + (int)TOTAL_MICROSTEPS) % (int)TOTAL_MICROSTEPS;
//   }

//   int computeError() {
//     int error = targetMicrostep - currentMicrostep;
//     if (error > TOTAL_MICROSTEPS / 2)
//         error -= TOTAL_MICROSTEPS;
//     if (error < -TOTAL_MICROSTEPS / 2)
//         error += TOTAL_MICROSTEPS;
//     return error;
//   }

//   // proportional control
//   unsigned long computeMicrostepPeriod(int error) {
//     // int error = abs(targetMicrostep - currentMicrostep);
//     if (error == 0) return 0;
//     float period = 1000000.0f / (error * Kp);
//     if (abs(period) > MAX_PERIOD) period = (period > 0) ? MAX_PERIOD : - MAX_PERIOD;
//     if (abs(period) < MIN_PERIOD) period = (period > 0) ? MIN_PERIOD : - MIN_PERIOD;
//     return (unsigned long)period;
//   }

// #endif

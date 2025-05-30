#include <ros.h>

/*************************************************** 
Servo Tester, code modified by Treggon Owens from
Default adafruit code.

Making them all move at once from ROS
****************************************************/


#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Servo.h>
#include <ros.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/String.h>

// This maintains the servo's desired state
int servoDriveArray[16];

// This maintains the servo's current drive state
int servoStateArray[16];

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define USMIN 450              // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2450             // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define RECEIVER_ON_FREQ 1000  // receiver should be producing >1000 if on
#define SERVO_FREQ 60          // Analog servos run at ~50 Hz updates
#define SERVO_MIDPOINT 1500    // mechanically where the center is for the servo

#define DELTA_FREQ 60  // the servo's frequency (default is 60)

// our servo # counter
uint8_t servonum = 0;
uint16_t microsec = SERVO_MIDPOINT;
uint16_t del = 1 / DELTA_FREQ + 1;
int direction = 1;

// Our safety variable
// if this is true, enable servos
// gets set below in 'HandleSafety()'
// which should link to the 'button_publisher
bool currentlysafe = false;

// This handles the arming state of the robot's servos
bool armed = false;

// Ros node handler and messages
ros::NodeHandle nh;
std_msgs::String button_msg;
std_msgs::UInt16 servo0_msg;

// I am using this to do the 'Arming Sequence'
// Until a button is pressed, its enable state is
ros::Publisher safety_button_publisher("safety_button_publisher", &button_msg);

// This handles if there were servo changes detected
bool changesdetected = false;



// Main Loop here
// Process
// 0) Arming and safety handler
// 1) See if we were told to go to new servo levels
// Do that
// 2) Check Sonic Sensors
// Do that
// 3) Arming state logic
// 4) ROS handler and spinonce

void servocallback(const std_msgs::UInt16& servomsg) {
  if (servomsg.data == 1) {
    // digitalWrite(LED, HIGH);
  } else {
    //  digitalWrite(LED, LOW);
  }
}

ros::Subscriber<std_msgs::UInt16> servo_subscriber("servo0_msg", &servocallback);

void setuppwm() {
  pwm.begin();
  /* (From Adafruit notes below....)
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50/60 Hz updates

  for (int servonumber = 0; servonumber < 16; servonumber++) {
    pwm.writeMicroseconds(servonumber, SERVO_MIDPOINT);
  }
}

void setup() {

  setuppwm();
  setalltomicrosec(SERVO_MIDPOINT);  // start with servo midpoint

  nh.initNode();
  delay(10);
}

void setalltomicrosec(uint16_t usec) {

  for (int servonumber = 0; servonumber < 16; servonumber++) {
    servoDriveArray[servonumber] = microsec;
    servoStateArray[servonumber] = microsec;
    pwm.writeMicroseconds(servonumber, usec);
  }
}

void setservomicrosec(uint16_t usec, int servonumber) {
  servoDriveArray[servonumber] = usec;
}

bool checkservochange() {

  bool returnbool = false;

  for (int servonumber = 0; servonumber < 16; servonumber++) {
    if (servoDriveArray[servonumber] != servoStateArray[servonumber]) {
      returnbool = true;
    }
  }
}

void sweepservos() {
  if (microsec > USMAX) {
    direction = -1;
    //setallservomidpoint();
  } else if (microsec < USMIN) {
    direction = 1;
  }

  if (direction > 0) {
    microsec = microsec + (DELTA_FREQ);
  } else {
    microsec = microsec - (DELTA_FREQ);
  }
}

bool handlesafety() {

  bool safe = false;



  return safe;
}

void handlearming() {
  if (currentlysafe == true) {
    // arm the system
  }
}

void loop() {

  // Servo Level code, see if ROS told us to go to new levels
  if (changesdetected) {
    changesdetected = false;
    currentlysafe = handlesafety();
    handlearming();
  } else {
    changesdetected = checkservochange();
  }

  //setalltomicrosec(microsec);
  nh.spinOnce();
  delay(del);
}

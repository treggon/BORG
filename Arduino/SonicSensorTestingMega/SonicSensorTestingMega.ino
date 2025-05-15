#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

#define TRIGGER_PIN 9
#define PWM_OUTPUT_PIN 10

long duration;
float distance;
ros::NodeHandle  nh;

sensor_msgs::Range range_msg;
ros::Publisher pub_range( "/ultrasound", &range_msg);

const int adc_pin = 0;

char frameid[] = "/ultrasound";

const int averages = 10;
int averagedistanceint = 0;
double averagedistancedouble = 0.0;
double totalDistances[averages];

float getRange_Ultrasound(int pin_num){
  int val = 0;
  for(int i=0; i<4; i++) val += analogRead(pin_num);
  float range =  val;
  return range /322.519685;   // (0.0124023437 /4) ; //cvt to meters
}

void averagedistance(double range)
{

  for(int i=averages-1; i>0; i--)
  {
    totalDistances[i] = totalDistances[i-1];
  }
  totalDistances[0] = range;
  for(int i=0; i<averages;i++)
  {
    averagedistancedouble += totalDistances[i];
  }
  averagedistancedouble = averagedistancedouble / averages;
  averagedistanceint = (int)averagedistancedouble;
}

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(PWM_OUTPUT_PIN, INPUT);
  Serial.println("System Start");
}

void readduration()
{
  // The sensor is triggered by a falling edge of a HIGH pulse that 
  // is more than 60 microseconds in duration.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(100);
  digitalWrite(TRIGGER_PIN, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  // If no object detected, fixed pulse width of 35ms is sent
  // by the sensor.
  pinMode(PWM_OUTPUT_PIN, INPUT);
  duration = pulseIn(PWM_OUTPUT_PIN, HIGH);
 
  // Convert the pulse width duration into a distance
  distance = duration;
  distance = distance * 10 / 58;
  
}

void loop()
{

  readduration();
  averagedistance(distance);
  Serial.print(averagedistanceint);
  Serial.println(" mm");
  
  delay(2);
}
/* This code was developed as part of a group robotics project using an R3 Arduino-compatible board, 4 DC motors, and various sensors including a color sensor and distance 
sensor. The goal was to build an autonomous vehicle capable of navigating an environment, identifying objects by color, and interacting with mission 
elements using a motorized arm.
*/

#include "Enes100.h"
#include <math.h>
#include "TCS34725.h"

// TCS34725 tcs;

int E1 = 5;
int E2 = 6;
int M1 = 4;
int M2 = 7;
float tolerancePos = 0.07;
float toleranceAngle = 0.03;

/* The code inside void setup() runs only once, before the code in void loop(). */
void setup(){

  pinMode(8, OUTPUT);
  pinMode(9, INPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);

  delay(3000);
	Enes100.begin("Green Machine", SEED, 7, 8, 9);
	delay(3000);

  Wire.begin();
  if (!tcs.attach(Wire)){
  	Serial.println("ERROR: TCS34725 NOT FOUND !!!");
  }
  tcs.integrationTime(33); // ms
  tcs.gain(TCS34725::Gain::X01);

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(19200);

  delay(1000);
}

void loop(){
  local dist = 100;

	Enes100.updateLocation();
	while(Enes100.location.x < 3){
		setYLocation(1.4);
		setTheta(0);

		while(isWall(dist)){
			changeYLocation(-2);
			setTheta(0);
		}

		driveToWall(dist);
	}

	driveToWall(dist);
	stopMotors();
}

void printColorSensor(){
  if (tcs.available()) { // if current measurement has done
    TCS34725::Color color = tcs.color();
    Serial.print("Color Temp : "); Serial.println(tcs.colorTemperature());
    Serial.print("Lux        : "); Serial.println(tcs.lux());
    Serial.print("R          : "); Serial.println(color.r);
    Serial.print("G          : "); Serial.println(color.g);
    Serial.print("B          : "); Serial.println(color.b);
    delay(1000);
  }
}

void goToMissionSite(){
  Enes100.updateLocation();

  if (Enes100.getY() < 1){
    setYLocation(1.5);
  } else{
    setYLocation(0.5);
  }
}

void setTheta(float theta){
	Enes100.updateLocation();

	while(Enes100.location.theta < (theta - toleranceAngle) || Enes100.location.theta > (theta + toleranceAngle)){
		turnRight(100);
		Enes100.updateLocation();
	}

	stopMotors();
}

void setYLocation(float y){
	Enes100.updateLocation();

	if (y > Enes100.location.y){
		setTheta(1.57);
	} else{
		setTheta(-1.57);
	}

	while(Enes100.location.y > (y + tolerancePos) || Enes100.location.y < (y - tolerancePos)){
		setBothMotors(150);
		Enes100.updateLocation();
	}

	stopMotors();
}

void changeYLocation(float amount){
	Enes100.updateLocation();
	setYLocation(Enes100.location.y + amount);
}

void driveToWall(float distance){

	while(!isWall(distance)){
		setBothMotors(100);
	}

	stopMotors();
}

int readDistanceSensor(){
  return analogRead(A0);
}

void setLeftMotorPWM(int speed, int dir){

  if (dir == 1){
    digitalWrite(M2, HIGH);
  } else{
    digitalWrite(M2, LOW);
  }

  analogWrite(E2, speed);
}

void setRightMotorPWM(int speed, int dir){
  if (dir == 1){
    digitalWrite(M1, HIGH);
  } else{
    digitalWrite(M1, LOW);
  }

  analogWrite(E1, speed);
}

bool isWall(float distance){
	return (readDistanceSensor() > 0 && readDistanceSensor() < distance);
}

void setBothMotors(int speed){
	setLeftMotorPWM(speed, 0);
	setRightMotorPWM(speed, 0);
}

void turnRight(int speed){
	setLeftMotorPWM(speed, 0);
	setRightMotorPWM(speed, 1);
}

void turnLeft(int speed){
	setLeftMotorPWM(speed, 1);
	setRightMotorPWM(speed, 0);
}

void stopMotors(){
	setLeftMotorPWM(0, 1);
	setRightMotorPWM(0, 1);
}

void readDistance(){
  Enes100.println(analogRead(A0));
  delay(300);
}

void extendArm(int milliseconds){
  digitalWrite(2, LOW);
  digitalWrite(4, HIGH);
  delay(milliseconds);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  delay(500);
}

void retractArm(int milliseconds){
  digitalWrite(2, HIGH);
  digitalWrite(4, LOW);
  delay(milliseconds);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  delay(500);
}

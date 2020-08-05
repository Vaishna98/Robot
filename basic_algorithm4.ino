
// Arduino Obstacle Avoiding Robot
// Code adapted from http://www.educ8s.tv
// First Include the NewPing and Servo Libraries
/****************************************/

#include <AFMotor.h>
#include <Servo.h>

#include<SoftwareSerial.h>
//SoftwareSerial SUART(15, 14); //2,3SRX=Dpin-2; STX-DPin-3

//Servo motor configurations
int angle1 = 90;


AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
//AF_DCMotor motor3(3);
//AF_DCMotor motor4(4);
Servo servo1;

// For Ultrasonic ans IR Sensor GPIO Configuration

//Ultrasonic GPIO
const int trigP = A0;
const int echoP = A1;
//const int MAX_DISTANCE= 200

//IR GPIO
const int ProxSensor = A2;
int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE ON BOTTOM SIDE

bool USObstacle = false; //ultrasonic sensor obstacle
bool IRObstacle = false; //bottom infrared sensor obstacle

long duration;
//int distance;


//boolean goesForward = false;
int distance = 100;
int speedSet = 0;

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;
unsigned long t1;
unsigned long t2;
unsigned long pulse_width;
float cm;
float inches;

String input = ""; //Serial input

enum Directions { Forward, Backward, TurnLeft, TurnRight, Brake};

Directions nextStep = Forward;
String lastStep, currentStep;

bool automode_Flg = false;
bool manualmode_Flg = true;
/****************************************/

void setup() {
  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT);
  pinMode(ProxSensor, INPUT);

  servo1.attach(9);
  servo1.write(115);
  delay(2000);
  //  distance = readPing();
  //  delay(100);
  //  distance = readPing();
  //  delay(100);
  //  distance = readPing();
  //  delay(100);
  //  distance = readPing();
  //  delay(100);

  motor1.setSpeed(100);
  motor2.setSpeed(100);
  //motor3.setSpeed(200);
  //motor4.setSpeed(200);
  pinMode(13, OUTPUT);

  Serial.begin(9600);      // Open serial channel at 9600 baud rate

  Serial3.begin(9600);//for 2560
  
  Serial.flush();
  automode_Flg=true;
}

void loop() {
  int distanceR =  0;
  int distanceL =  0;
  int distanceM =  0;
  IRObstacle = false;
  USObstacle = false;
  String input = "";
//  delay(40);
  
 
while (Serial3.available())
  {
    delay(3);//3
    if (Serial3.available() > 0)
    {
      char x = Serial3.read();  //read character
      input += x;
//      Serial.println(input);
//      Serial.println(n);        //show character on Serial Monitor
    }
  }//while
  
  if (input.indexOf("$M") != -1)
  {
    automode_Flg = false;  //for Manual mode
    manualmode_Flg = true;
    Serial.print("Manual");
  }

  else if (input.indexOf("$A") != -1) {
    automode_Flg = true;  //for Auto mode
    manualmode_Flg = false;
    Serial.print("Auto");
  }
  
  if (automode_Flg == true)
  {
    turnOnVaccume();
    checkDistance();
    checkDirection();
    drive();
  }//end of auto mode

  else if (manualmode_Flg == true)
  {
    if (input.indexOf("$S") != -1)
    { Serial.print("s");
      moveStop();
      delay(5000);
    }

    if (input.indexOf("$F") != -1)
    { Serial.print("f");
      moveForward();
      delay(2000);
      moveStop();
      delay(5000);

    }
    if (input.indexOf("$B") != -1)
    { Serial.print("b");
      moveBackward();
      delay(2000);
      moveStop();
      delay(5000);
    }
    if (input.indexOf("$R") != -1)
    { Serial.print("r");
      turnRight();
      delay(2000);
      moveStop();
      delay(5000);

    }
    if (input.indexOf("$L") != -1)
    { Serial.print("l");
      turnLeft();
      delay(2000);
      moveStop();
      delay(5000);

    }
    
  }//end of manual mode
  
}//loop

int lookRight()
{
  servo1.write(50);
  delay(500);
  //  int distance = readPing();
  checkDistance();
  delay(100);
  servo1.write(115);
  return distance;
}

int lookLeft()
{
  servo1.write(170);
  delay(500);
  //  int distance = readPing();
  checkDistance();
  delay(100);
  servo1.write(115);
  return distance;
  delay(100);
}

void moveStop()
{
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  //  motor3.run(RELEASE);
  //  motor4.run(RELEASE);
}

void moveForward()
{
  motor1.setSpeed(90);//100
  motor2.setSpeed(90);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

void moveBackward()
{
  motor1.setSpeed(90);//100
  motor2.setSpeed(90);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
}


void turnRight()
{
  motor1.setSpeed(100);//200
  motor2.setSpeed(100);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  delay(450);//300
}

void turnLeft()
{
  motor1.setSpeed(100);//200
  motor2.setSpeed(100);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  delay(450);//300
}


void checkDistance()
{

  // Hold the trigger pin high for at least 10 us
  digitalWrite(trigP, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigP, LOW);

  // Wait for pulse on echo pin
  while ( digitalRead(echoP) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  t1 = micros();
  while ( digitalRead(echoP) == 1);
  t2 = micros();
  pulse_width = t2 - t1;

  // Calculate distance in centimeters and inches. The constants
  // are found in the datasheet, and calculated from the assumed speed
  //of sound in air at sea level (~340 m/s).
  cm = pulse_width / 58.0;
  inches = pulse_width / 148.0;
  //  Serial.println(pulse_width);

  //IR digital read
  isObstacle = digitalRead(ProxSensor);

  // Print out results
  if ( pulse_width > MAX_DIST ) {
    Serial.println("Out of range");
  } else {
    Serial.print(cm);
    Serial.print(" cm \t");
    //    Serial.print(inches);
    //    Serial.println(" in");
  }

  // Wait at least 60ms before next measurement
  delay(60);

  if (cm <= 30)
  { //30
    USObstacle = true;
    Serial.println("Problem Ahead");
  }
  else
  {
    USObstacle = false;
  }

  //IR if-else statements
  if (isObstacle == HIGH )
  {
    IRObstacle = true;    //        Serial.println("Obstacle on bottom side");
  }
  else
  {
    IRObstacle = false;
  }

  if (IRObstacle == false && USObstacle == false)
  {
    digitalWrite(13, LOW); // set the LED off
  }  //declared obstacle detection
  else   if ((IRObstacle == true) || USObstacle == true)
  {
    digitalWrite(13, HIGH);   // set the LED on
  }
}


void checkDirection()
{
  Serial.println("checking direction");

  //the while loop will only exit when there's no obstacle detected by the sensors
  while ((IRObstacle == true ) || USObstacle == true)
  {
    Serial.println("Obstacle detected.");
    digitalWrite(13, HIGH);   // set the LED on

    //    if (IRObstacle == true && USObstacle == false)
    //    {
    //      //        currentStep = "TurnLeft";
    //
    //      nextStep = Brake;
    //      drive();
    //      delay(300);
    //      nextStep = Backward;
    //      drive();
    //      delay(1000);
    //      checkDistance(); //check if there's still an obstacle ahead
    //    }

    if (lastStep == "TurnRight")
    {
      currentStep = "TurnLeft";

      nextStep = Brake;
      drive();
      delay(300);
      nextStep = TurnLeft;//TurnLeft
      drive();
      delay(400);//200
      nextStep = Forward;//TurnLeft
      drive();
      delay(1200);//500
      nextStep = Brake;
      drive();
      delay(400);//100
      nextStep = TurnLeft;//TurnLeft
      //    currentStep="TurnRight";
      drive();
      delay(250);//200
    }
    else
    {
      currentStep = "TurnRight";

      nextStep = Brake;
      drive();
      delay(300);
      nextStep = TurnRight;//TurnLeft
      drive();
      delay(400);//200
      nextStep = Forward;//forward
      drive();
      delay(1200);//500
      nextStep = Brake;
      drive();
      delay(400);//100
      nextStep = TurnRight;//TurnLeft
      //    currentStep="TurnRight";
      drive();
      delay(250);//200
    }
    /*
      nextStep = Brake;
      drive();
      delay(300);
      nextStep = TurnRight;//TurnLeft
      drive();
      delay(200);
      nextStep = Brake;
      drive();
      delay(100);
      nextStep = TurnRight;//TurnLeft
      currentStep="TurnRight";
      drive();
      delay(200);
    */
    checkDistance(); //check if there's still an obstacle ahead

    lastStep = currentStep;

  }
  nextStep = Forward;
  drive();
}

void drive()
{
  switch (nextStep)
  {
    case Forward:
      moveForward();
      Serial.println("Forward");
      break;

    case Backward:
      moveBackward();
      Serial.println("Backward");
      break;

    case TurnLeft:
      turnLeft();
      Serial.println(" TurnLeft");
      break;

    case TurnRight:
      turnRight();
      Serial.println(" TurnRight");
      break;

    case Brake:
      moveStop();
      Serial.println(" Stopped");
      break;
  }
}

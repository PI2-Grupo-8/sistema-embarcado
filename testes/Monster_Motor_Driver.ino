// This work is based on Tutorial for Monster Motor Shield VNH2SP30:
// https://www.instructables.com/Monster-Motor-Shield-VNH2SP30/

#include <Wire.h>

#define BRAKE 0
#define CW 1
#define CCW 2

//MOTOR
#define MOTOR_A_PIN 23
#define MOTOR_B_PIN 0

#define PWM_MOTOR 12

short usSpeed = 150; //default motor speed
unsigned short usMotor_Status = BRAKE;

void setup()
{
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);
  pinMode(PWM_MOTOR, OUTPUT);

  Serial.begin(9600); // Initiates the serial to do the monitoring
  Serial.println("Begin motor control");
  Serial.println(); //Print function list for user selection
  Serial.println("Enter number for control option:");
  Serial.println("1. STOP");
  Serial.println("2. FORWARD");
  Serial.println("3. REVERSE");
  Serial.println("4. READ CURRENT");
  Serial.println("+. INCREASE SPEED");
  Serial.println("-. DECREASE SPEED");
  Serial.println();
}

void loop()
{
  char user_input;

  while (Serial.available())
  {
    user_input = Serial.read(); //Read user input and trigger appropriate function

    if (user_input == '1')
    {
      Stop();
    }
    else if (user_input == '2')
    {
      Forward();
    }
    else if (user_input == '3')
    {
      Reverse();
    }
    else if (user_input == '+')
    {
      IncreaseSpeed();
    }
    else if (user_input == '-')
    {
      DecreaseSpeed();
    }
    else
    {
      Serial.println("Invalid option entered.");
    }
  }
}

void Stop()
{
  Serial.println("Stop");
  usMotor_Status = BRAKE;
  motorGo(usMotor_Status, 0);
}

void Forward()
{
  Serial.println("Forward");
  usMotor_Status = CW;
  motorGo(usMotor_Status, usSpeed);
}

void Reverse()
{
  Serial.println("Reverse");
  usMotor_Status = CCW;
  motorGo(usMotor_Status, usSpeed);
}

void IncreaseSpeed()
{
  usSpeed = usSpeed + 10;
  if (usSpeed > 255)
  {
    usSpeed = 255;
  }

  Serial.print("Speed +: ");
  Serial.println(usSpeed);

  motorGo(usMotor_Status, usSpeed);
}

void DecreaseSpeed()
{
  usSpeed = usSpeed - 10;
  if (usSpeed < 0)
  {
    usSpeed = 0;
  }

  Serial.print("Speed -: ");
  Serial.println(usSpeed);

  motorGo(usMotor_Status, usSpeed);
}

void motorGo(uint8_t direct, uint8_t pwm) //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
  if (direct == CW)
  {
    digitalWrite(MOTOR_A_PIN, LOW);
    digitalWrite(MOTOR_B_PIN, HIGH);
  }
  else if (direct == CCW)
  {
    digitalWrite(MOTOR_A_PIN, HIGH);
    digitalWrite(MOTOR_B_PIN, LOW);
  }
  else
  {
    digitalWrite(MOTOR_A_PIN, LOW);
    digitalWrite(MOTOR_B_PIN, LOW);
  }

  analogWrite(PWM_MOTOR, pwm);
}

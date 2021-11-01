#include <Wire.h>
#include <HCSR04.h>
#include <heltec.h>
#include <MPU6050_light.h>

// Estados do driver
#define BRAKE 0
#define CW 1
#define CCW 2

// Driver motores esquerdo (E)
#define DRIVER_E_CW 23
#define DRIVER_E_CCW 0
#define DRIVER_E_PWM 12
short motorESpeed = 200; // Velocidade padrao
unsigned short motorEStatus = BRAKE;
char MOTOR_E = 'E';

//Driver motores direito (D)
#define DRIVER_D_CW 17
#define DRIVER_D_CCW 25
#define DRIVER_D_PWM 13
short motorDSpeed = 200; // Velocidade padrao
unsigned short motorDStatus = BRAKE;
char MOTOR_D = 'D';

// PWM
int FREQ = 5000; // Frequencia de 5kHz
int RES = 8;     // Resolucao de 8bits

// LDC1101
#define LDC_CS 15
#define LDC_PWM 16
#define LDC_SCK 4
#define LDC_SDI 1
#define LDC_SDO1 3
#define LDC_SDO2 34
#define LDC_SDO3 35
#define LDC_SDO4 32

// MPU6050
MPU6050 mpu(Wire);
int xAngle = 0;
int yAngle = 0;
int zAngle = 0;

// Dados da plantacao (exemplo)
int N_FILEIRAS = 40;     // Numero de fileiras
float COMP_FILEIRA = 50; // Comprimento da fileira (m)
float ESP_FILEIRA = 0.8; // Espacamento entre fileiras (m)

int fileiraAtual = 1;  // Fileira atual
float distTotal = 0;   // Distancia total percorrida

// Sensores ultrassonicos
HCSR04 hc(2,new int[4]{36,37,38,39},4);
float distE1 = 0;
float distE2 = 0;
float distD1 = 0;
float distD2 = 0;

void setup() {
  // LoRa Heltec
  Heltec.begin(false, true, true, true, 915E6);
  
  // Drivers
  pinMode(DRIVER_E_CW, OUTPUT);
  pinMode(DRIVER_E_CCW, OUTPUT);
  pinMode(DRIVER_E_PWM, OUTPUT);
  pinMode(DRIVER_D_CW, OUTPUT);
  pinMode(DRIVER_D_CCW, OUTPUT);
  pinMode(DRIVER_D_PWM, OUTPUT);
  ledcAttachPin(DRIVER_E_PWM, 0);
  ledcSetup(0, FREQ, RES);
  ledcAttachPin(DRIVER_D_PWM, 1);
  ledcSetup(1, FREQ, RES);
  Reverse(MOTOR_E);
  Forward(MOTOR_D);

  // LDC1101
  pinMode(LDC_CS, OUTPUT);
  pinMode(LDC_PWM, OUTPUT);
  pinMode(LDC_SCK, OUTPUT);
  pinMode(LDC_SDI, OUTPUT);
  pinMode(LDC_SDO1, INPUT);
  pinMode(LDC_SDO2, INPUT);
  pinMode(LDC_SDO3, INPUT);
  pinMode(LDC_SDO4, INPUT);

  // MPU6050
  Wire.begin();
  mpu.begin();
  mpu.calcGyroOffsets();
}

void loop() {
  // Medicao de angulo
  mpu.update();
  xAngle = int(mpu.getAngleX());
  yAngle = int(mpu.getAngleY());
  zAngle = int(mpu.getAngleZ());
  
  // Medicao da distancia entre as rodas e o canteiro
  distE1 = hc.dist(0);
  distE2 = hc.dist(1);
  distD1 = hc.dist(2);
  distD2 = hc.dist(3);

  // Possiveis ajustes de direcao
  if (distE1 < 5 || distD2 < 5 || zAngle < -30)
  {
    DecreaseSpeed(MOTOR_E);
    delay(1000);
    IncreaseSpeed(MOTOR_E);
  }
  if (distD1 < 5 || distE2 < 5 || zAngle > 30)
  {
    DecreaseSpeed(MOTOR_D);
    delay(1000);
    IncreaseSpeed(MOTOR_D);
  }

  // Possivel parada
  if(xAngle > 30 || xAngle < -30 || yAngle > 30 || yAngle < -30)
  {
    Stop();
  }

  // Medicao LDC1101
  distTotal = ReadDist();

  // Troca de fileira
  if (distTotal >= (fileiraAtual*COMP_FILEIRA + (fileiraAtual-1)*ESP_FILEIRA))
  {
    Stop();
    // Recebe dados restantes
    LoRa.beginPacket();
    LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print("Bateria: ");
    //LoRa.println(qtdBateria);
    LoRa.print("Fertilizante: ");
    //LoRa.println(qtdFertilizante);
    LoRa.print("Distancia: ");
    LoRa.println(distTotal);
    LoRa.print("Temperatura: ");
    //LoRa.println(temperatura);
    LoRa.endPacket();

    // Espera receber o sinal
    // ...
    // Recebe o sinal
    Reverse(MOTOR_E);
    Forward(MOTOR_D);
    while(distTotal < fileiraAtual*(COMP_FILEIRA+ESP_FILEIRA))
    {
      distTotal = ReadDist();
    }
    Stop();
    // Espera receber o sinal
    // ...
    // Recebe o sinal
    fileiraAtual++;
    if (fileiraAtual%2 == 0)
    {
      Forward(MOTOR_E);
      Reverse(MOTOR_D);
    } else {
      Reverse(MOTOR_E);
      Forward(MOTOR_D);
    }
  }
}

// Parar todos os motores
void Stop()
{
  motorEStatus = BRAKE;
  motorDStatus = BRAKE;
  motorGo(MOTOR_E, motorEStatus, 0);
  motorGo(MOTOR_D, motorDStatus, 0);
}

// Sentido horario para um par de motores
void Forward(char motor)
{
  if (motor == 'E')
  {
    motorEStatus = CW;
    motorGo(motor, motorEStatus, motorESpeed);
  }

  if (motor == 'D')
  {
    motorDStatus = CW;
    motorGo(motor, motorDStatus, motorDSpeed);
  }
}

// Sentido anti-horario para um par de motores
void Reverse(char motor)
{
  if (motor == 'E')
  {
    motorEStatus = CCW;
    motorGo(motor, motorEStatus, motorESpeed);
  }

  if (motor == 'D')
  {
    motorDStatus = CCW;
    motorGo(motor, motorDStatus, motorDSpeed);
  }
}

// Aumentar velocidade de um par de motores
void IncreaseSpeed(char motor)
{
  if (motor == 'E')
  {
    motorESpeed = 200;
    motorGo(motor, motorEStatus, motorESpeed);
  }

    if (motor == 'D')
  {
    motorDSpeed = 200;
    motorGo(motor, motorDStatus, motorDSpeed);
  }
}

// Diminuir velocidade de um par de motores
void DecreaseSpeed(char motor)
{
  if (motor == 'E')
  {
    motorESpeed = 150;
    motorGo(motor, motorEStatus, motorESpeed);
  }

  if (motor == 'D')
  {
    motorDSpeed = 150;
    motorGo(motor, motorDStatus, motorDSpeed);
  }
}

// Alterar direcao e velocidade de um par de motores
void motorGo(char motor, uint8_t direct, uint8_t pwm) //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
  if (motor == 'E')
  {
    if (direct == CW)
    {
      digitalWrite(DRIVER_E_CW, HIGH);
      digitalWrite(DRIVER_E_CCW, LOW);
    }
    else if (direct == CCW)
    {
      digitalWrite(DRIVER_E_CW, LOW);
      digitalWrite(DRIVER_E_CCW, HIGH);
    }
    else
    {
      digitalWrite(DRIVER_E_CW, LOW);
      digitalWrite(DRIVER_E_CCW, LOW);
    }

    ledcWrite(DRIVER_E_PWM, pwm);
  }

  if (motor == 'D')
  {
    if (direct == CW)
    {
      digitalWrite(DRIVER_D_CW, HIGH);
      digitalWrite(DRIVER_D_CCW, LOW);
    }
    else if (direct == CCW)
    {
      digitalWrite(DRIVER_D_CW, LOW);
      digitalWrite(DRIVER_D_CCW, HIGH);
    }
    else
    {
      digitalWrite(DRIVER_D_CW, LOW);
      digitalWrite(DRIVER_D_CCW, LOW);
    }
    
    ledcWrite(DRIVER_D_PWM, pwm);
  }
}

float ReadDist()
{
  float ldcE1 = analogRead(LDC_SDO1);
  float ldcE2 = analogRead(LDC_SDO2);
  float ldcD1 = analogRead(LDC_SDO3);
  float ldcD2 = analogRead(LDC_SDO4);
  float distancia = (ldcE1 + ldcE2 + ldcD1 + ldcD2)/4;
  return distancia;
}

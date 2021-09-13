#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050_light.h>
 
Adafruit_SSD1306 display(128, 64, &Wire);
 
MPU6050 mpu(Wire);
unsigned long timer = 0;
 
void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { 
    Serial.println(F("Erro na identificacao do display!"));
    for(;;);
  } 
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();                         
  Wire.begin();
  mpu.begin();
  display.println(F("Calibrando o MPU6050"));
  display.display();        
  mpu.calcGyroOffsets();
}
 
void loop() {
  mpu.update();  
  if((millis()-timer)>10)
  {                                           
    display.clearDisplay();
    display.setCursor(0,0);                         
    display.println("Angulo: ");
    display.print(int(mpu.getAngleZ()));
    display.display();
    timer = millis();  
  }
}

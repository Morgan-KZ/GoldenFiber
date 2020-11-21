#include "max6675.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//Термодатчик MAX6675
int thermoDO = 2;    // SO
int thermoCS = 3;    // CS
int thermoCLK = 4;   // SCK
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int real_temperature = 0;
// Конец Термодатчик

#define SSR_Relay 5 // SSR_Relay

#define Line_Sensor_RPM 7 // Line_Sensor_RPM
#define PIN_STEP 8        // Dispenser Out
#define PIN_DIR 9         // Dispenser Out
#define STEP_ROUND 200    // количество шагов на 1 оборот

LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей
const int button_up=A1;
const int button_ok=A2;
const int button_down=A3;
boolean buttonState_UP;
boolean buttonState_OK;
boolean buttonState_DOWN;
int a=0, b=0, c=0;
int temperature_value = 300;
int rotation_value = 900;
int real_rotation = 0;
int dispenser_value = 10;
int cooler_value = 80;
unsigned long previousMillisForRotation = 0;
unsigned long previousMillisForDispenser = 0; 
unsigned long previousMillisForTempSensor = 0;
volatile int pulses = 0;

void counter(){
  pulses++;
}

void setup(){
  //RPM Begin
  pinMode(Line_Sensor_RPM, INPUT);
  attachInterrupt(0, counter, FALLING);
  //RPM End
  pinMode(button_up, INPUT_PULLUP);
  pinMode(button_ok, INPUT_PULLUP);
  pinMode(button_down, INPUT_PULLUP);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  digitalWrite(PIN_DIR, HIGH);
  pinMode(SSR_Relay, OUTPUT);
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея 
}

void control_temp();
void control_dispenser();

void loop(){   
  while(a==0 && b==0){
    lcd.setCursor(2, 0);                                  //Главная страница
    lcd.print("Golden Fiber");   
    lcd.setCursor(0, 1);         
    lcd.print("T=");
    lcd.print(real_temperature);
    lcd.setCursor(6, 1);
    lcd.print("R=");
    lcd.print(real_rotation);
    lcd.setCursor(12, 1);
    lcd.print("D=");
    lcd.print(dispenser_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      a=1;    
    }
    //Control Temp
    control_temp();
    //RPM-Begin
    if(millis() - previousMillisForRotation >= 30000){
      previousMillisForRotation = millis();      
      real_rotation = pulses * 2;      
      pulses=0;
    }
    //RPM-End
    //Control Dispenser
    control_dispenser();
  } 
  while (a==1 && b==0){ //Level Up
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("Level UP");
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      a=0;
      b=0;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      lcd.clear();
      a=2;
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      lcd.clear();
      a=5;
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==2 && b==0){ //Temperature New_Val=b1
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("Temperature: ");
    lcd.print(temperature_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      a=2;
      b=1;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      lcd.clear();
      a=3;
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      lcd.clear();
      a=1;
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==3 && b==0){ //Roration New_Val=b2
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("Rotation: ");
    lcd.print(rotation_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      a=3;
      b=2;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      lcd.clear();
      a=4;
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      lcd.clear();
      a=2;
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==4 && b==0){ //Dispenser New_Val=b3
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("Dispenser: ");
    lcd.print(dispenser_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      a=4;
      b=3;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      lcd.clear();
      a=5;
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      lcd.clear();
      a=3;
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==5 && b==0){ //Cooler New_Val=b4
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("Cooler: ");
    lcd.print(cooler_value);
    lcd.print("%");
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      a=5;
      b=4;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      lcd.clear();
      a=1;
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      lcd.clear();
      a=4;
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  // Level 2
  while (a==2 && b==1){ //Temperature L2
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("New value: ");
    lcd.print(temperature_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      b=0;
      a=2;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      temperature_value--;
      lcd.setCursor(10, 1);
      lcd.print(temperature_value);
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      temperature_value++;
      lcd.setCursor(10, 1);
      lcd.print(temperature_value);
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==3 && b==2){ //Rotation L2
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("New value: ");
    lcd.print(rotation_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      b=0;
      a=3;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      rotation_value--;
      lcd.setCursor(10, 1);
      lcd.print(rotation_value);
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      rotation_value++;
      lcd.setCursor(10, 1);
      lcd.print(rotation_value);
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==4 && b==3){ //Dispenser L2
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("New value: ");
    lcd.print(dispenser_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      b=0;
      a=4;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      dispenser_value--;
      lcd.setCursor(10, 1);
      lcd.print(dispenser_value);
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      dispenser_value++;
      lcd.setCursor(10, 1);
      lcd.print(dispenser_value);
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }
  while (a==5 && b==4){ //Cooler L2
    lcd.setCursor(4, 0);
    lcd.print("Settings:");
    lcd.setCursor(0, 1);
    lcd.print("New value: ");
    lcd.print(cooler_value);
    buttonState_OK = !digitalRead(button_ok);
    if(buttonState_OK == HIGH){
      delay(300);
      lcd.clear();
      b=0;
      a=5;
    }
    buttonState_DOWN = !digitalRead(button_down);
    if(buttonState_DOWN == HIGH){
      delay(300);
      lcd.clear();
      cooler_value--;
      lcd.setCursor(10, 1);
      lcd.print(cooler_value);
    }
    buttonState_UP = !digitalRead(button_up);
    if(buttonState_UP == HIGH){
      delay(300);
      lcd.clear();
      cooler_value++;
      lcd.setCursor(10, 1);
      lcd.print(cooler_value);
    }
    //Control Temp
    control_temp();
    //Control Dispenser
    control_dispenser();
  }  
}

void control_temp(){
    if(millis() - previousMillisForTempSensor >= 5000){
        previousMillisForTempSensor= millis();
        real_temperature=thermocouple.readCelsius();
        if (real_temperature < temperature_value){ 
          digitalWrite(SSR_Relay, HIGH);
        } else {
          digitalWrite(SSR_Relay, LOW);
        }
    }
}
void control_dispenser(){
  /*
  digitalWrite(PIN_DIR, HIGH);
  digitalWrite(PIN_STEP, HIGH);
  delay(dispenser_value);
  digitalWrite(PIN_STEP, LOW);
  delay(dispenser_value);
  */
  if(millis() - previousMillisForDispenser >= dispenser_value){
    previousMillisForDispenser= millis();
    digitalWrite(PIN_STEP, HIGH);       
  }
  if(millis() - previousMillisForDispenser >= dispenser_value){
    previousMillisForDispenser= millis();
    digitalWrite(PIN_STEP, LOW);       
  }
}
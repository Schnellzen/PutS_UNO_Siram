#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h> 
#include <LCD_I2C.h> 

#define DHTTYPE DHT11 
#define DHTPIN 5
DHT dht(DHTPIN, DHTTYPE); 
int sensor_pin_1 = A0; // Soil Sensor input at Analog PIN A0 
int sensor_pin_2 = A1;
int output_value_1;
int output_value_2; 
int relayPin1 = 6;
int relayPin2 = 3; 
//float humid;
//float temp;

LCD_I2C lcd(0x27, 16, 2); 

////threshold and safety
#define safety 2 //pump won't turn on if the mc is below 2% (since a value below 2% indicates sensor failure)
//comment out safety to disable safety feature
#define threshold 70 //pump will turn on when mc is above 70% 
//change this value to your preference
////end- threshold and safety


////two point callib
#define callib //uncomment this for using your own calibration data (edit from line 22 to 31, replace it with you own data)

#ifdef callib //tjis value will only used if line 20 is uncommented
  #define mc1_raw_U 1024 //raw value when mc1_value_U is readed, for example, if the soil moisture is 0%, the raw value usually arround 1000
  #define mc1_raw_D 272
  #define mc1_value_U 0 //moisture content in percent when mc1_raw_U value read
  #define mc1_value_D 93 //moisture content in percent when mc1_raw_D value read

  #define mc2_raw_U 1024
  #define mc2_raw_D 265
  #define mc2_value_U 0
  #define mc2_value_D 93

#else 
  #define mc1_raw_U 1024
  #define mc1_raw_D 350 //first value used is 450
  #define mc1_value_U 0
  #define mc1_value_D 100

  #define mc2_raw_U 1024
  #define mc2_raw_D 350
  #define mc2_value_U 0
  #define mc2_value_D 100
#endif
////end- two point callib

//void-voidan 
// void dht_data(){ //uncomment this if only dht data update needed
//   float humid = dht.readHumidity(); 
//   float temp = dht.readTemperature();
// }

void dht_data_lcd(){
  float humid = dht.readHumidity(); 
  float temp = dht.readTemperature();
  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("SUHU : "); 
  lcd.print(temp); 
  lcd.print((char)223); 
  lcd.print("C"); 
   
  lcd.setCursor(0,1);  
  lcd.print("Humidity: "); 
  lcd.print(humid); 
  lcd.print("%"); 
}

void mc_data_exc(){
  output_value_1 = map(analogRead(sensor_pin_1),mc1_raw_U,mc1_raw_D,mc1_value_U,mc1_value_D); 
  if(output_value_1<threshold){
    digitalWrite(relayPin1, LOW); 
    #ifdef safety
    if(output_value_1<safety){
      digitalWrite(relayPin1, HIGH);
    }
    #endif
  }
  else{
    digitalWrite(relayPin1, HIGH); 
  }

  output_value_2 = map(analogRead(sensor_pin_2),mc2_raw_U,mc2_raw_D,mc2_value_U,mc2_value_D); 
  if(output_value_2<threshold){
    digitalWrite(relayPin2, LOW); 
    #ifdef safety
    if(output_value_2<safety){
      digitalWrite(relayPin2, HIGH);
    }
    #endif
  }
  else{
    digitalWrite(relayPin2, HIGH); 
  }

}

void mc_lcd(){
  lcd.clear();

  lcd.setCursor(0,0); 
  lcd.print("M1:"); 
  lcd.print(output_value_1); 
  lcd.print("% "); 

  lcd.setCursor(0,1); 
  if(output_value_1<50){ 
    lcd.print("P1 ON"); 
  }
  else { 
    lcd.print("P1 OFF");        
  }

  lcd.setCursor(8,0); // set placement for 2nd motor in LCD, since it 16char with 2 line, so set to (8,0)
  lcd.print("M2:"); 
  lcd.print(output_value_2); 
  lcd.print("% "); 

  lcd.setCursor(8,1); // set placement for 2nd motor in LCD
  if(output_value_2<50){ 
    digitalWrite(relayPin2, LOW); 
    lcd.print("P2 ON"); 
  }
  else { 
    digitalWrite(relayPin2, HIGH); 
    lcd.print("P2 OFF");        
  }

}

void setup(){ 
  lcd.begin(); 
  Serial.begin(9600); 
  lcd.backlight(); 
  pinMode(sensor_pin_1, INPUT); 
  pinMode(relayPin1, OUTPUT); 
  digitalWrite(relayPin1, HIGH);
  pinMode(sensor_pin_2, INPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin2, HIGH);
  dht.begin();
} 
 
void loop(){ 
  mc_data_exc();
//  dht_data();
  
  if(millis()%2000==0){
    if(millis()%4000==0){
      mc_lcd();
    }
    else{
      dht_data_lcd();
    }
  }
//  delay(200);
}
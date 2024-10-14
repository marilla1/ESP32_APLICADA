#include <LiquidCrystal_I2C.h> //lcd
LiquidCrystal_I2C lcd(0x27,16,2); //set the LCD address to 0x3F for a 16 chars and 2-line display


const int Led = 2;
const int trig = 13;
const int echo = 12;
float distancia;
long tiempo;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(Led, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  lcd.init(); // initialize the lcd lcd.backlight();
  lcd.backlight();
  lcd.setCursor(0,0); // column#4 and Row #1 

}

void loop() {
  // put your main code here, to run repeatedly:

  // digitalWrite(Led, HIGH);
  // Serial.println("ON");
  // delay(5000);
  // digitalWrite(Led, LOW);
  // Serial.println("OFF");
  // delay(5000);

  ultrasonico();

}

void ultrasonico() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  tiempo = pulseIn(echo, HIGH);
  distancia = tiempo * 0.034 / 2;


   // Controla el buzzer
  controlarLeds(distancia);    // Controla los LEDs
}

void controlarLeds(long distancia) {
  if (distancia > 5) {

    digitalWrite(Led, LOW);  // Enciende el LED 1 cuando la distancia es mayor a 150 cm
    lcd.print(distancia);
    lcd.print(" cm");
    lcd.setCursor(0,1); // column#4 and Row #1 
    lcd.print("Llenando");
    
    Serial.print("Llenando\n");
    Serial.print(distancia);
    Serial.print(" cm\n");
    
    delay(300);
    lcd.clear();



  } 
  else {

    digitalWrite(Led, HIGH);   // Apaga el LED 1
    Serial.print("Detenner Llenando\n");
    Serial.print(distancia);
    Serial.print(" cm\n");

    lcd.print(distancia);
    lcd.print(" cm");
    lcd.setCursor(1,1); // column#4 and Row #1 
    lcd.print("Detener Llenado");
    
    delay(300);
    lcd.clear();



  }
}

// =====================================================================================================
// =====================================================================================================
// ===============================  Authors: John Kennedy Loria Segundo  ===============================
// ====================  Arduino controls fans based on temperature.cpp - ver: 0.1  ====================
// ===========  Initial Date: 17/11/2018 - Update Date: 14/12/2018 - Final Date: ??/??/2018  ===========
// =====================================================================================================
// =====================================================================================================

//		STATUS:
//
//							IN PROGRESS...
//

//	Code Function:
//
//
//        EN:
//        Create a program that monitors the temperature of a device and proportionally increases
//        the speed of a FAN according to temperature, while another FAN is variable through a
//        potentiometer, when the temperature reaches a critical level it touches a buzzer and
//        lights an LED, and button to momentarily stop the first FAN and lights an LED indicating
//        it is pressed. An LCD indicates the temperature and give warnings. Have an emergency
//        cooling routine, turning on both Fans at the maximum and preventing getting out of this
//        routine until the temperature reaches a safe level.
//
//				PT:
//				Criar um programa que monitora a temperatura de um dispositivo e, proporcionalmente
//				aumenta a velocidade de um FAN conforme a temperatura, enquanto outro FAN é variável
//				através de um potenciômetro, quando a temperatura chegar a um nível crítico toca um
//				buzzer e acende um LED, e um botão para parar momentaneamente o primeiro FAN e acende
//				um LED indicando que está pressionado.
//				Um LCD indicar a temperatura e dar avisos.
//				Ter uma rotina de resfriamento de emergencia, ligando os dois Fans no máximo e
//				impedindo sair dessa rotina até que a temperatura alcance um nível seguro.
//
//					Itens:
//					—	2x Fans;
//					—	1x Potentiometer;
//					—	1x Buzzer;
//					—	3x LEDs;
//					—	1x LCD;
//					—	1x Button;
//					—	1x LM35 (Temperature Sensor).
//

//	Logical Description:
//
//
//
//

//	Version Details:
//
//
//				This code has been created to exercise Arduino language
//				development (focused in C++) for Arduino MCU modules.
//
//
//				In this project has been used:
//
//				- 		 Board:		Arduino UNO
//				- 		   MCU:		Atmel - ATMEGA328P
//				- 		 Clock:		16MHz (Xtal: 16MHz)
//				- 		Editor:		Sublime Text (Version 3.1.1)
//				- 		Syntax:		Arduino Language (C++ based)
//				-      Library:		LiquidCrystal_I2C / TimerOne
//				-  Compilation:		Arduino IDE
//				-	 Simulator:		Proteus 8.4
//				-  Boot-loader: 	Arduino
//

//										CODING:
//  ----------------------------------------------------------------------------------------------------

//				Initial Definitions:

#include <LiquidCrystal_I2C.h>
#include "TimerOne.h"

#define Fan_2 10
#define Fan_1 9

#define Button 2

#define Potentiometer 15
#define LM35 14

#define Buzzer 13
#define LED_Warning 12
#define LED_Stoped 8
#define LED_Normal 7

//  ----------------------------------------------------------------------------------------------------

//				Memory Organization:

LiquidCrystal_I2C lcd(0x20, 0, 1, 2, 4, 5, 6, 7); // Confirm pins 0 to 7

byte LM35_update_measure = 0;
byte LM35_update_measure_old = 0;

byte Potentiometer_update_measure = 0;
byte Potentiometer_update_measure_old = 0;

//  ----------------------------------------------------------------------------------------------------

//				LM35_update_measure_isr (TimerOne Interrupt Sub-Routine):

void LM35_update_measure_isr()
{
  LM35_update_measure = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;

  Potentiometer_update_measure = (float(analogRead(Potentiometer)) * 5 / (1023) * 20);
}

//  ----------------------------------------------------------------------------------------------------

//				StopFan_isr (Interrupt Sub-Routine):

void StopFan_isr()
{
}

//  ----------------------------------------------------------------------------------------------------

//				ShowTemperature Sub-Routine:

void ShowTemperature()
{
  if (LM35_update_measure != LM35_update_measure_old || Potentiometer_update_measure != Potentiometer_update_measure_old)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(LM35_update_measure);
    lcd.print("\xDF");
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Pot: ");
    lcd.print(Potentiometer_update_measure);
    lcd.print("%");

    LM35_update_measure_old = LM35_update_measure;

    Potentiometer_update_measure_old = Potentiometer_update_measure;
  }
}

//  ----------------------------------------------------------------------------------------------------

//				initializing_routine Sub-Routine:

void initializing_routine()
{
  for (int i = 0; i < 3; i++)
  {
    lcd.print("Initializing");
    delay(250);
    lcd.clear();
    lcd.print("Initializing.");
    delay(250);
    lcd.clear();
    lcd.print("Initializing..");
    delay(250);
    lcd.clear();
    lcd.print("Initializing...");
    delay(250);
    lcd.clear();
  }

  lcd.print(" Welcome =D ");
  delay(250);
}

//  ----------------------------------------------------------------------------------------------------

//				Start Setup:

void setup()
{
  for (int i = 16; i < 20; i++)
  {
    pinMode(i, OUTPUT);
  }

  digitalWrite(Buzzer, HIGH);

  for (int i = 4; i < 8; i++)
  {
    pinMode(i, OUTPUT);
  }

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(15, INPUT);
  pinMode(14, INPUT);

  attachInterrupt(2, StopFan_isr, FALLING); //  Maybe doesn't work properly

  Timer1.initialize(150);
  Timer1.attachInterrupt(LM35_update_measure_isr);

  lcd.begin(16, 2);

  //	initializing_routine();						// Temporally Disable
}

//  ----------------------------------------------------------------------------------------------------

//				Main Routine:

void loop()
{
  ShowTemperature();
}

// 										END!  =)
//  ----------------------------------------------------------------------------------------------------

//					Sketches:
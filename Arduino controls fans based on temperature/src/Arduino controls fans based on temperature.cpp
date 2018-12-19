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
//			EN:
//			Create a program that monitors the temperature of a device and proportionally increases
//			the speed of a FAN according to temperature, while another FAN is variable through a
//			potentiometer, when the temperature reaches a critical level it touches a buzzer and
//			lights an LED, and button to momentarily stop the first FAN and lights an LED indicating
//			it is pressed. An LCD indicates the temperature and give warnings. Have an emergency
//			cooling routine, turning on both Fans at the maximum and preventing getting out of this
//			routine until the temperature reaches a safe level.
//
//			PT:
//			Criar um programa que monitora a temperatura de um dispositivo e, proporcionalmente
//			aumenta a velocidade de um FAN conforme a temperatura, enquanto outro FAN é variável
//			através de um potenciômetro, quando a temperatura chegar a um nível crítico toca um
//			buzzer e acende um LED, e um botão para parar momentaneamente o primeiro FAN e acende
//			um LED indicando que está pressionado.
//			Um LCD indicar a temperatura e dar avisos.
//			Ter uma rotina de resfriamento de emergencia, ligando os dois Fans no máximo e
//			impedindo sair dessa rotina até que a temperatura alcance um nível seguro.
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
//				-  Boot-loader: 	Arduino Uno
//

//										CODING:
//  ----------------------------------------------------------------------------------------------------

//				Initial Definitions:

			#include 	<Arduino.h>
			#include 	<LiquidCrystal_I2C.h>
			#include 	"TimerOne.h"

		#define 	Fan_2 			10
		#define 	Fan_1 			 9

		#define 	Button 			 2

		#define 	Potentiometer	15
		#define 	LM35 			14

		#define		Buzzer 			13
		#define		LED_Warning 	12
		#define		LED_Stoped 		 8
		#define		LED_Normal 		 7

//  ----------------------------------------------------------------------------------------------------

//				Memory Organization:

	LiquidCrystal_I2C lcd( 0x20, 16, 2 ); 

	byte LM35_update_measure = 0 ;
	byte LM35_update_measure_old = 0 ;

	int	 LM35_update_measure_for_PWM = 0 ;

	int  Potentiometer_update_measure = 0 ;
	int  Potentiometer_update_measure_old = 0 ;

	int  Potentiometer_update_measure_for_PWM = 0 ;

	bool Normal_State =  true ;
	bool Stoped_State = false ;

//  ----------------------------------------------------------------------------------------------------

//				Update_measures_isr (TimerOne Interrupt Sub-Routine):

void Update_measures_isr()
{
	LM35_update_measure = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;
	LM35_update_measure_for_PWM = map( LM35_update_measure, 45 , 75 , 338 , 1023 );
	if( LM35_update_measure_for_PWM < 338 )
	{
		LM35_update_measure_for_PWM = 338 ;
	}

	Potentiometer_update_measure = (float(analogRead(Potentiometer)) * 5 / (1023) * 20);
	Potentiometer_update_measure_for_PWM = (float(analogRead(Potentiometer)) * 5 / (1023) * 204.6);

	if( LM35_update_measure > 75 )
	{
		Normal_State = false ;
	}
	if( LM35_update_measure <= 75 )
	{
		Normal_State = true ;
	}
}

//  ----------------------------------------------------------------------------------------------------

//				StopFan_isr (Interrupt Sub-Routine):
/*
void StopFan_isr() 									// Maybe doesn't work properly
{
	Timer1.setPwmDuty( Fan_2, 0 ); 					
	
	if( digitalRead( Button ) != HIGH )
	{
		Stoped_State = true ;
		digitalWrite( LED_Stoped, HIGH );
	}
	else
	{
		Stoped_State = false ;
		digitalWrite( LED_Stoped, LOW );
	}
}	*/

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
	for ( int i = 0 ; i < 3 ; i++ )
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

//				Warning_sr Sub-Routine:

void Warning_sr()
{
	digitalWrite( LED_Normal, 	LOW );
	digitalWrite( LED_Warning, HIGH );
	digitalWrite( Buzzer, 		LOW );

	Timer1.setPwmDuty( Fan_2, HIGH );
	Timer1.setPwmDuty( Fan_1, HIGH );

	if( LM35_update_measure <= 75 )				// doesn't work
	{
		Normal_State = true ;
	
		digitalWrite( LED_Normal, HIGH );
		digitalWrite( LED_Warning, LOW );
		digitalWrite( Buzzer, 	  HIGH );
	}
}

//  ----------------------------------------------------------------------------------------------------

//				Start Setup:

void setup()
{
	pinMode( Buzzer, OUTPUT );
	digitalWrite( Buzzer, HIGH );

	pinMode( LED_Normal, OUTPUT );
	pinMode( LED_Warning, OUTPUT );
	pinMode( LED_Stoped, OUTPUT );

	pinMode( Fan_2, OUTPUT );
	pinMode( Fan_1, OUTPUT );

	pinMode( Potentiometer, INPUT );
	pinMode( LM35, INPUT );

	pinMode( Button, INPUT );

//	attachInterrupt( Button, StopFan_isr, LOW ); //  Maybe doesn't work properly

	Timer1.initialize(150);
	Timer1.attachInterrupt( Update_measures_isr );

	lcd.begin( 16 , 2 );

	Timer1.pwm( Fan_2 , 512 );
	Timer1.pwm( Fan_1 , 512 );

//	initializing_routine();						// Temporally Disable
}

//  ----------------------------------------------------------------------------------------------------

//				Main Routine:

void loop()
{
	ShowTemperature();

	Timer1.setPwmDuty( Fan_2, Potentiometer_update_measure_for_PWM );
	Timer1.setPwmDuty( Fan_1, LM35_update_measure_for_PWM );

	while( LM35_update_measure > 75 )
	{
		Warning_sr();
	}
}

// 										END!  =)
//  ----------------------------------------------------------------------------------------------------

/*					Sketches:








*/
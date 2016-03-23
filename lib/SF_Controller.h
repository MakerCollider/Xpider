
#pragma once

#ifndef SF_CONTROLLER
#define SF_CONTROLLER

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdbool.h>
#include <signal.h>	//
#include <pthread.h>	//presse
#include "mraa.h"

//#define true 	1   		///spell to trure !!!!!....=_=///...
//#define false 	0
#define HIGH 	1 
#define LOW 	0
#define RIGHT 	1           //rotation/turn right  
#define LEFT 	0           //rotation/turn left 
#define STOP    0
								//for arduino board				//for edibot-grove
#define rotation_pin1  	7	    //#define rotation_pin1  10		#define rotation_pin1   6
#define rotation_pin2  	15    	//#define rotation_pin2	 11		#define rotation_pin2   7
#define rotation_pwm	8

#define move_pin1  	   	6		//#define move_pin1  	  3		#define move_pin1  	   10
#define move_pin2  	   	14 		//#define move_pin2  	  5		#define move_pin2  	   11
#define move_pwm		11

#define pitch_pin  		10		//#define pitch_pin  	  6		#define pitch_pin  		8
#define lighthouse_pin  9		//#define lighthouse_pin  9		#define lighthouse_pin  9

#define hall_sensor_pin1  12	//#define lighthouse_pin  9		#define lighthouse_pin  9
#define hall_sensor_pin2  13	//#define lighthouse_pin  9		#define lighthouse_pin  9

#define MIN_PULSE_WIDTH			544		// the shortest pulse sent to a servo in us
#define MAX_PULSE_WIDTH			2400	// the longest pulse sent to a servo in us
#define DEFAULT_PULSE_WIDTH		1500	// default pulse width when servo is attached
#define PWM_SERVO_PERIOD		19000 	//in us
#define SERVO_MAX_DEGREE		120 	//servo max degree

extern          bool rcvState;    		//0           // successfully received state/truth  
extern          bool rcvState1;   		//
extern unsigned  int Package_Count;
extern unsigned  int Package_Count_1;
extern unsigned char Rvalue_A;
extern unsigned char weapon_Clip[5];
extern 			char weapon_Clip_c[5];

extern 			char rcvBuffer[30];

extern unsigned char Command[30];
extern unsigned char stateMachine;
extern unsigned char stateMachine1;
extern unsigned char Data_HEAD_1;       		//fist data head of a package
extern unsigned char Data_HEAD_2;
extern unsigned char Data_LENGTH;          		//including data first two byte 0xA1/B1 0xF1
extern unsigned char IR_LENGTH;
extern unsigned char IR_HEAD;
extern unsigned char IR_Data;
extern unsigned char rcvByte;              		//store one byte from serial port 
extern           int rcvIndex; 
extern unsigned char rcvByte1;              	//store one byte from serial port 
extern           int rcvIndex1;

extern unsigned char xorchkm;
extern unsigned char AR_ADDR;           		//AR Device address
extern unsigned char Data_END;
extern unsigned char Operation;      			//F1-send; F2-change AR Address; F3-change Baud rate 
                                    			// check the datasheet for details 
extern unsigned char DestnID;        			//destination id
// extern unsigned char SourceADDR = 0xB1;
extern unsigned char AttackID;          		//distinguish attacking from

extern unsigned char R_Direction;          //rotation direction 0/1
extern unsigned char R_Speed;              //rotation speed
extern unsigned char M_Direction;          //move direction  
extern unsigned char M_Speed;              //move speed   
extern unsigned char P_Degree;             //Picth degree 
extern 			char Rvalue_C;
extern 			char damage[30];
extern			int R_Degree_num;

mraa_result_t rv;
mraa_gpio_context 	ROTATION_PIN1;
mraa_gpio_context	ROTATION_PIN2;
mraa_pwm_context  	ROTATION_PWM;
mraa_gpio_context	MOVE_PIN1;
mraa_gpio_context	MOVE_PIN2;
mraa_pwm_context 	MOVE_PWM;
mraa_pwm_context	PITCH_PIN;	
mraa_pwm_context	LIGHTHOUSE;
mraa_gpio_context	HALL_SENSOR_PIN1;
mraa_gpio_context	HALL_SENSOR_PIN2;
mraa_uart_context	UART;

int ADC(void);
int setup(void);

void Set_Command();
void Rotation_Control(int degree_num, unsigned char Speed);
void Move_Control(unsigned char Direction, unsigned char Speed);
void Motor_Control(unsigned char Pin );
void Pitch_Control(unsigned char POS);
void Attack_Action();
void Aim_Action();
void Fire_In_Hole();                     //fire with weapon data weapon_Clip[]
void Set_Package();
void HelloFN();
void Life_Tower(float fade_step, float sleep_time);

bool Attack_Analysis(void);

int map_c(int value, int a, int b, int x,int y);

void isr_hall_sensor1(void* args);
// void isr_hall_sensor2(void* args);

#ifdef __cplusplus
}
#endif

#endif



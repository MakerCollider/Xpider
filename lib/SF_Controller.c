#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>    
#include <unistd.h>
#include "mraa.h"
#include "SF_Controller.h"

         bool rcvState = 0;             // successfully received state/truth  
         bool rcvState1 = 0;   
unsigned  int Package_Count = 0;
unsigned  int Package_Count_1 = 0;
unsigned char Rvalue_A = 0;
unsigned char Rvalue_B = 0;
unsigned char weapon_Clip[5] = {0xA1, 0xF1, 0xEE, 0xEE, 0xEE};
         char weapon_Clip_c[5];
         char rcvBuffer[30];
unsigned char Command[30] = {0};
unsigned char stateMachine = 0;
unsigned char stateMachine1 = 0;
unsigned char Data_HEAD_1 = 0x55;       //fist data head of a package
unsigned char Data_HEAD_2 = 0xAA;
unsigned char Data_LENGTH = 10;         //including data first two byte 0xA1/B1 0xF1
unsigned char IR_LENGTH = 5;
unsigned char IR_HEAD = 0x00;
unsigned char IR_Data = 0x05;
unsigned char rcvByte = 0;              //store one byte from serial port 
          int rcvIndex = 0; 
unsigned char rcvByte1 = 0;             //store one byte from serial port 
          int rcvIndex1 = 0;  
unsigned char xorchkm = 0;
unsigned char AR_ADDR = 0xA1;           //AR Device address
unsigned char Data_END = 0xEE;
unsigned char Operation_Code=0xF1;      //F1-send; F2-change AR Address; F3-change Baud rate 
                                        // check the datasheet for details 
unsigned char DestnID = 0x00;           //destination id
// unsigned char SourceADDR = 0xB1;
unsigned char SourceID = 0xA1; 
unsigned char AttackID = 0xAA;          //distinguish attacking from
unsigned char R_Direction = 0;          //rotation direction 0/1
unsigned char R_Speed  = 0;             //rotation speed
unsigned char M_Direction = 0;          //move direction  
unsigned char M_Speed = 0;              //move speed   
unsigned char P_Degree = 0;             //Picth degree 
         char Rvalue_C;
         char damage[30] = {0};
          int isr_counter1 = 0;                   //for hall sensor, mark 
          int start_counter;
          int R_Degree_num;


// Gpio Edge types for interupts
// mraa_result_t   mraa_gpio_isr (mraa_gpio_context dev, mraa_gpio_edge_t edge, void(*fptr)(void *), void *args);
// Enumerator
// MRAA_GPIO_EDGE_NONE     No interrupt on Gpio
// MRAA_GPIO_EDGE_BOTH     Interupt on rising & falling
// MRAA_GPIO_EDGE_RISING   Interupt on rising only
// MRAA_GPIO_EDGE_FALLING  Interupt on falling only    

int setup(void)
{   
    // mraa_result_t rv;
    // rv = mraa_init();                // !!!mraa have been Initialised before...could't be re-initialised
    if(0)
    {   
        printf("mraa setup faild... -_-///\n");
        // mraa_result_print(rv);   
        return -1;
    }
    else
    {
        fprintf(stdout, "mraa setup success...>_< ...\n mraa version: %s\n", mraa_get_version());      
        
        // rotation_pin1    
        ROTATION_PIN1 = mraa_gpio_init(rotation_pin1);
        rv = mraa_gpio_dir(ROTATION_PIN1, MRAA_GPIO_OUT);   // set direction to OUT
        if(rv != MRAA_SUCCESS){  mraa_result_print(rv);}
        mraa_gpio_write(ROTATION_PIN1, LOW);
        // mraa_gpio_close(ROTATION_PIN1);  
        
        // rotation_pin2
        ROTATION_PIN2 = mraa_gpio_init(rotation_pin2);
        rv = mraa_gpio_dir(ROTATION_PIN2, MRAA_GPIO_OUT);   // set direction to OUT
        if(rv != MRAA_SUCCESS){  mraa_result_print(rv);}
        mraa_gpio_write(ROTATION_PIN2, LOW);
        // mraa_gpio_close(ROTATION_PIN2);

        // rotation_pwm
        ROTATION_PWM = mraa_pwm_init(rotation_pwm);
        if(ROTATION_PWM == NULL){  return 1;}           //
        // mraa_pwm_period_us(ROTATION_PWM, 200);
        mraa_pwm_enable(ROTATION_PWM, 1);
        mraa_pwm_write(ROTATION_PWM, 0.0f);
        // mraa_pwm_close (ROTATION_PWM);
        
        // move_pin1
        MOVE_PIN1 = mraa_gpio_init(move_pin1);
        rv = mraa_gpio_dir(MOVE_PIN1, MRAA_GPIO_OUT);   // set direction to OUT
        if(rv != MRAA_SUCCESS){  mraa_result_print(rv);}
        mraa_gpio_write(MOVE_PIN1, LOW);
        // mraa_gpio_close(MOVE_PIN1);

        // move_pin2
        MOVE_PIN2 = mraa_gpio_init(move_pin2);
        rv = mraa_gpio_dir(MOVE_PIN2, MRAA_GPIO_OUT);   // set direction to OUT
        if(rv != MRAA_SUCCESS){  mraa_result_print(rv);}
        mraa_gpio_write(MOVE_PIN2, LOW);
        // mraa_gpio_close(MOVE_PIN2);

        printf("MOVE_PWM Init .. ..\n");
        // move_pwm
        MOVE_PWM = mraa_pwm_init(move_pwm);
        if(MOVE_PWM == NULL)
        {  
            printf("MOVE_PWM failed .. @_@..\n");
        }
        else
          {
            printf("MOVE_PWM successfully ....\n");
          }

        mraa_pwm_period_us(MOVE_PWM, 200);
        mraa_pwm_enable(MOVE_PWM, 1);
        mraa_pwm_write(MOVE_PWM, 0.0f);
        // mraa_pwm_close (MOVE_PWM);

        // pitch_pin
        PITCH_PIN = mraa_pwm_init(pitch_pin);
        if(PITCH_PIN == NULL){  printf("PITCH_PIN failed .. @_@..\n");}
        mraa_pwm_period_us(PITCH_PIN, PWM_SERVO_PERIOD);
        mraa_pwm_enable(PITCH_PIN, 1);
        mraa_pwm_pulsewidth_us(PITCH_PIN, DEFAULT_PULSE_WIDTH);
        // mraa_pwm_write(PITCH_PIN, 0.5f);    //steering engine;  
        // mraa_pwm_close (PITCH_PIN);

        // lighthouse
        LIGHTHOUSE = mraa_pwm_init(lighthouse_pin);
        if(LIGHTHOUSE == NULL){ printf("LIGHTHOUSE failed .. @_@..\n");}
        mraa_pwm_period_us(LIGHTHOUSE, 200);
        mraa_pwm_enable(LIGHTHOUSE, 1);
        mraa_pwm_write(LIGHTHOUSE, 0.0f);
        // mraa_pwm_close (LIGHTHOUSE);

        //hall_sensor_pin1
        HALL_SENSOR_PIN1 = mraa_gpio_init(hall_sensor_pin1);
        rv = mraa_gpio_dir(HALL_SENSOR_PIN1, MRAA_GPIO_IN);   // set direction to OUT
        if(rv != MRAA_SUCCESS){  mraa_result_print(rv);}
        mraa_gpio_edge_t edge = MRAA_GPIO_EDGE_BOTH;
        mraa_gpio_isr(HALL_SENSOR_PIN1, edge, &isr_hall_sensor1, NULL);

        //hall_sensor_pin2
        HALL_SENSOR_PIN2 = mraa_gpio_init(hall_sensor_pin2);
        rv = mraa_gpio_dir(HALL_SENSOR_PIN2, MRAA_GPIO_IN);   // set direction to OUT
        if(rv != MRAA_SUCCESS){  mraa_result_print(rv);}
        // mraa_gpio_edge_t edge = MRAA_GPIO_EDGE_BOTH;
        // mraa_gpio_isr(HALL_SENSOR_PIN2, edge, &isr_hall_sensor2, NULL);

        UART = mraa_uart_init(0);
        if(UART == NULL)
        {  
            fprintf(stderr, "UART failed to setup\n"); return EXIT_FAILURE;
        }
        else
            {
                printf("Initialised UART .. @_@..hhh \n");
            }   

        mraa_uart_set_baudrate(UART, 57600);    //Set the baudrate.
        mraa_uart_set_mode(UART, 8,MRAA_UART_PARITY_NONE, 1);
        // mraa_uart_set_timeout(mraa_uart_context dev,int read,int write, int interchar); 
        // mraa_uart_stop(mraa_uart_context dev);   //Destroy a mraa_uart_context
        // mraa_uart_write(UART, buffer, sizeof(buffer));
        // mraa_uart_read(UART, buffer, sizeof(buffer));
        // mraa_uart_data_available(mraa_uart_context dev, unsigned int millis);

        if(LIGHTHOUSE == NULL)
        {
            fprintf(stderr, "Are you sure that this pins you requested is valid on your platform?");
            exit(1);
            return -1;
        }
        else
            {
                printf("Initialised io pins...n_n...\n");
                return 1;
            }

        Pitch_Control(60);  //Initialised forbiden 
    }
}

void isr_hall_sensor1(void* args)
{
    static int valA = 0; 
    static int valB = 0;
    static int d = 0;
    static int D_Value;
    valA = mraa_gpio_read(HALL_SENSOR_PIN1);
    valB = mraa_gpio_read(HALL_SENSOR_PIN2);
    if(valA==valB)
    {
        d=1;
    }
    else
        {
            d=-1;
        }
    isr_counter1+=d;

    D_Value = abs(isr_counter1 - start_counter);
    if(D_Value < 2*abs(R_Degree_num))
    {
        printf("D_Value:%d\n", D_Value);
    }
    else if(D_Value >= 2*abs(R_Degree_num))
         {   
             mraa_gpio_write(ROTATION_PIN1, LOW);
             mraa_gpio_write(ROTATION_PIN2, LOW);// stop it, when it t 
         }

    // printf("isr_counter1:%d\n...Y", isr_counter1);
}

int map_c(int value, int a, int b, int x, int y)
{
    int result = 0;
    if(a >= b || x >= y)
    {
        printf("-_-///...must be: a < b && x < y");
    }
    else
        {
            result = (int)((y - x)/(float)(b-a)*(value - a) + x);
            return result;
        }
}

// 500rpm  PB 1:30  1:4.4 30 pusles / r   to motor every 12 degree / pusle | to  
void Rotation_Control(int degree_num, unsigned char Speed)
{
    printf("Rotation_Control\n");  
    static float Speed_f = 0.0f;
    Speed_f = Speed/255.0; 
    // R_Degree_num =  degree_num;      
    start_counter = isr_counter1;       //before rotation mark the counter
                                        //!haven't reset isr_counter1 vanlue
    if(degree_num >= 0)
    {
        printf("Rotation to Right...Speed:%d \n", Speed);
        mraa_gpio_write(ROTATION_PIN1, LOW);
        mraa_gpio_write(ROTATION_PIN2, HIGH);
        mraa_pwm_write(ROTATION_PWM, Speed_f);
    } 
    else if(degree_num < 0)
         {
            printf("Rotation to LEFT...Speed:%d \n", Speed);
            mraa_gpio_write(ROTATION_PIN1, HIGH);
            mraa_gpio_write(ROTATION_PIN2, LOW);
            mraa_pwm_write(ROTATION_PWM, Speed_f);
         }
}

void Move_Control(unsigned char Direction, unsigned char Speed)
{   
    printf("Move_Control\n");
    static float Speed_f = 0.0f;
    Speed_f = Speed/255.0;
    printf("Speed_f: %f \n",Speed_f);
    if(Direction == 0)
    {   
        printf("Move to Back...Speed:%d \n", Speed);
        mraa_gpio_write(MOVE_PIN1, LOW);
        mraa_gpio_write(MOVE_PIN2, HIGH);
        mraa_pwm_write(MOVE_PWM, Speed_f);
    } 
    else if(Direction == 1)
         {
            printf("Move to Forward...Speed:%d \n", Speed);
            mraa_gpio_write(MOVE_PIN1, HIGH);
            mraa_gpio_write(MOVE_PIN2, LOW);// statement
            mraa_pwm_write(MOVE_PWM, Speed_f);
         }
}

void Pitch_Control(unsigned char POS)
{
    printf("Pitch_Control\n");
    static int PW_Servo = 0;
    PW_Servo = map_c(POS,0,120,MIN_PULSE_WIDTH,MAX_PULSE_WIDTH);
    if((POS <= SERVO_MAX_DEGREE) && (POS >= 0))
    {   
        printf("Turn to : %d degree \n", POS);
        mraa_pwm_pulsewidth_us(PITCH_PIN,PW_Servo);
    }
    else    
        {
            printf("error...degree must between: 0 ~ %d \n",SERVO_MAX_DEGREE);
        }
        sleep(1);
}

void Life_Tower(float fade_step, float sleep_time)
{   
    static float value = 0;
    value = 0;
    while(value < 1)
    {    
        value += fade_step;
        mraa_pwm_write(LIGHTHOUSE, value);
        usleep(sleep_time);
    }
    value = 1;
    while(value >= 0)
    {  
        value -= fade_step;
        mraa_pwm_write(LIGHTHOUSE, value);
        usleep(sleep_time);
    }
}

void Fire_In_Hole()                     //fire with weapon data weapon_Clip[]
{   
    weapon_Clip[0] = AR_ADDR;
    weapon_Clip[1] = Operation_Code;
    weapon_Clip[2] = IR_HEAD;
    static int i = 0;
    for (i = 0; i < IR_LENGTH; i++)
    {
        weapon_Clip_c[i] = (char)(weapon_Clip[i]);
    }
    mraa_uart_write(UART, weapon_Clip_c, 5);
} 

bool Attack_Analysis(void)
{   
    while(mraa_uart_data_available(UART, 0)>0) 
    {
        mraa_uart_read(UART, &rcvByte, 1);
        // printf("I am in 1...Data: %x\n",rcvByte);
        // printf("stateMachine: %x\n",stateMachine);
        switch(stateMachine)
        {
            case 0:
                    if(rcvByte == IR_HEAD)                   //get a head byte of a package 
                    {
                        stateMachine = 1;
                        weapon_Clip[2] = rcvByte;                 //store the head byte 
                    }
                    else
                        {
                            stateMachine = 0;
                        }   
                    break;
            case 1:
                    if(rcvIndex < (IR_LENGTH  - 3))             //
                    {
                        weapon_Clip[(rcvIndex + 3)] = rcvByte;    //store two data bytes 
                        rcvIndex++;
                    } 
                    if(rcvIndex == 2)          //for receiving 3 bytes data to finish and breakout
                    {
                        rcvState = true;               //Having recevied a package successfully.
                        rcvIndex = 0;                  //set rcvIndex 
                        stateMachine = 0;              //set stateMachine
                        Package_Count++; 
                    }
                    break;
            default:
            break;
        }
        return rcvState;  
    } 
}

void Set_Package()
{
    Data_HEAD_1 = 0x00;       //fist data head of a package
    Data_HEAD_2;
    Data_LENGTH = 5;          //including data first two byte 0xA1/B1 0xF1
    IR_LENGTH = 5;
    rcvByte = 0;              //store one byte from serial port 
    rcvIndex = 0; 
    rcvByte1 = 0;             //store one byte from serial port 
    rcvIndex1 = 0;
    rcvState1 = 0;
    xorchkm = 0;
    AR_ADDR = 0xA1;           //AR Device address
    Data_END = 0xEE;
    Operation_Code=0xF1;      //F1-send; F2-change AR Address; F3-change Baud rate
    // DestnID = 0xA1;         //destination id
    // SourceADDR = 0xB1;
    SourceID = 0xA1; 
    AttackID = 0xAA;          //distinguish attacking from
    Rvalue_C;        
} 

bool attackChecking(void)
{   
    

}

void Motor_Control(unsigned char Pin )
{

}

void Aim_Action()
{


}



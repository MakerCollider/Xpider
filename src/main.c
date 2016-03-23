#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
// #include <stdbool.h>
#include <mraa.h>
#include <pthread.h>    //
#include "SF_Controller.h"

// R_Direction = 0;          //rotation direction 0/1
// R_Speed  = 0;             //rotation speed
// M_Direction = 0;          //move direction  
// M_Speed = 0;              //move speed   
// P_Degree = 0;     
// unsigned sleep (unsigned int seconds);//s
// int usleep (useconds_t usec);//us

bool running = false,flagA = false,flagB = false;
int inData  = 0;
char inchar,last_inchar, new_inchar;

// int running = false,flagA = false,flagB = false;
void *flash(void* cs)
{  
    while(running)
    {   //switch(state)     //flash /sos  /breathing
        if(flagA)
        {
            Life_Tower(0.008, 10000);   //fade step, fade time   
            usleep(50000);
            // flagA = false;
        }
    }
}

void *serialEvent(void* cs)
{
    while(running)
    {   
        Attack_Analysis();
        if(rcvState == true)
        {   
            printf("Send sussess .. >_< ...%d \n",Package_Count);
            Fire_In_Hole();
            rcvState = false;
        }    
        // flagB = false;
       
    }
}

void clear(int signo)
{
    printf("exit.....\n");
    running = 0;
}

void halt(void)
{   
    Rotation_Control(0, STOP);
    Move_Control(0, STOP);
    Pitch_Control(SERVO_MAX_DEGREE/2);
    printf("HALT\n");
    // sleep(time);
}
void stop_All(int time)
{   
    // Rotation_Control(R_Direction, STOP);
    Move_Control(M_Direction, STOP);
    Pitch_Control(STOP);
    printf("STOP\n");
    sleep(time);
}

void move_(int r_direction, int r_speed, int m_direction, int m_speed, int p_degree, int time_)
{   
    R_Direction = r_direction;    //
    M_Direction = m_direction; 
    R_Speed = r_speed;
    M_Speed = m_speed;
    P_Degree = p_degree;   
    // Rotation_Control(R_Direction, R_Speed);
    Move_Control(M_Direction, M_Speed);
    Pitch_Control(P_Degree);        //0~180
    sleep(time_);
}




int main(int argc, char* argv[])
{
    printf("convert sussess n_n >_< ...\n");
    
    setup();

    signal(SIGINT, clear);      //system signal "trl+C"
    signal(SIGTERM, clear);     //system signal "kill +PID"

    running = true;
    flagA = true;

    pthread_t flashThread, serialEventThread;
    pthread_create(&flashThread, NULL, flash, NULL);
    pthread_create(&serialEventThread, NULL, serialEvent, NULL);
    
    printf("let's count three numbers ...\n");

    int i =0;
    for(i = 3; i >0 ; i--)
    {
        sleep(1);
        printf("count: %d\n",i);
    }
    sleep(1);
    printf("count sussess  >_< Y ...\n");

    R_Degree_num = 10;    //
    // M_Direction = m_direction; 
    R_Speed = 120;
    M_Speed = 150;
    P_Degree = 60;

    if(MOVE_PWM == NULL)
    {  
        printf("MOVE_PWM failed .. @_@..\n");
    }
    else 
    {
        printf("MOVE_PWM sussessed .. hhh..\n");
    } 

    // mraa_pwm_context  MOVE_PWM1;
    // MOVE_PWM1 = mraa_pwm_init(11);
    // if(MOVE_PWM1 == NULL){  printf("MOVE_PWM failed .. @_@..\n");}
    // // mraa_pwm_period_us(MOVE_PWM, 200);
    //mraa_pwm_enable(MOVE_PWM, 1);

    while(running)
    {     
        printf("...M_Speed:%d \n", M_Speed);  
        //scanf("%C",&inchar);
        printf("input command: W A S D /Q\n");
        printf("order is : ");
        last_inchar = inchar;

        scanf("%c",&inchar);
        while(getchar()!='\n');   //get '\n'

        printf("last_inchar: %c  ", last_inchar);
        printf("new_inchar: %c  \n", new_inchar);

        if((last_inchar == 'w') && (new_inchar == 's'))
        {
            while(M_Speed > 0)
            {    
                printf("abs M_Speed:%d\n", M_Speed);
                M_Speed = M_Speed - 1;
                mraa_pwm_write(MOVE_PWM, M_Speed/255.0);
                usleep(10000);
            }
        }
        else if((last_inchar == 's') && (new_inchar == 'w'))
             {
                 while(M_Speed > 0)
                 {    
                     printf("abs M_Speed:%d\n", M_Speed);
                     M_Speed = M_Speed - 1;
                     mraa_pwm_write(MOVE_PWM, M_Speed/255.0);
                     usleep(10000);
                 }
             }

        M_Speed = 120;

        switch(inchar)
        {
            case 'w':
            case 'W':
                    printf("FORWARD\n");
                    Move_Control(1, M_Speed);
                    break;
            case 's':
            case 'S':
                    printf("BACKWARD\n");
                    Move_Control(0, M_Speed);
                    break;
            case 'a':
            case 'A':
                    printf("TURN RIGHT\n");
                    Rotation_Control(R_Degree_num, R_Speed);
                    break;
            case 'd':
            case 'D':
                    printf("TURN LEFT\n");
                    Rotation_Control(-1*R_Degree_num, R_Speed);
                    break;
            case 'z':
            case 'Z':
                    if(P_Degree < 60)
                    {
                        P_Degree = 60;
                        Pitch_Control(P_Degree);
                    } 
                    else if(P_Degree >= 60 && P_Degree <= 120)
                         {  
                              P_Degree = P_Degree + 5;
                              Pitch_Control(P_Degree);
                         } 
                         else if (P_Degree > 120)
                              {
                                  P_Degree = 60;
                                   Pitch_Control(P_Degree);
                              }
                    break;
            case 'c':
            case 'C':
                    if(P_Degree >= 0 && P_Degree <= 60)
                    {
                        P_Degree = P_Degree - 5;
                        Pitch_Control(P_Degree);
                    } 
                    else if(P_Degree > 60 && P_Degree <= 120)
                         {  
                              P_Degree = 60;
                              Pitch_Control(P_Degree);
                         } 
                         else if(P_Degree > 120)
                              {
                                  P_Degree = 60;
                                  Pitch_Control(P_Degree);
                              }
                    break;                    
            case 'q':
            case 'Q':
                    halt();
                    break;
            default:
            break;
        }
        usleep(10000);

    }

// void Life_Tower(unsigned float fade_step, unsigned float sleep_time)

    return 0;
}



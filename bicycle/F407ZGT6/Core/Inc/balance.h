#ifndef __BALANCE_H__
#define __BALANCE_H__
#include "usart.h"
void show_param_func(int type);
void deal_param_handle(char* msg, int len);
void show_param_init();
void param_init();

void scope_show();
typedef struct 
{
    float angular_v_kp;
    float angular_v_ki;
    float angular_v_kd;
    float angular_kp;
    float angular_ki;
    float angular_kd;
    float fly_wheel_speed_kp;
    float fly_wheel_speed_ki;
    float fly_wheel_speed_kd;
    float angular_zero;             //角度零点
    float angular_target;           //目标角度
    float fly_whell_speed_target;   //飞轮速期望
    float scope_flag;
    float Steer_Kp;                 //舵机kp
    float Steer_Ki;                 //舵机ki
    float Steer_Kd;                 //舵机kd
    float Balance_Kp;               //舵机平衡kp
    float Balance_Ki;               //舵机平衡ki
    float Balance_Kd;               //舵机平衡kd
    float yaw_kp;
    float yaw_ki;
    float yaw_kd;
		
}paramTypeDef;

typedef struct 
{
    float* param0[10];
    float* param1[10];
    float* param2[10];  //仅用于示波器显示
    char buf0[300];
    char buf1[300];
    char buf2[300];
}show_paramTypeDef;

float Angle_Velocity(float Gyro,float Gyro_Target);//角速度环
float X_balance_Control(float Angle,float Angle_Zero,float gyro);//角度环
float Velocity_Control(float encoder,float target_encoder);//速度环
void balance(void);//平衡控制主函数
void scpoe_show_one();
extern unsigned char RunFlag;
extern paramTypeDef param;
extern float PWM_X,PWM_accel,PWM_Final;                  // PWM中间量

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))     //????int??±???????×???
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))   // ??????????±???????????×?????????????×???
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

void ANODT_Send2(unsigned short _a,unsigned short _b,unsigned short a1,unsigned short b1,unsigned short a2,unsigned short b2);

#endif
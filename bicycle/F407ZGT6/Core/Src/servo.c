#include "servo.h"
#include "usart.h"
#include "balance.h"
#include "tim.h"
int middle_duty = 1150;

#define Servo_Center_Mid 930      //舵机直行中值940
#define Servo_Left_Max   (Servo_Center_Mid+100)      //舵机左转极限值
#define Servo_Right_Min  (Servo_Center_Mid-100)      //舵机右转极限值

caramera_t caramera;    

void servo_init(){
    HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);
	// HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_2);
	// HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_3);
	// HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_4);
    servo_set_duty(0);
}

void servo_set_duty(int duty){
    int target = Servo_Center_Mid + duty;
    target = target > Servo_Left_Max ?  Servo_Left_Max : target;
    target = target < Servo_Right_Min ?  Servo_Right_Min : target;
    float res = (float)target / 10000;
    set_pwm_duty(res);
}

void caramera_init(){
    caramera.pos_x = 0;
}

int sz = 0;
int res = 0;
extern int c22;
void caramera_get(char *rec, int len){
    c22++;
    if (len <= 3)
        return;
    int pos = 0;
    for (int i = 0; i < len-2; i++){
        if (rec[i] == 'p' && rec[i+1] == 'o' && rec[i+2] == 's'){
            pos = i+4;
            break;
        }
    }
    if(pos > len) return;
    if (pos == 0)
        return;
    int pos_x = 0;
    if(rec[pos] == '-'){
        pos++;
        while (rec[pos] != 'e'){
            pos_x = pos_x * 10 + rec[pos] - '0';
            pos++;
        }
        pos_x = -pos_x;
    }
    else{
        while (rec[pos] != 'e'){
            pos_x = pos_x * 10 + rec[pos] - '0';
            pos++;
        }
    }
    caramera.pos_x = pos_x;
    sz++;
}
//舵机pid
int Steer_Engine_control(float image_bias)
{
    int steer_out;
    static float Last_image_bias;
    static float bias_intergral;
    bias_intergral += image_bias;
    if(bias_intergral>=50) bias_intergral = 50;
    if(bias_intergral<=-50) bias_intergral = -50;
    steer_out = param.Steer_Kp*image_bias+ param.Steer_Ki * bias_intergral + param.Steer_Kd*(image_bias-Last_image_bias);
    Last_image_bias=image_bias;
    return steer_out;
}
//舵机打角限速
int Steer_Speed_Limit(int now,int last,int limit){
    static int cnt = 0;
    cnt++;
    if (cnt >= 25)
    {
        cnt = 0;
        if ((now - last) >= limit)
            return (last + limit);
        else if ((now - last) <= -limit)
            return (last - limit);
        else
            return now;
    }
    return last;
}

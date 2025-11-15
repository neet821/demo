#ifndef __SERVO_H__
#define __SERVO_H__

#include "main.h"
#include "tim.h"
void servo_init();

void servo_set_duty(int duty);

void caramera_get(char *rec, int len);
void caramera_init();

int Steer_Engine_control(float image_bias);//舵机pid
int Steer_Speed_Limit(int now,int last,int limit);//舵机限�?
typedef struct {
    int pos_x;
} caramera_t;

extern caramera_t caramera;

#endif


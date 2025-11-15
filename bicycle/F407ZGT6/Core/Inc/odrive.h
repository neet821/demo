#ifndef __ODRIVE_H__
#define __ODRIVE_H__

typedef struct
{
    float set_speed0;
    float set_speed1;
    float fliter_speed0[3];
    float fliter_speed1[3];
    float now_speed0;
    float now_speed1;
    int speed0_i;
    int speed1_i;
}OdirveTypeDef;

extern OdirveTypeDef odrive;


void odrive_speed_ctl(unsigned char num, float speed);
void odrive_feedback();
void odrive_init();
void odrive_analyze_speed(char* msg,int len);

#endif
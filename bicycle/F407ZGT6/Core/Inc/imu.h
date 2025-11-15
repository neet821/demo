#ifndef __IMU_H__
#define __IMU_H__

#include "main.h"


void imu_task(void);
void imu_init(void);
float low_pass_filter(float value);
float low_pass_filter2(float value);
void low_pass_filter_init(void);
extern float gyr[3],eul[3];

typedef struct
{

	float wx; /*!< omiga, +- 2000dps => +-32768  so gx/16.384/57.3 =	rad/s */
	float wy;
	float wz;

	float vx;
	float vy;
	float vz;

	float rol;
	float pit;
	float yaw;
} imu_t;

extern imu_t      imu;

#endif


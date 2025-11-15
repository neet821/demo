#include "balance.h"
#include "math.h"
#include "imu.h"
#include "odrive.h"
#include "usart.h"
#include "servo.h"
paramTypeDef param;
show_paramTypeDef show_param;
extern imu_t imu;
float blank = 0;
float PWM_X,PWM_accel,PWM_Final;                  // PWM中间量
int Flag_Stop = 0;
int cnt;//角度环计数
int cnt1;//速度环计数
int count = 0;//后轮计数
int cnt2 = 0;
char DataToSend[100];
int Steer_Target=0;//舵机pid计算目标值
int Steer_Target_Last=0;//舵机last值
float vx;
unsigned char RunFlag = 0;
int sidewalk_time = 0;

void param_init(){
   
    param.angular_v_kp = -2.650;//2.434;	2.480	//类似角度环中的kp，增加或减少此值以优化角速度的响应。
    param.angular_v_ki = 0;			//改善稳态误差的修正
    param.angular_v_kd = -1.135;			//0.998减少系统的震荡并提高稳定性。
	  //角度环
    param.angular_kp = -12.5 ;//13.483并级 -32.05 0 -6.205       12 5 0	增大kp会提高响应速度，但可能引入振荡。减小kp会减缓响应，但可能导致系统不稳定。
    param.angular_ki = 0;			//通过逐步增加ki来消除稳态误差，但要注意过大的ki可能导致系统振荡。
    param.angular_kd = -8;		//7.625增大kd可以减小振荡并改善系统的稳定性，特别是在系统接近设定值时，但过大的kd可能导致系统反应过慢。
	  //初始化飞轮速度PID控制参数
    param.fly_wheel_speed_kp = 0.95;//0.73		//控制飞轮速度的响应速度。一般情况下，kp越高，响应越快。
    param.fly_wheel_speed_ki = 0.3;//0.32		//消除稳态误差。
    param.fly_wheel_speed_kd = 0.02;			//减少振荡，帮助平稳控制飞轮速度。0
	
    param.angular_zero = 3.31+0.02;
	
    param.fly_whell_speed_target = 0;
    param.scope_flag = 0;
    param.Steer_Kp = 3;//舵机kp
    param.Steer_Ki = 0;//舵机kp
    param.Steer_Kd = 0;//舵机kd
}


//角速度环
float Angle_Velocity(float Gyro,float Gyro_Target)
{
    float Angle_Velocity_Bias;
    float PWM_Out;
    static float Angle_Velocity_Last_Bias,Angle_Velocity_Integral;
    Angle_Velocity_Bias = Gyro - Gyro_Target;
    Angle_Velocity_Integral+=Angle_Velocity_Bias;
    if(Angle_Velocity_Integral > 10000) Angle_Velocity_Integral =10000;                    //积分限幅
    if(Angle_Velocity_Integral < -10000) Angle_Velocity_Integral = -10000;                    //积分限幅串级采用
    PWM_Out = param.angular_v_kp * Angle_Velocity_Bias + param.angular_v_ki * Angle_Velocity_Integral + param.angular_v_kd * (Angle_Velocity_Bias - Angle_Velocity_Last_Bias);
    Angle_Velocity_Last_Bias = Angle_Velocity_Bias;                             //保留上次误差
    return PWM_Out;
}

int Error= 0;
//角度环
float X_balance_Control(float Angle,float Angle_Zero,float gyro)
{
     float PWM,Bias;
     static float error,X_Balance_Last_Bias;
     Bias=Angle-Angle_Zero;                                            //获取偏差
     error+=Bias;
	   Error = error-'0'+48;
	  //偏差累积
     if(error>+30) error=+30;                                          //积分限幅
     if(error<-30) error=-30;                                          //积分限幅
    //  PWM=param.angular_kp*Bias + param.angular_ki*error + (Bias - X_Balance_Last_Bias)*param.angular_kd;   //获取最终数值
    PWM=param.angular_kp*Bias + param.angular_ki*error + (gyro)*param.angular_kd;   //获取最终数值
     X_Balance_Last_Bias = Bias;                                        //记录上次偏差
     return PWM;
}


//速度环
float Velocity_Control(float encoder,float target_encoder)
{
    float encoder_bias,Velocity;
    static float encoder_bias_integral;
    encoder_bias = encoder - target_encoder;
    encoder_bias_integral += encoder_bias;
    if(encoder_bias_integral > +500) encoder_bias_integral = +500;                    //积分限幅
    if(encoder_bias_integral < -500) encoder_bias_integral = -500;                    //积分限幅是500
    Velocity = encoder_bias * param.fly_wheel_speed_kp/10 + encoder_bias_integral * param.fly_wheel_speed_ki/1000;
	
	
	//  count+=1;
//	  if(count>=100){
//			char buf[32];
//			sprintf(buf, "%f\n", encoder_bias_integral);
//			for(int i=0; i < strlen(buf); i++)
//				ITM_SendChar(buf[i]);
//		count=0;
//			if(encoder_bias_integral > 300 || encoder_bias_integral < -300)
//				encoder_bias_integral = 0;
//		}
    return Velocity;  
	
}
extern volatile int g_start;
void balance(void)
{
  count++;//后轮计数器
  cnt++;//角度
	imu_task();
  cnt1++;//速度
	param_init();
   vx = low_pass_filter(imu.vx);
	
    /////// 动量轮控制//////////串级
	if(RunFlag == 1 || RunFlag == 2)
	{
     if(cnt1>=50)
		 {
			 PWM_accel = Velocity_Control(odrive.now_speed0,0);
			 cnt1=0;
		 }//动量轮电机速度环正反馈 速度左正右负
	
		 if(cnt>=5)
		 {
		   PWM_X = X_balance_Control(imu.rol,param.angular_zero+PWM_accel,vx);
			 cnt=0;
		 }// 动量轮电机控制左右倾角  角度左负右正
     PWM_Final = Angle_Velocity(vx,PWM_X);        //角速度环  角速度左负右正
     odrive.set_speed0 = PWM_Final;//速度设置左正右负
	 }
		 
		 //odrive.set_speed0 = low_pass_filter2(odrive.set_speed0);

    //并级 -32.05 0 -6.205       12 5 0
    // if(cnt>=3)
	// 	{		
	// 			PWM_X = X_balance_Control(imu.rol,param.angular_zero,imu.vx);
	// 			PWM_accel = Velocity_Control(odrive.now_speed0,0);
	// 			cnt = 0;
	// 	}
    // odrive.set_speed0 = PWM_X + PWM_accel;//速度设置左正右负
    /*
    //动量轮限幅
    if(odrive.set_speed0>50) odrive.set_speed0=50;        // 动量轮电机限幅
    else if(odrive.set_speed0<-50) odrive.set_speed0=-50; // 动量轮电机限幅
		 
		//后轮开始运动 

    //摔倒停车判断
     if((imu.rol-param.angular_zero)>3 || (imu.rol-param.angular_zero)<-3){
         Flag_Stop = 1;
     }
     //else Flag_Stop = 0;

    if(Flag_Stop == 1 || g_start==0){
        odrive.set_speed0 = 0;
			  odrive_speed_ctl(0, 0);
    }
		*/
		if(RunFlag == 2)
		{

    // 舵机pid
    //Steer_Target =  Steer_Engine_control(caramera.pos_x); //舵机pid

    //Steer_Target = Steer_Speed_Limit(Steer_Target,Steer_Target_Last,3); // 舵机打角限速
    Steer_Target =  Steer_Engine_control(-(imu.rol - param.angular_zero));
		servo_set_duty(Steer_Target);//舵机控制
    Steer_Target_Last = Steer_Target;//记录上次打角值
		}
		else if(RunFlag == 3)
		{
			servo_set_duty(0);	
			odrive.set_speed0 = 0;
			odrive_speed_ctl(0, 0);
			odrive.set_speed1 = 0;
			odrive_speed_ctl(1, 0);
			if(sidewalk_flag == 1)
			{
				sidewalk_time++;
				if(sidewalk_time >= 10000)
				{
					RunFlag = 2;
				}
			}
		}

}



void ANODT_Send2(unsigned short _a,unsigned short _b,unsigned short a1,unsigned short b1,unsigned short a2,unsigned short b2)
{
    unsigned char _cnt = 0;
    unsigned char  sc = 0;
    unsigned char  ac = 0;
    unsigned char  i = 0;
    DataToSend[_cnt++] = 0xAA;
    DataToSend[_cnt++] = 0xFF;
    DataToSend[_cnt++] = 0xF1;
    DataToSend[_cnt++] = 12;

    DataToSend[_cnt++] = BYTE0(_a);
    DataToSend[_cnt++] = BYTE1(_a);

    DataToSend[_cnt++] = BYTE0(_b);
    DataToSend[_cnt++] = BYTE1(_b);

    DataToSend[_cnt++] = BYTE0(a1);
    DataToSend[_cnt++] = BYTE1(a1);

    DataToSend[_cnt++] = BYTE0(b1);
    DataToSend[_cnt++] = BYTE1(b1);

    DataToSend[_cnt++] = BYTE0(a2);
    DataToSend[_cnt++] = BYTE1(a2);

    DataToSend[_cnt++] = BYTE0(b2);
    DataToSend[_cnt++] = BYTE1(b2);

    for(i = 0; i< _cnt;i++){
            sc += DataToSend[i];
            ac += sc;
    }
    DataToSend[_cnt++] = sc;
    DataToSend[_cnt++] = ac;

    // UART_PutBuff(UART3, DataToSend, _cnt);
    //todo 串口2 发送
    // for(i = 0; i< _cnt;i++){
    // HAL_UART_Transmit_DMA(&huart2,(uint8_t*)msg,strlen(msg));
    // HAL_UART_Transmit_DMA(&huart2,(uint8_t*)DataToSend,_cnt);
    HAL_UART_Transmit(&huart2,(uint8_t*)DataToSend,_cnt,100);
}
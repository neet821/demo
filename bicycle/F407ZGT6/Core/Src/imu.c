#include "imu.h"
#include "imu_data_decode.h"
#include "packet.h"
#include "usart.h"
uint8_t rxbuf[1]={0};
float	gyr[3],eul[3];
imu_t                 imu={0};

char ggflag;
float rol=0;
float range= 0;
void imu_init(void)
{
	imu_data_decode_init();
}


//ÐèÒªÖØÐ´
void imu_task(void)
{
	// gyr[0]=id0x91.gyr[0];
	// gyr[1]=id0x91.gyr[1];
	// gyr[2]=id0x91.gyr[2];
	// eul[0]=id0x91.eul[0];
	// eul[1]=id0x91.eul[1];
	// eul[2]=id0x91.eul[2];
	imu.pit = id0x91.eul[0];
	imu.rol = id0x91.eul[1];//??  ????
	imu.yaw = id0x91.eul[2];
	imu.vx = id0x91.gyr[0];//???  ????
	imu.vy = id0x91.gyr[1];
	imu.vz = id0x91.gyr[2];	
}
int cct = 0;
void USART3_IRQHandler(void)
{
	
	cct++;
	if(LL_USART_IsActiveFlag_RXNE(USART3) && LL_USART_IsEnabledIT_RXNE(USART3))
	{
		rxbuf[0]=LL_USART_ReceiveData8(USART3);
		packet_decode(rxbuf[0]);
	//***************??????*************//
	}
		// LL_USART_ClearFlag_ORE(UART8);

	// if(LL_USART_IsActiveFlag_ORE(USART3)){
		LL_USART_ClearFlag_ORE(USART3);
		// LL_USART_ReceiveData8(UART8);
	// }
	// ggflag = LL_USART_IsActiveFlag_ORE(UART8);
		//  	if(USART_GetFlagStatus(UART8, USART_IT_ORE) != RESET)  
        //             //???USART_GetFlagStatus?????ORE????
		// {
		// 	USART_ClearFlag(UART8,USART_FLAG_ORE);//??ORE???
		// 	LL_USART_ReceiveData8(UART8);           //????????			
        //  } 
		//   //****************??????****************//

}


float fc = 2.0f;     //????
float Ts = 0.01f;    //????
float pi = 3.14159f; //?
float alpha = 0.03f;     //????

/************************ ?????? alpha *****************************/
void low_pass_filter_init(void)
{
  float b = 2.0 * pi * fc * Ts;
  alpha = b / (b + 1);
}

float low_pass_filter(float value)
{
  static float out_last = 0; //??????
  float out;

  /***************** ?????????? out_last ?? ******************/
  static char fisrt_flag = 1;
  if (fisrt_flag == 1)
  {
    fisrt_flag = 0;
    out_last = value;
  }

  /*************************** ???? *********************************/
  out = out_last + alpha * (value - out_last);
  out_last = out;

  return out;
}

float low_pass_filter2(float value)
{
  static float out_last2 = 0; //??????
  float out;

  /***************** ?????????? out_last ?? ******************/
  static char fisrt_flag2 = 1;
  if (fisrt_flag2 == 1)
  {
    fisrt_flag2 = 0;
    out_last2 = value;
  }

  /*************************** ???? *********************************/
  out = out_last2 + alpha * (value - out_last2);
  out_last2 = out;

  return out;
}
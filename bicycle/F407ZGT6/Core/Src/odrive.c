#include "odrive.h"
#include "usart.h"
#include "math.h"

extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart2;

#define IF_YELLOW 1  //如果板子为黄色动量轮电机接驱动M0，该值为1  否则该值为0 

OdirveTypeDef odrive;

//初始化结构体
void odrive_init(){
    odrive.set_speed0 = 0;//初始设定速度
    odrive.set_speed1 = 0;
    odrive.now_speed0 = 0;
    odrive.now_speed1 = 0;
    odrive.fliter_speed0[0] = 0;
    odrive.fliter_speed0[1] = 0;
    odrive.fliter_speed0[2] = 0;
    odrive.fliter_speed1[0] = 0;
    odrive.fliter_speed1[1] = 0;
    odrive.fliter_speed1[2] = 0;
    odrive.speed0_i = 0;
    odrive.speed1_i = 0;
}


// // odrive 速度闭环 num 选择电机
// //ex odrive_speed_ctl(0,odrive.set_speed0);
// void odrive_speed_ctl(unsigned char num, float speed)
// {
//     char uart5_tx_data[14];
//     unsigned char uart5_tx_i = 0;
//     uart5_tx_data[0] = 'c';
//     uart5_tx_data[1] = ' ';
//     if(num==0)
//     {
//         uart5_tx_data[2] = '0';
//     }
//     else if(num==1)
//     {
//         uart5_tx_data[2] = '1';
//     }
//         uart5_tx_data[3] = ' ';
//     if(speed<0)
//     {
//         uart5_tx_data[4] = '-';
//         // uart5_tx_data[5] = (short)(-speed)%100/10+48;
//         uart5_tx_data[5] = (short)(-speed)%10/1+48;
//         uart5_tx_data[6] = '.';
//         uart5_tx_data[7] = (short)(-speed*10)%10/1+48;
//         uart5_tx_data[8] = (short)(-speed*100)%10/1+48;
//         uart5_tx_data[9] = (short)(-speed*1000)%10/1+48;
//         uart5_tx_data[10] = (short)(-speed*10000)%10/1+48;
//         uart5_tx_data[11] = (short)(-speed*100000)%10/1+48;
//     }
//     else
//     {
//         uart5_tx_data[4] = '+';
//         // uart5_tx_data[5] = (short)(speed)%100/10+48;
//         uart5_tx_data[5] = (short)(speed)%10/1+48;
//         uart5_tx_data[6] = '.';
//         uart5_tx_data[7] = (short)(speed*10)%10/1+48;
//         uart5_tx_data[8] = (short)(speed*100)%10/1+48;
//         uart5_tx_data[9] = (short)(speed*1000)%10/1+48;
//         uart5_tx_data[10] = (short)(speed*10000)%10/1+48;
//         uart5_tx_data[11] = (short)(speed*100000)%10/1+48;
//     }
//     uart5_tx_data[12] = 0x0D;
//     uart5_tx_data[13] = 0x0A;
//     HAL_UART_Transmit(&huart6,uart5_tx_data,14,100);
//     // HAL_UART_Transmit(&huart2,uart5_tx_data,12,100);
//     // while(__HAL_UART_GET_FLAG(&huart6,UART_FLAG_TC)!=SET);		//等待发送结束
//     // while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC)!=SET);		//等待发送结束

// }


// odrive 速度闭环 num 选择电机
//ex odrive_speed_ctl(0,odrive.set_speed0);
void odrive_speed_ctl(unsigned char num, float speed)
{
    #if IF_YELLOW == 1
    num = num == 0 ? 1 : 0;
    #endif

    char uart5_tx_data[12];
    unsigned char uart5_tx_i = 0;
    uart5_tx_data[0] = 'v';
    uart5_tx_data[1] = ' ';
    if(num==0)
    {
        uart5_tx_data[2] = '0';
    }
    else if(num==1)
    {
        uart5_tx_data[2] = '1';
    }
        uart5_tx_data[3] = ' ';
    if(speed<0)
    {
        uart5_tx_data[4] = '-';
        uart5_tx_data[5] = (short)(-speed)%100/10+48;
        uart5_tx_data[6] = (short)(-speed)%10/1+48;
        uart5_tx_data[7] = '.';
        uart5_tx_data[8] = (short)(-speed*10)%10/1+48;
        uart5_tx_data[9] = (short)(-speed*100)%10/1+48;
        // uart5_tx_data[9] = (short)(-speed*1000)%10/1+48;
        // uart5_tx_data[10] = (short)(-speed*10000)%10/1+48;
        // uart5_tx_data[11] = (short)(-speed*100000)%10/1+48;
    }
    else
    {
        uart5_tx_data[4] = '+';
        uart5_tx_data[5] = (short)(speed)%100/10+48;
        uart5_tx_data[6] = (short)(speed)%10/1+48;
        uart5_tx_data[7] = '.';
        uart5_tx_data[8] = (short)(speed*10)%10/1+48;
        uart5_tx_data[9] = (short)(speed*100)%10/1+48;
        // uart5_tx_data[9] = (short)(speed*1000)%10/1+48;
        // uart5_tx_data[10] = (short)(speed*10000)%10/1+48;
        // uart5_tx_data[11] = (short)(speed*100000)%10/1+48;
    }
    uart5_tx_data[10] = 0x0D;
    uart5_tx_data[11] = 0x0A;
    HAL_UART_Transmit(&huart1,uart5_tx_data,12,100);
    // HAL_UART_Transmit(&huart2,uart5_tx_data,12,100);
    // while(__HAL_UART_GET_FLAG(&huart6,UART_FLAG_TC)!=SET);		//等待发送结束
    // while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC)!=SET);		//等待发送结束

}

//获得odrive电机速度
void odrive_feedback()
{
    #if IF_YELLOW == 1
    char* msg = "f 0\r\n";
    #else
    char* msg = "f 1\r\n";
    #endif
     HAL_UART_Transmit(&huart1,msg,strlen(msg),100);
}
float vtt = 0;

// 
void odrive_analyze_speed(char* msg,int len){
    //检验
    if(len <= 15) return;
    // if(msg[len-1] != '\n') return;   
    int msg_i;
    //解析
    for (msg_i = len-1; msg_i > 0; msg_i--) 
        if(msg[msg_i] == '.')
            break;
    if(msg_i == 0) return;
    float res = (msg[msg_i+1] - '0')*0.1 + (msg[msg_i+2] - '0')*0.01 + (msg[msg_i+3] - '0')*0.001;
    int i = 0;
    while(msg[--msg_i]!=' '){
        if(msg[msg_i] == '-') res = -res;
        else res += (msg[msg_i] - '0')*pow(10,i);
        i++;
    }
		vtt = res;
    //滤波
    odrive.speed0_i = (++odrive.speed0_i) % 3;
    odrive.fliter_speed0[odrive.speed0_i] = res;
    odrive.now_speed0 = (odrive.fliter_speed0[0]+odrive.fliter_speed0[1]+odrive.fliter_speed0[2])/3;

}
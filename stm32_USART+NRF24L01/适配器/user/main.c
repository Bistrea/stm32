/****************************************************************************
* �ļ���: main.c
* ���ݼ���:
*       ��ʾ USART1 �����յ���������0x0d 0x0aʱ�������յ������ݴ��ͳ�ȥ
                       V6��V3�壩 V2��MINI�壩 LED1��˸��ʾ�����շ����ݡ� 
*
���壺	
	TXD1----- PA9-US1-TX
	RXD1----- PA10-US1-RX
	���ʣ�9600,n,8,1 
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
uint8_t TxBufferRF[40];  
uint8_t RxBufferRF[40];
uint8_t rf_rec_flag,rf_tx_flag;

/* */
extern void SPI2_NRF24L01_Init(void);
extern void RX_Mode(void);
extern void TX_Mode(void);

/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
uint8_t TxBufferUSART[40]; 
uint8_t RxBufferUSART[40];
uint8_t usart_rec_flag,usart_tx_flag,i;
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t RxCounter1 = 0x00; 

uint32_t Rec_Len;


/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

void Delay(__IO uint32_t nCount);
void USART_Config(USART_TypeDef* USARTx);
void USART_SendChar(USART_TypeDef* USARTx,uint8_t data);//����һ��char

GPIO_InitTypeDef GPIO_InitStructure;
USART_ClockInitTypeDef USART_ClockInitStruct;
USART_InitTypeDef USART_InitStruct;


int main(void){
   uint8_t a=0;//LED�ߵ͵�ѹ����
  /* System Clocks Configuration */
  RCC_Configuration();											  //ϵͳʱ������    
  /*Ƕ�������жϿ����� 
      ˵����USART1��ռ���ȼ�����0�����1λ�� ���������ȼ�����0�����7λ�� */ 
  NVIC_Configuration();											  //�ж�Դ����
  /*�Կ���LEDָʾ�Ƶ�IO�ڽ����˳�ʼ�������˿�����Ϊ������������������ٶ�Ϊ50Mhz��PA9,PA10�˿ڸ���Ϊ����1��TX��RX��
  ������ĳ������ʱ������Ӧ�������ڵĶ˿ڵ�ʱ�ӽ���ʹ�ܡ������޷����óɹ��������õ��˶˿�B�� ���Ҫ������˿ڵ�ʱ��
  ����ʹ�ܣ�ͬʱ�����õ�����IO�ڹ����������ô��ڡ���˻�Ҫʹ��AFIO�����ù���IO��ʱ�ӡ�*/
  GPIO_Configuration();											  //�˿ڳ�ʼ��
  SPI2_NRF24L01_Init(); 										  //SPI2��NRF24L01�ӿڳ�ʼ��  
  USART_Config(USART1);											  //����1��ʼ��

  /*NRF24L01����Ϊ����ģʽ*/
  RX_Mode(); 

   while (1)
  {
	if(usart_rec_flag==1) //�ж��Ƿ��յ�һ֡��Ч����
	{												  
		usart_rec_flag=0;
		NRF_Send_Data(TxBufferRF,sizeof(TxBufferRF));
		if(a==0){GPIO_SetBits(GPIOB, GPIO_Pin_5);a=1;}          //LED1  ������˸                
		else{GPIO_ResetBits(GPIOB, GPIO_Pin_5);a=0;}
	}
	if(rf_rec_flag==1)
	{
	  	rf_rec_flag=0;
	   	for(i=0;i<32;i++)//�����ַ���
		{
			USART_SendChar(USART1,TxBufferUSART[i]);
		//	Delay(0x0000ff00);
		}
	}
  }
}
//--------------------------------------------------------------------------------------
void USART_SendChar(USART_TypeDef* USARTx,uint8_t data){
	USART_SendData(USARTx,data);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
}
//--------------------------------------------------------------------------------------
void USART_Config(USART_TypeDef* USARTx){
  USART_InitStructure.USART_BaudRate = 9600;						//����19200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

  /* Configure USART1 */
  USART_Init(USARTx, &USART_InitStructure);							//���ô��ڲ�������
 
  
  /* Enable USART1 Receive and Transmit interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);						//ʹ�ܷ��ͻ�����ж�   

  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);	
}
//--------------------------------------------------------------------------------------
void Delay(__IO uint32_t nCount){
  for(;nCount!=0;nCount--);
}
/*--------------------------------------------------------------------------------------
ϵͳʱ������Ϊ72MHZ+����ʱ������*/ 
void RCC_Configuration(void){
   SystemInit(); 
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO  , ENABLE);  
}
//-------------------------------------------------------------------------------------- 
void GPIO_Configuration(void){
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //LED1����--PB5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 //USART1 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //A�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 //USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 //A�˿� 
}
//--------------------------------------------------------------------------------------
void NVIC_Configuration(void){
 /*  �ṹ����*/
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;	    
  
  /* ���ȼ��� 1  */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		   
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			     	//���ô���1�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     	//��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				//�����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ��
  NVIC_Init(&NVIC_InitStructure);											  

																    
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;					//NRF24L01 �ж���Ӧ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		    //��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//�����ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ��
  NVIC_Init(&NVIC_InitStructure);											    

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	   //NRF24L01 IRQ  PA0
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;					   //NRF24L01 IRQ PA0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			   //EXTI�ж�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		   //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;						   //ʹ��
  EXTI_Init(&EXTI_InitStructure);	
}

/****************************************************************************
*
* �ļ���: bsp.c
* ���ݼ���:
*       �������ṩ��Ӳ��ƽ̨�ĳ�ʼ��
		
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2    2011-07-01 sun68  �������ļ�
*
*/
#include "..\App\includes.h"
#include "demo.h"
#include "NRF24L01.h"


/* �����˴���оƬ��SPIƬѡ���� */
#define TP_CS()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)	  
#define TP_DCS() GPIO_SetBits(GPIOB,GPIO_Pin_7)

void tp_Config(void) ;
u16 TPReadX(void);
u16 TPReadY(void);
extern void FSMC_LCD_Init(void); 
extern void SPI2_NRF24L01_Init(void);

extern void SPI2_NRF24L01_Init(void);
extern void RX_Mode(void);
extern void TX_Mode(void);
extern void SPI2_NRF24L01_Init(void);

extern unsigned char SPI_Read(BYTE reg);
extern unsigned char SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);
extern unsigned char SPI2_NRF_SendByte(unsigned char byte);
extern void MODE_CE(unsigned char a);
extern unsigned char SPI2_NRF_SendByte(unsigned char byte);
extern unsigned char SPI_RW_Reg(unsigned char data1,unsigned char data2);
extern unsigned char SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);
extern void RX_Mode(void);
extern void TX_Mode(void);
extern void NRF24L01_TXBUF(uint8_t* data_buffer, uint8_t Nb_bytes);

extern void USART_Config(USART_TypeDef* USARTx);
/****************************************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ������Ϊ72MHZ�� ����ʱ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void RCC_Configuration(void){

  SystemInit();	  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE , ENABLE);
}

/****************************************************************************
* ��    �ƣ�void GPIO_Configuration(void)
* ��    �ܣ�ͨ��IO������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //״̬LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 //ͨ���������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //���ģʽ����ٶ�50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 //USART1 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //A�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 //USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 //A�˿� 
}


/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void NVIC_Configuration(void)
{
  /*  �ṹ����*/
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  

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


/****************************************************************************
* ��    �ƣ�void BSP_Init(void)
* ��    �ܣ��ܶ����ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void BSP_Init(void)
{
  /*  NRF24L01��6ͨ�������ַ�ĳ�ʼ�� */
  TX_ADDRESS0[0]=0x34;	            //ͨ��0 �����ַ
  TX_ADDRESS0[1]=0x43;
  TX_ADDRESS0[2]=0x10;
  TX_ADDRESS0[3]=0x10;
  TX_ADDRESS0[4]=0x01;	 

  TX_ADDRESS1[0]=0x01;				//ͨ��1 �����ַ
  TX_ADDRESS1[1]=0xE1;
  TX_ADDRESS1[2]=0xE2;
  TX_ADDRESS1[3]=0xE3;
  TX_ADDRESS1[4]=0x02;

  TX_ADDRESS2[0]=0x02;			   //ͨ��2 �����ַ
  TX_ADDRESS2[1]=0xE1;
  TX_ADDRESS2[2]=0xE2;
  TX_ADDRESS2[3]=0xE3;
  TX_ADDRESS2[4]=0x02;

  TX_ADDRESS3[0]=0x03;			   //ͨ��3 �����ַ
  TX_ADDRESS3[1]=0xE1;
  TX_ADDRESS3[2]=0xE2;
  TX_ADDRESS3[3]=0xE3;
  TX_ADDRESS3[4]=0x02;

  TX_ADDRESS4[0]=0x04;			   //ͨ��4 �����ַ
  TX_ADDRESS4[1]=0xE1;
  TX_ADDRESS4[2]=0xE2;
  TX_ADDRESS4[3]=0xE3;
  TX_ADDRESS4[4]=0x02;

  TX_ADDRESS5[0]=0x05;			   //ͨ��5 �����ַ
  TX_ADDRESS5[1]=0xE1;
  TX_ADDRESS5[2]=0xE2;
  TX_ADDRESS5[3]=0xE3;
  TX_ADDRESS5[4]=0x02;	 
    
  RCC_Configuration();  	       //ϵͳʱ�ӳ�ʼ�����˿�����ʱ��ʹ��
  GPIO_Configuration();			   //״̬LED1�ĳ�ʼ��
  SPI2_NRF24L01_Init();			   //SPI2�ӿڼ� NRF24L01��ʼ�� 
  USART_Config(USART1);			   //��ʼ������1
  NVIC_Configuration();			   //�ж�Դ����		 
  tp_Config();					   //SPI1 ������·��ʼ��	    
  FSMC_LCD_Init();				   //FSMC TFT�ӿڳ�ʼ��  
}

/****************************************************************************
* ��    �ƣ�void  OS_CPU_SysTickInit(void)
* ��    �ܣ�ucos ϵͳ����ʱ�ӳ�ʼ��  ��ʼ����Ϊ1msһ�ν���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;


    RCC_GetClocksFreq(&rcc_clocks);		                        //���ϵͳʱ�ӵ�ֵ

    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//���ʱ�ӽ��ĵ�ֵ

	SysTick_Config(cnts);										//����ʱ�ӽ���

  
}

/****************************************************************************
* ��    �ƣ�void tp_Config(void)
* ��    �ܣ�TFT ���������Ƴ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void tp_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure; 
  SPI_InitTypeDef   SPI_InitStructure; 

  /* SPI1 ʱ��ʹ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
 
  /* SPI1 SCK(PA5)��MISO(PA6)��MOSI(PA7) ���� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //����ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* SPI1 ����оƬ��Ƭѡ�������� PB7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������ģʽ
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  /* ����SPI1�����Ϲҽ���4�����裬������ʹ�ô�����ʱ����Ҫ��ֹ����3��SPI1 ���裬 ������������ */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016BƬѡ 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                //SPI1 VS1003Ƭѡ 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //SPI1 ����ģ��Ƭѡ
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_4);			//SPI CS1
  GPIO_SetBits(GPIOB, GPIO_Pin_12);			//SPI CS4
  GPIO_SetBits(GPIOA, GPIO_Pin_4);			//SPI NSS	   
  
   /* SPI1���� ���� */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //ȫ˫��  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8λ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //ʱ�Ӽ��� ����״̬ʱ��SCK���ֵ͵�ƽ
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //�������NSS
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //�����ʿ��� SYSCLK/64
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //���ݸ�λ��ǰ
  SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC����ʽ�Ĵ�����ʼֵΪ7 
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* SPI1 ʹ�� */  
  SPI_Cmd(SPI1,ENABLE);  
}

/****************************************************************************
* ��    �ƣ�unsigned char SPI_WriteByte(unsigned char data) 
* ��    �ܣ�SPI1 д����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
unsigned char SPI_WriteByte(unsigned char data) 
{ 
 unsigned char Data = 0; 

  //�ȴ����ͻ�������
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
  // ����һ���ֽ�  
  SPI_I2S_SendData(SPI1,data); 

   //�ȴ��Ƿ���յ�һ���ֽ� 
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET); 
  // ��ø��ֽ�
  Data = SPI_I2S_ReceiveData(SPI1); 

  // �����յ����ֽ� 
  return Data; 
}  

/****************************************************************************
* ��    �ƣ�void SpiDelay(unsigned int DelayCnt) 
* ��    �ܣ�SPI1 д��ʱ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void SpiDelay(unsigned int DelayCnt)
{
 unsigned int i;
 for(i=0;i<DelayCnt;i++);
}

/****************************************************************************
* ��    �ƣ�u16 TPReadX(void) 
* ��    �ܣ�������X�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
u16 TPReadX(void)
{ 
   u16 x=0;
   TP_CS();	                        //ѡ��XPT2046 
   SpiDelay(10);					//��ʱ
   SPI_WriteByte(0x90);				//����X���ȡ��־
   SpiDelay(10);					//��ʱ
   x=SPI_WriteByte(0x00);			//������ȡ16λ������ 
   x<<=8;
   x+=SPI_WriteByte(0x00);
   SpiDelay(10);					//��ֹXPT2046
   TP_DCS(); 					    								  
   x = x>>3;						//��λ�����12λ����Ч����0-4095
   return (x);
}
/****************************************************************************
* ��    �ƣ�u16 TPReadY(void)
* ��    �ܣ�������Y�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
u16 TPReadY(void)
{
   u16 y=0;
   TP_CS();	                        //ѡ��XPT2046 
   SpiDelay(10);					//��ʱ
   SPI_WriteByte(0xD0);				//����Y���ȡ��־
   SpiDelay(10);					//��ʱ
   y=SPI_WriteByte(0x00);			//������ȡ16λ������ 
   y<<=8;
   y+=SPI_WriteByte(0x00);
   SpiDelay(10);					//��ֹXPT2046
   TP_DCS(); 					    								  
   y = y>>3;						//��λ�����12λ����Ч����0-4095
   return (y);
}



/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/


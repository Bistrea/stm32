/**
  ******************************************************************************
  * @file USART/Interrupt/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and 
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"
#include "NRF24L01.h"
#include "globals.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t TxBufferUSART[40];  
extern uint8_t RxBufferUSART[40];
extern __IO uint8_t TxCounter1;
extern __IO uint8_t RxCounter1; 

extern uint8_t usart_rec_flag,usart_tx_flag;

extern uint8_t TxBufferRF[40];  
extern uint8_t RxBufferRF[40];
extern uint8_t rf_rec_flag,rf_tx_flag;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval : None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval : None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval : None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval : None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval : None
  */
void USART1_IRQHandler(void)      //����1 �жϷ������
{
  unsigned int i;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //�ж϶��Ĵ����Ƿ�ǿ�
  {	
    
    RxBufferUSART[RxCounter1++] = USART_ReceiveData(USART1);   //�����Ĵ��������ݻ��浽���ջ�������
	
    if(RxBufferUSART[RxCounter1-2]==0x0d&&RxBufferUSART[RxCounter1-1]==0x0a)     //�жϽ�����־�Ƿ���0x0d 0x0a
    {
	  for(i=0; i< RxCounter1; i++) TxBufferRF[i] = RxBufferUSART[i]; 	     //�����ջ�����������ת�����ͻ�������׼��ת��
	  usart_rec_flag=1;															 //���ճɹ���־
	  TxBufferRF[RxCounter1]=0;		                                     //���ͻ�����������    
	  TxCounter1=RxCounter1;
	  RxCounter1=0;
    }
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG 
  { 
     USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					     //��ֹ�����������жϣ� 
  }	
}  
/*******************************************************************************
* Function Name  : EXTI0 �жϺ���
* Description    : NRF24L01�жϷ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void){
	u8 i=0;
 	u8 status;	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)		    //�ж��Ƿ������EXTI0�ж�
  	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0){ //�ж��Ƿ���PA0�߱��			
			status=SPI_Read(READ_REG1+STATUS);			// ��ȡ״̬�Ĵ������ж����ݽ���״��	
			if(status & 0x40)				    		// �ж��Ƿ���յ�����				   
			{			
				//GPIO_ResetBits(GPIOB, GPIO_Pin_5);   
			 	SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);  //�ӽ��ջ��������������
				for(i=0; i<32; i++){							  //��USB �˵�1�Ļ��������������
					TxBufferUSART[i] = rx_buf[i];     
				}
				rf_rec_flag=1;
			}
			else if((status &0x10)>0){					 //����ﵽ��󸴷�����				
				SPI_RW_Reg(0xe1,0);					 	 //������ͻ�����  				
				RX_Mode();								 //�������ģʽ	   			
			}
			else if((status &0x20)>0){					 //������յ�Ӧ�� 
				GPIO_SetBits(GPIOB, GPIO_Pin_5);   
				SPI_RW_Reg(0xe1,0);					     //������ͻ�����  			
				RX_Mode();								 //�������ģʽ	   			
			}
			SPI_RW_Reg(WRITE_REG1+STATUS, status);	     //���07�Ĵ�����־
		}		
		EXTI_ClearITPendingBit(EXTI_Line0);			 //���EXTI0�ϵ��жϱ�־              
	} 
}

#include "includes.h"

//--------------------------------------------------------------------------------------
void USART_SendChar(USART_TypeDef* USARTx,uint8_t data)
{
	USART_SendData(USARTx,data);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
}
//--------------------------------------------------------------------------------------
void USART_Config(USART_TypeDef* USARTx)
{
	USART_InitTypeDef USART_InitStructure;
	
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
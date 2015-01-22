/****************************************************************************
*
* �ļ���: Fun.c
* ���ݼ���:
*       �����̲���ucGUI3.90�汾�� ʹ���˶Ի����б�򣬵�ѡ���ı��� �༭�򣬰�ť�Ȳ���
*
*/
#include <stddef.h>           /* needed for definition of NULL */
#include "..\GUIinc\GUI.h"
#include "..\GUIinc\WM.h"
#include "..\GUIinc\BUTTON.h"
#include "..\GUIinc\DIALOG.h"
#include "..\GUIinc\LISTBOX.h"
#include "..\GUIinc\EDIT.h"
#include "..\GUIinc\FRAMEWIN.h"	
#include "..\GUIinc\SLIDER.h"	
#include "demo.h"
#include "NRF24L01.h"

void Fun(void);
extern void RX_Mode(void);
extern void TX_Mode(void);	
extern void RX_Mode(void);
extern void TX_Mode(void);
extern void NRF24L01_TXBUF(uint8_t* data_buffer, uint8_t Nb_bytes);

extern void USART_SendChar(USART_TypeDef* USARTx,uint8_t data);
uint8_t control_data,speed_change_flag,i;
/* ucgui���Ͷ���*/ 
WM_HWIN hWin;
WM_HWIN hListBox[8];
WM_HWIN text1,text2,text3,text4,text5,text6,bt[2],edit1,edit2,slider0,rd0,list1,slider1,slider2,slider3,slider4,slider5;
WM_HWIN text_speed1,text_speed2,text_speed3,text_speed4,text_speed5;	
GUI_COLOR DesktopColorOld;
const GUI_FONT* pFont = &GUI_FontComic24B_1;
const GUI_FONT* pFont18 = &GUI_FontComic18B_1;
 
int speed1,speed2,speed3,speed4,speed5;//��������������ٶ�
char speed_show[3];//������speedxת��Ϊchar*Ȼ����ʾ�õ�
/* �����˶Ի�����Դ�б� */
static const GUI_WIDGET_CREATE_INFO aDialogCreate[] = {
  //�������壬 ��С��800X480  ԭ����0��0
  { FRAMEWIN_CreateIndirect, "http://beautifulzzzz", 0,0,0, 800, 480, FRAMEWIN_CF_ACTIVE },
  { BUTTON_CreateIndirect,   "SEND",    GUI_ID_BUTTON0,       0,    395,  200,  55 },
 
  { BUTTON_CreateIndirect,   "CLEAR", GUI_ID_BUTTON2,   200,    395, 200,  55 },
  { EDIT_CreateIndirect,     "",       GUI_ID_EDIT1,    0,   190, 400,  65, EDIT_CF_LEFT, 50 },
  { EDIT_CreateIndirect,     "",       GUI_ID_EDIT2,    0,   290, 400,  65, EDIT_CF_LEFT, 50 },
  
  //����TEXT�ؼ�������Ǵ����X��X����СXXY  ���������
  { TEXT_CreateIndirect,     "Send Text Area",  GUI_ID_TEXT0,   1,   160,  400,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Receive Text Area ",  GUI_ID_TEXT1,     1,   263,  400, 25, TEXT_CF_LEFT },
 
  { TEXT_CreateIndirect,     "2M bps",  GUI_ID_TEXT2,   23,   22,  140,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "1M bps",  GUI_ID_TEXT3,     23,   42,  140,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "250K bps",  GUI_ID_TEXT5,     23,   62,  140,  25, TEXT_CF_LEFT },
  
  { TEXT_CreateIndirect,     "",  GUI_ID_TEXT4,     0,   120,  400,  25, TEXT_CF_LEFT },

  { RADIO_CreateIndirect,     "Receive Mode",  GUI_ID_RADIO0,     3,   33,  40,  52, RADIO_TEXTPOS_LEFT,3},

  { LISTBOX_CreateIndirect,  "",       GUI_ID_LISTBOX0,  134,    13,  130,  90, 0, 0 },

   //��������
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER1,  440,  60, 320, 25, 0, 0 },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER2,  440,  120, 320, 25, 0, 0 },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER3,  440,  180, 320, 25, 0, 0 },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER4,  440,  240, 320, 25, 0, 0 },
  { SLIDER_CreateIndirect,   NULL,     GUI_ID_SLIDER5,  440,  300, 320, 25, 0, 0 },
  //���������Ӧ��text
  { TEXT_CreateIndirect,     "0",  GUI_ID_TEXT_SPEED1,   770,   60,   25,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "0",  GUI_ID_TEXT_SPEED2,   770,   120,  25,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "0",  GUI_ID_TEXT_SPEED3,   770,   180,  25,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "0",  GUI_ID_TEXT_SPEED4,   770,   240,  25,  25, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "0",  GUI_ID_TEXT_SPEED5,   770,   300,  25,  25, TEXT_CF_LEFT },
};

/*  ������NRF24L01ͨ��ѡ���б��ĳ�ʼ��Ŀ */
static const GUI_ConstString _apListBox[] = {
  "Pipe 0", "Pipe 1","Pipe 2","Pipe 3","Pipe 4","Pipe 5", NULL
};


/****************************************************************************
* ��    �ƣ�static void _cbCallback(WM_MESSAGE * pMsg)
* ��    �ܣ�ucgui�ص�����������Ϊ�Ի�������Ӧ�ĺ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
static void _cbCallback(WM_MESSAGE * pMsg) {
  int NCode, Id;
  switch (pMsg->MsgId) {
    case WM_NOTIFY_PARENT:				  		//֪ͨ���������¼��ڴ��ڲ����Ϸ���
      Id    = WM_GetId(pMsg->hWinSrc);    		//��öԻ��򴰿��﷢���¼��Ĳ�����ID
      NCode = pMsg->Data.v;               		//֪ͨ����
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:    		//���岿���������ͷ�             
          if (Id == GUI_ID_BUTTON2) {     		//����CLEAR���ɿ�
            memcpy(status_buf, "", 20);	  		//���״̬�ı������� 
			memcpy(rx_buf, "", 32);		  		//��ս����ı������� 		
			TEXT_SetText(text5,(const char *)status_buf);	   	//���״̬�ı���	
			EDIT_SetText(edit2,(const char *)rx_buf);			//��ս����ַ��༭��
			memcpy(tx_buf, "", 32);				//��շ����ı�������
			NRF24L01_TXBUF(tx_buf,32);			//�������ַ����������ַ�ͨ��NRF24L01���ͳ�ȥ		
          }          
		  else if (Id == GUI_ID_BUTTON0) {      //����SEND ���ɿ�
		  	memcpy(tx_buf, "1234567890abcdefghij!@#$%^&*()-=", 32);	  //��32�ֽڵ��ı������������ı�������
			memcpy(rx_buf, "", 32);				//��ս����ı������� 	
			memcpy(status_buf, "", 20);			//���״̬�ı������� 
			EDIT_SetText(edit2,(const char *)rx_buf);			//��ս����ַ��༭��	
			NRF24L01_TXBUF(tx_buf,32);			//�������ַ����������ַ�ͨ��NRF24L01���ͳ�ȥ
			memcpy(tx_buf, "", 32);				//��շ����ı�������
			TEXT_SetText(text5,(const char *)status_buf);		//���״̬�ı���   			
          }
		  else if (Id == GUI_ID_RADIO0) {       //NRF24L01�������ʵ�ѡ���ѡ�������
		  	nrf_baud= RADIO_GetValue(rd0);		//������ʱ�ʾֵ
		    RX_Mode();							//�������ģʽ			
          }
		  else if (Id == GUI_ID_LISTBOX0){      //NRF24L01����ͨ��ѡ����
			nrf_Pipe= LISTBOX_GetSel(list1);	//���NRF24LL01����ͨ����ʾֵ 			
		    RX_Mode();   						//�������ģʽ 	
		  }else if(Id == GUI_ID_SLIDER1){       //slider1 ��ֵ���ı�
			speed1=SLIDER_GetValue(slider1);//���slider1��ֵ
			if(speed1>0){
			   speed_show[0]='+';
			   speed_show[1]='0'+speed1;
			   control_data=8+speed1;
			}else if(speed1<0){
			   speed_show[0]='-';
			   speed_show[1]='0'-speed1;
			   control_data=16-speed1;
			}else{
			   speed_show[0]=' ';
			   speed_show[1]='0';
			   control_data=0;
			}
//			USART_SendChar(USART1,control_data);
			TEXT_SetText(text_speed1,speed_show);
			speed_change_flag=1;	                 
		  }else if(Id == GUI_ID_SLIDER2){       //slider2 ��ֵ���ı�
			speed2=SLIDER_GetValue(slider2);//���slider2��ֵ
			if(speed2>0){
			   speed_show[0]='+';
			   speed_show[1]='0'+speed2;
			   control_data=32+8+speed2;
			}else if(speed2<0){
			   speed_show[0]='-';
			   speed_show[1]='0'-speed2;
			   control_data=32+16-speed2;
			}else{
			   speed_show[0]=' ';
			   speed_show[1]='0';
			   control_data=0;
			}
			TEXT_SetText(text_speed2,speed_show);
			speed_change_flag=1;	  	  
		  }else if(Id == GUI_ID_SLIDER3){       //slider3 ��ֵ���ı�
			speed3=SLIDER_GetValue(slider3);//���slider3��ֵ
			if(speed3>0){
			   speed_show[0]='+';
			   speed_show[1]='0'+speed3;
			   control_data=64+8+speed3;
			}else if(speed3<0){
			   speed_show[0]='-';
			   speed_show[1]='0'-speed3;
			   control_data=64+16-speed3;
			}else{
			   speed_show[0]=' ';
			   speed_show[1]='0';
			   control_data=0;
			}
			TEXT_SetText(text_speed3,speed_show);
			speed_change_flag=1;	  
		  }else if(Id == GUI_ID_SLIDER4){       //slider4 ��ֵ���ı�
		    speed4=SLIDER_GetValue(slider4);//���slider4��ֵ
			if(speed4>0){
			   speed_show[0]='+';
			   speed_show[1]='0'+speed4;
			   control_data=96+8+speed4;
			}else if(speed4<0){
			   speed_show[0]='-';	  
			   speed_show[1]='0'-speed4;
			   control_data=96+16-speed4;
			}else{
			   speed_show[0]=' ';
			   speed_show[1]='0';
			   control_data=0;
			}
			TEXT_SetText(text_speed4,speed_show);
			speed_change_flag=1;	  
		  }else if(Id == GUI_ID_SLIDER5){       //slider5 ��ֵ���ı�
			speed5=SLIDER_GetValue(slider5);//���slider5��ֵ
			if(speed5>0){
			   speed_show[0]='+';
			   speed_show[1]='0'+speed5;
			   control_data=128+8+speed5;
			}else if(speed5<0){
			   speed_show[0]='-';
			   speed_show[1]='0'-speed5;
			   control_data=128+16-speed5;
			}else{
			   speed_show[0]=' ';
			   speed_show[1]='0';
			   control_data=0;
			}
			TEXT_SetText(text_speed5,speed_show);
			speed_change_flag=1;	  
		  }
		 break;
		 default: break;
      }	    
    default:
      WM_DefaultProc(pMsg);						//Ĭ�ϳ�����������Ϣ
	  break;
  }
}


/****************************************************************************
* ��    �ƣ�void Fun(void)
* ��    �ܣ���ʾ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Fun(void) {   
  GUI_CURSOR_Show();                                        //�����ͼ����ʾ  
  
  /* �����Ի���ʱ����������Դ�б���Դ��Ŀ�� ����ָ�������ڶ�����Ӧ�Ļص�����  */
  hWin = GUI_CreateDialogBox(aDialogCreate, GUI_COUNTOF(aDialogCreate), _cbCallback, 0, 0, 0);

  FRAMEWIN_SetFont(hWin, &GUI_FontComic18B_1);	  			//�Ի����������� 
  FRAMEWIN_SetClientColor(hWin, GUI_BLACK);		  			//�Ի���Ĵ�����ɫ�Ǻ�ɫ
  memcpy(tx_buf, "1234567890abcdefghij!@#$%^&*()-=", 32);   //������Ϊ32�ֽڵķ����ַ������������ͻ�������
  memcpy(rx_buf, "", 32);									//�����ջ��������
 
  /* ����ı����� */		
  text1 = WM_GetDialogItem(hWin, GUI_ID_TEXT0);				//��öԻ�����GUI_ID_TEXT0��Ŀ(�ı���Send Text Area)�ľ��
  text2 = WM_GetDialogItem(hWin, GUI_ID_TEXT1);				//��öԻ�����GUI_ID_TEXT1��Ŀ(�ı���Receive Text Area)�ľ��
  text3 = WM_GetDialogItem(hWin, GUI_ID_TEXT2);				//��öԻ�����GUI_ID_TEXT2��Ŀ(�ı���2M BPS)�ľ��
  text4 = WM_GetDialogItem(hWin, GUI_ID_TEXT3);				//��öԻ�����GUI_ID_TEXT3��Ŀ(�ı���1M BPS)�ľ��
  text6 = WM_GetDialogItem(hWin, GUI_ID_TEXT5);				//��öԻ�����GUI_ID_TEXT5��Ŀ(�ı���250K BPS)�ľ��
  text5 = WM_GetDialogItem(hWin, GUI_ID_TEXT4);				//��öԻ�����GUI_ID_TEXT4��Ŀ(״̬�ַ��ı���)�ľ��  
  /* �����ı������� */
  TEXT_SetFont(text1,pFont);							    //���öԻ������ı���Send Text Area������
  TEXT_SetFont(text2,pFont);								//���öԻ������ı���Receive Text Area������
  TEXT_SetFont(text3,pFont18);								//���öԻ������ı���2M BPS������
  TEXT_SetFont(text4,pFont18);								//���öԻ������ı���1M BPS������
  TEXT_SetFont(text6,pFont18);								//���öԻ������ı���250K BPS������
  TEXT_SetFont(text5,pFont);								//���öԻ�����״̬�ַ��ı��������
  /* �����ı�����ɫ */
  TEXT_SetTextColor(text1,GUI_GREEN);						//���öԻ������ı���Send Text Area��������ɫ
  TEXT_SetTextColor(text2,GUI_GREEN );						//���öԻ������ı���Receive Text Area��������ɫ
  TEXT_SetTextColor(text3,GUI_YELLOW);						//���öԻ������ı���2M BPS��������ɫ
  TEXT_SetTextColor(text4,GUI_YELLOW);						//���öԻ������ı���1M BPS��������ɫ
  TEXT_SetTextColor(text6,GUI_YELLOW);   					//���öԻ������ı���250K BPS��������ɫ
  TEXT_SetTextColor(text5,GUI_YELLOW);						//���öԻ�����״̬�ַ��ı����������ɫ
  TEXT_SetBkColor(text5,GUI_BLUE);							//���öԻ�����״̬�ַ��ı���ı�����ɫ

  /* �༭����� */
  edit1 = WM_GetDialogItem(hWin, GUI_ID_EDIT1);				//��öԻ�����GUI_ID_EDIT1��Ŀ(�༭�� �����ַ�����ʾ��)�ľ��
  EDIT_SetFont(edit1,pFont18);								//���öԻ�����༭�� �����ַ�����ʾ��������
  EDIT_SetText(edit1,(const char *)tx_buf);					//���öԻ�����༭�� �����ַ�����ʾ�����ַ���
  edit2 = WM_GetDialogItem(hWin, GUI_ID_EDIT2);			    //��öԻ�����GUI_ID_EDIT2��Ŀ(�༭�� �����ַ�����ʾ��)�ľ��
  EDIT_SetFont(edit2,pFont18);								//���öԻ�����༭�� �����ַ�����ʾ��������
  EDIT_SetText(edit2,(const char *)rx_buf);					//���öԻ�����༭�� �����ַ�����ʾ�����ַ���

  /* ��ť��� */
  bt[0]=WM_GetDialogItem(hWin,GUI_ID_BUTTON0);				//��öԻ�����GUI_ID_BUTTON0��Ŀ(����SEND)�ľ��
  bt[1]=WM_GetDialogItem(hWin, GUI_ID_BUTTON2);				//��öԻ�����GUI_ID_BUTTON2��Ŀ(����CLEAR)�ľ��
  BUTTON_SetFont(bt[0],pFont);								//���öԻ����ﰴ��SEND������
  BUTTON_SetFont(bt[1],pFont);								//���öԻ����ﰴ��CLEAR������
  BUTTON_SetTextColor(bt[0],0,GUI_WHITE); 					//���öԻ����ﰴ��SENDδ�����µ�������ɫ
  BUTTON_SetTextColor(bt[1],0,GUI_WHITE);					//���öԻ����ﰴ��CLEARδ�����µ�������ɫ

  /* List��� */			
  nrf_Pipe=0;												//NRF24L01��ʼ����ͨ������Ϊ0
  list1 = WM_GetDialogItem(hWin, GUI_ID_LISTBOX0);			//��öԻ�����GUI_ID_LISTBOX0��Ŀ(�б��-ͨ��ѡ��)�ľ��     
  LISTBOX_SetText(list1, _apListBox);						//���öԻ������б��-ͨ��ѡ�������Ŀ
  LISTBOX_SetFont(list1,pFont18);							//���öԻ������б��-ͨ��ѡ�������
  LISTBOX_SetSel(list1,nrf_Pipe);							//���öԻ������б��-ͨ��ѡ��Ľ���ѡ��
  SCROLLBAR_CreateAttached(list1, SCROLLBAR_CF_VERTICAL);	//���öԻ������б��-ͨ��ѡ��ľ�����Ϊ����		

  /* Radio��ť��� */		
  rd0 = WM_GetDialogItem(hWin, GUI_ID_RADIO0);				//��öԻ�����GUI_ID_RADIO0��Ŀ(��ѡ��-����ѡ��)�ľ��
  nrf_baud=0;												//NRF24L01���� ��ʼΪ2MPS
  RADIO_SetValue(rd0,0);									//���öԻ������ѡ��-����ѡ��Ľ���ѡ��
  RX_Mode();												//NRF24L01�������ģʽ 
 
  /* ���slider�����ľ�� */    
  slider1 = WM_GetDialogItem(hWin, GUI_ID_SLIDER1);
  slider2 = WM_GetDialogItem(hWin, GUI_ID_SLIDER2);
  slider3 = WM_GetDialogItem(hWin, GUI_ID_SLIDER3);
  slider4 = WM_GetDialogItem(hWin, GUI_ID_SLIDER4);
  slider5 = WM_GetDialogItem(hWin, GUI_ID_SLIDER5);
  /* ����slider������ȡֵ��Χ-8-8*/  
  SLIDER_SetRange(slider1,-8,8);	
  SLIDER_SetRange(slider2,-8,8);
  SLIDER_SetRange(slider3,-8,8);
  SLIDER_SetRange(slider4,-8,8);
  SLIDER_SetRange(slider5,-8,8);
  /* ����slider������ֵ*/      
  SLIDER_SetValue(slider1,0);  
  SLIDER_SetValue(slider2,0);    
  SLIDER_SetValue(slider3,0);  
  SLIDER_SetValue(slider4,0);  
  SLIDER_SetValue(slider5,0);  
  /* ��ȡ�ı����� */
  text_speed1 = WM_GetDialogItem(hWin, GUI_ID_TEXT_SPEED1);	
  text_speed2 = WM_GetDialogItem(hWin, GUI_ID_TEXT_SPEED2);	
  text_speed3 = WM_GetDialogItem(hWin, GUI_ID_TEXT_SPEED3);	
  text_speed4 = WM_GetDialogItem(hWin, GUI_ID_TEXT_SPEED4);	
  text_speed5 = WM_GetDialogItem(hWin, GUI_ID_TEXT_SPEED5);	
  /* �����ı������� */
  TEXT_SetFont(text_speed1,pFont18);
  TEXT_SetFont(text_speed2,pFont18);
  TEXT_SetFont(text_speed3,pFont18);
  TEXT_SetFont(text_speed4,pFont18);
  TEXT_SetFont(text_speed5,pFont18);									
  /* �����ı�����ɫ */
  TEXT_SetTextColor(text_speed1,GUI_YELLOW);		
  TEXT_SetTextColor(text_speed2,GUI_YELLOW);		
  TEXT_SetTextColor(text_speed3,GUI_YELLOW);		
  TEXT_SetTextColor(text_speed4,GUI_YELLOW);		
  TEXT_SetTextColor(text_speed5,GUI_YELLOW);		

  speed_change_flag=0;

  while (1)
  {	   		
	if(Rx_Succ==1){	                                        //��NRF24L01���յ���Ч����
		EDIT_SetText(edit2,(const char *)rx_buf);		    //�����ջ��������ַ�д�뵽�����ַ��༭����
		TEXT_SetText(text5,(const char *)status_buf);		//��״̬�ı����������ַ�д�뵽״̬�ı�����
		Rx_Succ=0; 
//		for(i=0;i<sizeof(rx_buf);i++)		
//			USART_SendChar(USART1,rx_buf[i]);
	}
	if(speed_change_flag!=0)
	{
	   speed_change_flag=0;
	   USART_SendChar(USART1,control_data);
	}
	WM_Exec();									            //ˢ����Ļ
  }	  
}
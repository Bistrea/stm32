#include "stm32f10x.h"
#include "lcd.h"
#include "lcd_font.h"
#include "lcdconf.h"


extern void LCD_WR_REG(unsigned char index);
extern void LCD_WR_CMD(unsigned int index,unsigned int val);

extern void LCD_WR_Data(unsigned int val);
extern void LCD_clear(unsigned int p);
u16 lcd_ReadData(void);
extern unsigned int LCD_RD_data(void);

extern unsigned int  HDP;
extern unsigned int  HT;
extern unsigned int  HPS;
extern unsigned int  LPS;
extern unsigned char HPW;

extern unsigned int  VDP;
extern unsigned int  VT;
extern unsigned int  VPS;
extern unsigned int  FPS;
extern unsigned char VPW;

/****************************************************************************
* ��    �ƣ�void lcd_Initializtion()
* ��    �ܣ���ʼ�� lcd ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_Initializtion();
****************************************************************************/
void lcd_Initializtion()
{ 
   LCD_Init1();
}

/****************************************************************************
* ��    �ƣ�void lcd_SetCursor(u16 x,u16 y)
* ��    �ܣ�������Ļ����
* ��ڲ�����x      ������
*           y      ������
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_SetCursor(10,10);
****************************************************************************/
void lcd_SetCursor(u16 x,u16 y)
{	
 
	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002A;
  	*(__IO uint16_t *) (Bank1_LCD_D)= x>>8; 	
  	*(__IO uint16_t *) (Bank1_LCD_D)= x&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= HDP>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= HDP&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002b; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= y>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= y&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= VDP>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= VDP&0x00ff; 
}

/****************************************************************************
* ��    �ƣ�void lcd_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
* ��    �ܣ����ô�������
* ��ڲ�����StartX     ����ʼ����
*           StartY     ����ʼ����
*           EndX       �н�������
*           EndY       �н�������
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_SetWindows(0,0,100,100)��
****************************************************************************/
void lcd_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
    *(__IO uint16_t *) (Bank1_LCD_C)= 0x002A;
  	*(__IO uint16_t *) (Bank1_LCD_D)= StartX>>8; 	
  	*(__IO uint16_t *) (Bank1_LCD_D)= StartX&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= EndX>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= EndX&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002b; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= StartY>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= StartY&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= EndY>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= EndY&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002c;  

}

/****************************************************************************
* ��    �ƣ�void lcd_Clear(u16 dat)
* ��    �ܣ�����Ļ����ָ������ɫ��������������� 0xffff
* ��ڲ�����dat      ���ֵ
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_Clear(0xffff);
****************************************************************************/
void lcd_Clear(u16 dat)
{
    unsigned int l=480,w;
	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002A;
  	*(__IO uint16_t *) (Bank1_LCD_D)= 0>>8; 	
  	*(__IO uint16_t *) (Bank1_LCD_D)= 0&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= HDP>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= HDP&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002b; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= 0>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= 0&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= VDP>>8; 
  	*(__IO uint16_t *) (Bank1_LCD_D)= VDP&0x00ff; 
  	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002c;  
	while(l--)
	{
	    for(w=0;w<272;w++)
		{    
          	*(__IO uint16_t *) (Bank1_LCD_D)=dat;
			//LCD_WR_Data(dat);
		}
	}
}

/****************************************************************************
* ��    �ƣ�u16 lcd_GetPoint(u16 x,u16 y)
* ��    �ܣ���ȡָ���������ɫֵ
* ��ڲ�����x      ������
*           y      ������
* ���ڲ�������ǰ������ɫֵ
* ˵    ����
* ���÷�����i=lcd_GetPoint(10,10);
****************************************************************************/
u16 lcd_GetPoint(u16 x,u16 y)
{
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x002A;
  *(__IO uint16_t *) (Bank1_LCD_D)= x>>8; 	
  *(__IO uint16_t *) (Bank1_LCD_D)= x&0x00ff; 
  *(__IO uint16_t *) (Bank1_LCD_D)= HDP>>8; 
  *(__IO uint16_t *) (Bank1_LCD_D)= HDP&0x00ff; 
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x002b; 
  *(__IO uint16_t *) (Bank1_LCD_D)= y>>8; 
  *(__IO uint16_t *) (Bank1_LCD_D)= y&0x00ff; 
  *(__IO uint16_t *) (Bank1_LCD_D)= VDP>>8; 
  *(__IO uint16_t *) (Bank1_LCD_D)= VDP&0x00ff; 
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x002e;  

  return (lcd_ReadData());
}

/****************************************************************************
* ��    �ƣ�void lcd_SetPoint(u16 x,u16 y,u16 point)
* ��    �ܣ���ָ�����껭��
* ��ڲ�����x      ������
*           y      ������
*           point  �����ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_SetPoint(10,10,0x0fe0);
****************************************************************************/
void lcd_SetPoint(u16 x,u16 y,u16 point)
{
    *(__IO uint16_t *) (Bank1_LCD_C)= 0x002A;
	*(__IO uint16_t *) (Bank1_LCD_D)= x>>8; 	
	*(__IO uint16_t *) (Bank1_LCD_D)= x&0x00ff; 
	*(__IO uint16_t *) (Bank1_LCD_D)= HDP>>8; 
	*(__IO uint16_t *) (Bank1_LCD_D)= HDP&0x00ff; 
	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002b; 
	*(__IO uint16_t *) (Bank1_LCD_D)= y>>8; 
	*(__IO uint16_t *) (Bank1_LCD_D)= y&0x00ff; 
	*(__IO uint16_t *) (Bank1_LCD_D)= VDP>>8; 
	*(__IO uint16_t *) (Bank1_LCD_D)= VDP&0x00ff; 
	*(__IO uint16_t *) (Bank1_LCD_C)= 0x002c;  
 	*(__IO uint16_t *) (Bank1_LCD_D)= point;

}


/****************************************************************************
* ��    �ƣ�void lcd_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* ��    �ܣ���ָ��������ʾһ��8x16�����ascii�ַ�
* ��ڲ�����x          ������
*           y          ������
*           charColor  �ַ�����ɫ
*           bkColor    �ַ�������ɫ
* ���ڲ�������
* ˵    ������ʾ��Χ�޶�Ϊ����ʾ��ascii��
* ���÷�����lcd_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void lcd_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u8 tmp_char=0;

  for (i=0;i<16;i++)
  {
    tmp_char=ascii_8x16[((c-0x20)*16)+i];
    for (j=0;j<8;j++)
    {
      if ( (tmp_char >> 7-j) & 0x01 == 0x01)
        {
          lcd_SetPoint(x+j,y+i,charColor); // �ַ���ɫ
        }
        else
        {
          lcd_SetPoint(x+j,y+i,bkColor); // ������ɫ
        }
    }
  }
}


/****************************************************************************
* ��    �ƣ�u16 lcd_BGR2RGB(u16 c)
* ��    �ܣ�RRRRRGGGGGGBBBBB ��Ϊ BBBBBGGGGGGRRRRR ��ʽ
* ��ڲ�����c      BRG ��ɫֵ
* ���ڲ�����RGB ��ɫֵ
* ˵    �����ڲ���������
* ���÷�����
****************************************************************************/
u16 lcd_BGR2RGB(u16 c)
{
  u16  r, g, b;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;
  
  return( (b<<11) + (g<<5) + (r<<0) );
}



/****************************************************************************
* ��    �ƣ�u16 lcd_ReadData(void)
* ��    �ܣ���ȡ����������
* ��ڲ�������
* ���ڲ��������ض�ȡ��������
* ˵    �����ڲ�����
* ���÷�����i=lcd_ReadData();
****************************************************************************/
u16 lcd_ReadData(void)
{
  u16 val=0;
  val=*(__IO uint16_t *) (Bank1_LCD_D);
  //LCD_RD_data();
  return val;
}


/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/

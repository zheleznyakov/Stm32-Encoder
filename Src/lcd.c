#include "lcd.h"
#include "stm32f1xx_hal.h"
#include "string.h"

// sendData отправляет байт данных по шине i2c
// pData - байт, например 0x25, где 2 (0010) это DB7-DB4 или DB3-DB0, а 5(0101) это сигналы LED, E, RW, RS соответственно
// более подробная информация о работе дислея по четырехбитному интерфейсу в datasheet к дисплею
void sendData(LCD1602 *scr , uint8_t *pData)
{
  *pData |= (1<<2);// устанавливаем стробирующий сигнал E в 1
  while(HAL_I2C_Master_Transmit(scr->hi2c,scr->DevAddress,pData,1,1000)!=HAL_OK)
  {}
  HAL_Delay(5); 
  *pData &=~(1<<2);// устанавливаем стробирующий сигнал E в 0
  while(HAL_I2C_Master_Transmit(scr->hi2c,scr->DevAddress,pData,1,1000)!=HAL_OK)
  {}
  HAL_Delay(5);
}
// initLCD функция начальной инициализации дисплея
// выполняет инструкции инициализации дисплея для четырехбитного интерфейса в соответствии с плгоритмом из datasheet
void initLCD(LCD1602 *scr)
{
  uint8_t buf;
  buf = 0x30; // 0b00110000

  HAL_Delay(20);   //ждем 15ms пока устанавливается питающее напряжение (например после включения устройства)
  
  sendData(scr,&buf); //отправляем байт данныx buf
  sendData(scr,&buf); //три раза
  sendData(scr,&buf); //в соответствии с инструкцией по инициализации дисплея
  
  buf = 0x20; // 0b00100000 выбор 4х битного интерфейса
  sendData(scr,&buf);
  
  // выбор количетва строк дисплея и шрифт 0x00101000 (смотри datasheet к дисплею)
  // команда отправляется за два раза, так как четырехбитный интерфейс
  buf = 0x20; // 0x20 в двоичном виде 0010 0000: 0010-первый полубайт команды, 0000 - сигналы rs,rw,e,led
  sendData(scr,&buf);
  buf = 0xC0; //1100 0000 N=1 две строки F=1 (5*10), 0000 - сигналы rs,rw,e,led
  sendData(scr,&buf);


  //display off
  buf =0;
  sendData(scr,&buf);
  buf = 0x80;
  sendData(scr,&buf);
  //display clear
  buf =0;
  sendData(scr,&buf);
  buf = 0x10;
  sendData(scr,&buf);
  
  //I/D - установка направления движения курсора после ввода символа. (1 - влево, 0 - вправо)
  //S - сдвиг курсора сопровождается сдвигом символов.
  buf =0;
  sendData(scr,&buf);
  buf = 0x30;
  sendData(scr,&buf);
  
  //включаем дисплей
  buf =0;
  sendData(scr,&buf);
  buf = 0xC8;
  sendData(scr,&buf);
  
}
//writeLCD выводим символ s на экран scr
void writeLCD(LCD1602 *scr, uint8_t s)
{
    uint8_t command;
    command = ((s&0xf0)|0x09); //формируем верхний полубайт в команду для дисплея
    sendData(scr,&command);
    
    command = ((s&0x0f)<<4)|0x09;// формируем нижний полубайт в команду для дисплея
    sendData(scr,&command);

}
void writeStringLCD(LCD1602 *scr,char *str)
{
  for (int i=0;i<strlen(str);i++)
  {
    writeLCD(scr,str[i]);
  }
}
// moveXY переместить курсор в позицию X, Y
void moveXY(LCD1602 *scr,uint8_t x, uint8_t y)
{
	uint8_t command, adr;
	// проверим не выходит ли x и y за пределы максимальных значений
	// у дисплея две строчки по 40 символов в каждой согласно даташиту
	if (y>1) y=1;
	if (x>39) x = 39;
	// переведем координаты x и y в адрес памяти DDRAM согласно инструкции к дисплею
	if (y==0)
		adr = x;
	if (y==1)
		adr = x+0x40;
	// так как используем четырехбитный интрефейс, сформируем две команды для перемещения в нужную позицию DDRAM
	command = adr&0xf0;
	command = command|0x80;
	command = command|0x08;
	//command = ((adr&0xf0)|0x80)&0xF8; //adr&0xf0 выделяем старшую тетраду, |0x80 устанавливаем старший бит в 1(DB7=1 согласно инструкции)&0xF8 добавляем 8(1000 - Led,E,Rw,Rs) в млаюшую тетраду
	sendData(scr,&command);

	command = (adr<<4)|0x08;
	sendData(scr,&command);

}
//moveDisplayRigth перемещает дисплей на одну позицию враво
void moveDisplayRight(LCD1602 *scr)
{
	uint8_t command;
	command = 0x18;
	sendData(scr,&command);

	command = 0xC8;
	sendData(scr,&command);
}
// moveDispayLeft перемещает дисплей на одну позицию влево
void moveDisplayLeft(LCD1602 *scr)
{
	uint8_t command;
		command = 0x18;
		sendData(scr,&command);

		command = 0x88;
		sendData(scr,&command);
}

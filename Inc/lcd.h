/* File led.hpp */
 #include "stm32f1xx_hal.h"
 #ifndef LED_H
 #define LED_H

  typedef struct
  {
    I2C_HandleTypeDef *hi2c;
    uint16_t DevAddress;
  }LCD1602;


  void sendData(LCD1602 *scr , uint8_t *pData); //отправка команды по четырехбитному интерфейсу
  void initLCD(LCD1602 *scr); //инициализация дисплея
  void writeLCD(LCD1602 *scr, uint8_t s);// вывод символа s на дисплей
  void writeStringLCD(LCD1602 *scr,char *str);// вывод строки str на дисплей
  void moveXY(LCD1602 *scr,uint8_t x, uint8_t y); // переместить курсор в позицию X, Y
  void moveDisplayRight(LCD1602 *scr); //сдвинуть экран вправо на одну позицию
  void moveDisplayLeft(LCD1602 *scr); //сдвинуть экран влево на одну позицию
 #endif /* LED_H */

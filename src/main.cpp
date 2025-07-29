/***
 * 基于esp32c3+arduino的简易电报机，能将指定文本信息转换为摩斯电码，通过LED灯进行输出。
 * 工作原理：通过串口输入要发送的信息，系统将文本信息转换为摩斯电码，然后通过LED灯的闪烁进行输出。
 * 注意：仅支持英文字母、数字、英文逗号和英文句号，不支持汉字等特殊字符。
 *      单次可以发送的最长文本长度为 102400 字节（100KB）。
 * 使用方式：通过串口向开发板发送文本信息，然后可以在GPIO8引脚查看LED灯的闪烁。
 *         运行过程中按下boot键触发中断，系统停止翻译和输出。
 *
 * Author: Peter Adams
 * Date: 2025-07-29
 * Description: esp32-c3 学习项目
 * Board: ESP32-C3 Super Mini/ESP32-C3-DevKitM-1
 * Platform/Framework: PlatformIO 6.11/Arduino ESP32 Core 2.x
 * Editor: VSCode
 */

#include "morse-code.h"

ESP32MorseCode *morse_code = nullptr;

void setup()
{
  morse_code = new ESP32MorseCode(MAX_BUF_LENGTH);
  delay(1500);
  Serial.println("System ready.");
}

void loop()
{
  if (Serial.available() > 0)
  {
    while (Serial.available() > 0)
    {
      morse_code->getInput(Serial.read());
    }

    bool res = morse_code->isInputValid();
    if (res)
    {
      morse_code->translateInput();
    }
    else
    {
      Serial.println("Input invalid.\n");
      morse_code->clearBuffer();
      morse_code->errorBlink();
    }
  }
  else
  {
    delay(16);
  }
}
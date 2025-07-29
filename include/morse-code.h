#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include "Arduino.h"

#define LED_PIN 8
#define BOOT_BTN_PIN 9
#define MORSE_BASE_TIME 150                 // 基本时长，单位ms
#define MORSE_DOT_TIME MORSE_BASE_TIME      // 点的时长（亮）
#define MORSE_DASH_TIME 3 * MORSE_BASE_TIME // 划的时长（亮）
#define MORSE_DOT_DASH_TIME MORSE_BASE_TIME // 点划之间的时长（灭）
#define MORSE_CHAR_TIME 3 * MORSE_BASE_TIME // 字符之间的时长（灭）
#define MORSE_WORD_TIME 7 * MORSE_BASE_TIME // 单词之间的时长（灭）
#define ERROR_BLINK_TIME 80
#define DOT '.'
#define DASH '_'

#define MAX_BUF_LENGTH 102400

// 支持的字符
const char surported_char[] = {
    'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z', '1',
    '2', '3', '4', '5', '6', '7', '8',
    '9', '0', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S',
    'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

// 映射表
const char morse_code_table[][6] = {
    {2, DOT, DASH},             // A
    {4, DASH, DOT, DOT, DOT},   // B
    {4, DASH, DOT, DASH, DOT},  // C
    {3, DASH, DOT, DOT},        // D
    {1, DOT},                   // E
    {4, DOT, DOT, DASH, DOT},   // F
    {3, DASH, DASH, DOT},       // G
    {4, DOT, DOT, DOT, DOT},    // H
    {2, DOT, DOT},              // I
    {4, DOT, DASH, DASH, DASH}, // J
    {3, DASH, DOT, DASH},       // K
    {4, DOT, DASH, DOT, DOT},   // L
    {2, DASH, DASH},            // M
    {2, DASH, DOT},             // N
    {3, DASH, DASH, DASH},      // O
    {4, DOT, DASH, DASH, DOT},  // P
    {4, DASH, DASH, DOT, DASH}, // Q
    {3, DOT, DASH, DOT},        // R
    {3, DOT, DOT, DOT},         // S
    {1, DASH},                  // T
    {3, DOT, DOT, DASH},        // U
    {4, DOT, DOT, DOT, DASH},   // V
    {3, DOT, DASH, DASH},       // W
    {4, DASH, DOT, DOT, DASH},  // X
    {4, DASH, DOT, DASH, DASH}, // Y
    {4, DASH, DASH, DOT, DOT},  // Z

    {5, DASH, DASH, DASH, DASH, DASH}, // 0
    {5, DOT, DASH, DASH, DASH, DASH},  // 1
    {5, DOT, DOT, DASH, DASH, DASH},   // 2
    {5, DOT, DOT, DOT, DASH, DASH},    // 3
    {5, DOT, DOT, DOT, DOT, DASH},     // 4
    {5, DOT, DOT, DOT, DOT, DOT},      // 5
    {5, DASH, DOT, DOT, DOT, DOT},     // 6
    {5, DASH, DASH, DOT, DOT, DOT},    // 7
    {5, DASH, DASH, DASH, DOT, DOT},   // 8
    {5, DASH, DASH, DASH, DASH, DOT},  // 9
};

class ESP32MorseCode
{
private:
  char *inputBuffer;
  uint32_t inputBufferLength;
  bool isCheckedInput;
  bool isInterrupt;
  static ESP32MorseCode *instance; // 添加静态实例指针

  // 将输入缓冲中的所有小写字母转为大写
  void toUpperCase();
  // 根据某字符的摩斯电码数组控制led闪烁
  void blink(const char *code);
  // 打印缓冲区字符
  void printBuffer();
  // 是否发生中断
  bool interrupt(bool stateRestore = true);
  // 中断处理函数
  static void IRAM_ATTR interruptHandler();

public:
  ESP32MorseCode(uint32_t inputBufLength);
  ~ESP32MorseCode();
  ESP32MorseCode();
  // 判断inputBuffer指向的缓冲区内的字符是否合法
  bool isInputValid();
  // 获取一个字符输入，将字符存进缓冲区，忽略回车、换行符或结束符
  void getInput(const char c);
  // 直接翻译输入缓冲区中的字符，并控制led输出
  void translateInput();
  // 清空缓冲区
  void clearBuffer();
  // 错误闪烁，在遇到错误时快速闪烁5次
  void errorBlink();
};

#endif
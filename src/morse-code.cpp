#include "morse-code.h"

ESP32MorseCode *ESP32MorseCode::instance = nullptr;

ESP32MorseCode::ESP32MorseCode() {}
ESP32MorseCode::ESP32MorseCode(uint32_t inputBufLength)
{
  if (inputBufLength > MAX_BUF_LENGTH)
  {
    inputBufLength = MAX_BUF_LENGTH;
  }

  this->inputBuffer = new char[inputBufLength];
  this->inputBufferLength = 0;
  this->isCheckedInput = false;
  this->isInterrupt = false;
  ESP32MorseCode::instance = this;

  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BOOT_BTN_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, HIGH);
  // 注册中断函数
  attachInterrupt(digitalPinToInterrupt(BOOT_BTN_PIN), ESP32MorseCode::interruptHandler, FALLING);
}

ESP32MorseCode::~ESP32MorseCode()
{
  delete[] this->inputBuffer;
  ESP32MorseCode::instance = nullptr;
  detachInterrupt(digitalPinToInterrupt(BOOT_BTN_PIN));
}

void ESP32MorseCode::getInput(const char c)
{
  if (this->inputBufferLength >= MAX_BUF_LENGTH - 1)
    return;

  if (c != '\n' && c != '\r' && c != '\0')
  {
    this->inputBuffer[this->inputBufferLength++] = c;
  }
}

void ESP32MorseCode::printBuffer()
{
  for (int i = 0; i < this->inputBufferLength; i++)
    Serial.print(this->inputBuffer[i]);
  Serial.println();
}

bool ESP32MorseCode::isInputValid()
{
  Serial.println("Checking input...");
  Serial.print("Input buffer: ");
  this->printBuffer();
  uint32_t i = 0;

  for (i; i < this->inputBufferLength; i++)
  {
    if (this->interrupt(false))
      return true;
    const char c = this->inputBuffer[i];
    if (!isdigit(c) && !isalpha(c) && c != ' ' && c != '.' && c != ',')
    {
      Serial.printf("Invalid character: %c\n", c);
      return false;
    }
  }

  this->isCheckedInput = true;
  return true;
}

void ESP32MorseCode::toUpperCase()
{
  Serial.println("Converting to upper case...");
  for (uint32_t i = 0; i < this->inputBufferLength; i++)
  {
    if (this->interrupt(false))
      return;
    this->inputBuffer[i] = toupper(this->inputBuffer[i]);
  }
  Serial.println("Converted.");
}

void ESP32MorseCode::blink(const char *codeList)
{
  if (this->interrupt())
    return;
  const uint8_t len = (uint8_t)codeList[0];
  for (uint8_t i = 1; i < len + 1; i++)
  {
    if (this->interrupt(false))
      return;
    const char code = codeList[i];
    Serial.printf("%c ", code);
    // 元素间隔
    if (code == DASH)
    {
      digitalWrite(LED_PIN, LOW);
      delay(MORSE_DASH_TIME);
    }
    else if (code == DOT)
    {
      digitalWrite(LED_PIN, LOW);
      delay(MORSE_DOT_TIME);
    }
    digitalWrite(LED_PIN, HIGH);
    delay(MORSE_DOT_TIME);
  }
  Serial.println();
}

void ESP32MorseCode::translateInput()
{
  if (this->interrupt())
    return;
  if (!this->isCheckedInput)
  {
    Serial.println("Error, has not checked input yet.");
    this->clearBuffer();
    this->errorBlink();
    return;
  }
  // 将缓冲区的字符转换为大写
  this->toUpperCase();

  Serial.println("Translating...");
  for (uint32_t i = 0; i < this->inputBufferLength; i++)
  {
    if (this->interrupt())
      return;
    // 获取一个字符和与之对应的摩斯码
    const char c = this->inputBuffer[i];

    // 空格为单词间隔
    if (c == ' ')
    {
      delay(MORSE_WORD_TIME);
      continue;
    }

    if (c == ',' || c == '.')
      continue;

    const char *morse_code_arr = nullptr;
    if (isalpha(c))
    {
      morse_code_arr = morse_code_table[c - 65];
    }
    else if (isdigit(c))
    {
      morse_code_arr = morse_code_table[c - 48 + 26];
    }

    // 通过led输出该字符
    this->blink(morse_code_arr);
    if (this->interrupt())
      return;
    // 等待一个字符间隔
    if (i < this->inputBufferLength - 1)
      delay(MORSE_CHAR_TIME);
  }

  // 全部打印完后清空缓冲区并关闭led
  this->clearBuffer();
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Done.\n");
}

void ESP32MorseCode::errorBlink()
{
  for (uint8_t i = 0; i <= 5; i++)
  {
    digitalWrite(LED_PIN, LOW);
    delay(ERROR_BLINK_TIME);
    digitalWrite(LED_PIN, HIGH);
    delay(ERROR_BLINK_TIME);
  }
}

void ESP32MorseCode::clearBuffer()
{
  memset(this->inputBuffer, 0, sizeof(char) * this->inputBufferLength);
  this->inputBufferLength = 0;
}

bool ESP32MorseCode::interrupt(bool stateRestore)
{
  if (this->isInterrupt)
  {
    if (stateRestore)
    {
      Serial.println("\nTask interrupted.\n");
      this->isInterrupt = false;
      this->clearBuffer();
    }
    return true;
  }
  return false;
}

void IRAM_ATTR ESP32MorseCode::interruptHandler()
{
  // 使用静态实例指针访问非静态成员
  if (ESP32MorseCode::instance != nullptr)
  {
    ESP32MorseCode::instance->isInterrupt = true;
  }
}
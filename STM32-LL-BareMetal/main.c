/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const uint8_t seg_map[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

volatile uint8_t rx_byte = 0;
volatile uint8_t data_ready = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Print(char *str);
void LCD_Clear(void);
void Display_On_Segment(char ch);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  
  // ????? ?????? ???? SPI1
  NVIC_SetPriority(SPI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(SPI1_IRQn);

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();

  // ?????????? ????? ???????? ????????
  LL_mDelay(50);
  LCD_Init();
  LCD_Print("SPI Received:");

/* ????? ????? ??? ?? ???? ????? ????? ???? ????? ??? ??????? ?? ??? ????? ?? */
  LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_HARD_INPUT);

  /* ????????? ???? ?????? SPI1 */
  LL_SPI_EnableIT_RXNE(SPI1);
  LL_SPI_Enable(SPI1);

  /* Infinite loop */
while (1)
{
    // ?. ??? ????? ????? ?? SPI ???
    if (data_ready == 1)
    {
        data_ready = 0;
        
        // ????? ???? ?? ??? ???????? ?????
        LCD_SendCommand(0xC0);
        LCD_SendData(rx_byte);
        
        // ???? ????? ???? ?????????
        // ???? ????? ????? ????? ???? ??????? ????? ???????? (PB4-PB7) ??
        // ?? ??? ???? ??????? ????? ? ??? PB8-PB14 ?? ?? ????
        uint8_t seg_val = seg_map[rx_byte - '0'];
        
        // ??? ???? ??? ??????? ????? ?? ?????????
        GPIOB->ODR &= ~0x7F00; 
        // ?? ???? ??? ??????? ?????????
        GPIOB->ODR |= (seg_val << 8);
    }
	}
}

// ????? ?????????? ? ????? LCD ???????? (?? 4 ???)
void LCD_SendCommand(uint8_t cmd) {
    // ????? 4 ??? ????
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0); // RS = 0 (?????)
    GPIOB->ODR = (GPIOB->ODR & 0xFF0F) | ((cmd & 0xF0) << 0);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);  // E = 1
    for(volatile int i=0; i<500; i++);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1); // E = 0
    for(volatile int i=0; i<500; i++);

    // ????? 4 ??? ?????
    GPIOB->ODR = (GPIOB->ODR & 0xFF0F) | ((cmd & 0x0F) << 4);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);  // E = 1
    for(volatile int i=0; i<500; i++);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1); // E = 0
    LL_mDelay(2);
}

void LCD_SendData(uint8_t data) {
    // ????? 4 ??? ????
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0); // RS = 1 (????)
    GPIOB->ODR = (GPIOB->ODR & 0xFF0F) | ((data & 0xF0) << 0);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);  // E = 1
    for(volatile int i=0; i<500; i++);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1); // E = 0
    for(volatile int i=0; i<500; i++);

    // ????? 4 ??? ?????
    GPIOB->ODR = (GPIOB->ODR & 0xFF0F) | ((data & 0x0F) << 4);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);  // E = 1
    for(volatile int i=0; i<500; i++);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1); // E = 0
    for(volatile int i=0; i<500; i++);
}

void LCD_Init(void) {
    LL_mDelay(15);
    LCD_SendCommand(0x03);
    LL_mDelay(5);
    LCD_SendCommand(0x03);
    LL_mDelay(1);
    LCD_SendCommand(0x03);
    LCD_SendCommand(0x02); // ????? ?? ?? 4 ???
    LCD_SendCommand(0x28); // 2 ?? ? ???? 5x8
    LCD_SendCommand(0x0C); // ???? ???? ???? ? ????? ???? ????????
    LCD_SendCommand(0x06); // ???? ?????? ???????? ?? ???
    LCD_SendCommand(0x01); // ??? ???? ????
    LL_mDelay(2);
}

void LCD_Print(char *str) {
    while(*str) {
        LCD_SendData(*str++);
    }
}

// ???? ???? ???? ?????? ????????? ?? ???? ??????? ?????
void Display_On_Segment(char ch) {
    // ??? ??????: ?? ?????? ?? ????????? ???? ???????? ?? ???? ?? (??? 8)
    // ?? 0x7F ???? ???? ????? ???? ???? ???? ??? ????????
    uint8_t test_code = 0x7F; 
    
    // ??? ???? ??????? ? ?? ?? ???? B
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|
                                  LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14);
    
    // ????? ?? ??? ???? ???? ??? ???? ?????????
    LL_GPIO_SetOutputPin(GPIOB, (uint32_t)(test_code << 8));
}


/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0);

  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

  while(LL_RCC_HSI_IsReady() != 1);

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI);

  LL_Init1msTick(8000000);
  LL_SetSystemCoreClock(8000000);
}

/**
  * @brief SPI1 Initialization Function
  */
static void MX_SPI1_Init(void)
{
  LL_SPI_InitTypeDef SPI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* ?. ????????? ???? ?????????? ????? SPI1 ? ???? A */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

  /**SPI1 GPIO Configuration
  PA5      ------> SPI1_SCK
  PA6      ------> SPI1_MISO
  PA7      ------> SPI1_MOSI
  */
  /* ?. ???????? ???????? ????? MISO ? MOSI ?? ???? Alternate Function */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* ?. ????? ?????????? ????? SPI1 ?? ???? Slave */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  
  /* ????? ?????: ????? ?????? ??? NSS ?? ????????? ??? ??? ???? ????? ??? ??? */
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT; 
  
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

  // ???? ????? ???? ??????? ?????
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0|LL_GPIO_PIN_1);
  LL_GPIO_ResetOutputPin(GPIOB, 0xFFF0); // ??????? ? ?? ?? ???? B

  // ????? ??????? RS ? E ???????? ??? ???? A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // ????? ?? ??????? ????? ????? ???????? ? ????????? ??? ???? B (??????? PB4 ?? PB14)
	GPIO_InitStruct.Pin = LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7|
												LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11|
												LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1);
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

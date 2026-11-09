#include "stm32f10x.h"

const uint8_t seg_code[] = {
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F  /* 9 */
};

uint8_t active_seg = 0;     
uint8_t current_digit = 0;  
uint8_t is_locked = 0;

void Clock_Init(void);
void GPIO_Init(void);
void USART1_Init(void);
void USART1_SendChar(uint8_t ch);
void USART1_SendString(char* str);
uint8_t USART1_GetChar(void);
void Display_Digit_On_Segment(uint8_t segment, uint8_t number);
void Display_Raw_Code_On_Segment(uint8_t segment, uint8_t raw_code);
void Clear_All_Displays(void);

int main(void) {
    Clock_Init();
    GPIO_Init();
    USART1_Init();
    
    Clear_All_Displays();
    Display_Digit_On_Segment(0, 0); 

    char rx_char;
    uint8_t digit_0 = 0; 
    uint8_t digit_1 = 0; 
    uint8_t digit_2 = 0; 

    USART1_SendString("\r\nSTM32 Pure Register Ready\r\n");

    while(1) {
        rx_char = USART1_GetChar();
        
        if (rx_char != 0) {
            
            if (rx_char == '*') {
                active_seg = 0;
                digit_0 = 0; digit_1 = 0; digit_2 = 0;
                is_locked = 0;
                Clear_All_Displays();
                Display_Digit_On_Segment(0, 0); 
                USART1_SendString("* -> Cleared\r\n");
                continue; 
            }

            if (rx_char >= '0' && rx_char <= '9' && is_locked == 0) {
                current_digit = rx_char - '0'; 
                
                uint8_t temp_d0 = digit_0;
                uint8_t temp_d1 = digit_1;
                uint8_t temp_d2 = digit_2;

                if (active_seg == 0)      temp_d0 = current_digit;
                else if (active_seg == 1) temp_d1 = current_digit;
                else if (active_seg == 2) temp_d2 = current_digit;
                
                uint16_t total_value = temp_d0 + (temp_d1 * 10) + (temp_d2 * 100);
                
                if (total_value > 255) {
                    USART1_SendString("Error: Exceeds 255\r\n");
                } else {
                    digit_0 = temp_d0;
                    digit_1 = temp_d1;
                    digit_2 = temp_d2;
                    Display_Digit_On_Segment(active_seg, current_digit);
                    USART1_SendChar(rx_char);
                    USART1_SendString(" -> Registered\r\n");
                }
            }
            else if (rx_char == '#') {
                if (active_seg < 2 && is_locked == 0) {
                    active_seg++; 
                    if (active_seg == 1)  digit_1 = 0;
                    if (active_seg == 2)  digit_2 = 0;
                    
                    Display_Digit_On_Segment(active_seg, 0); 
                    USART1_SendString("# -> Next Segment\r\n");
                } 
                else if (active_seg == 2 && is_locked == 0) {
                    is_locked = 1;
                    USART1_SendString("# -> Locked\r\n");
                }
            }
        }
    }
}

void Clock_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;   
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

void GPIO_Init(void) {
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 

    GPIOB->CRL = 0x33333333; 
    GPIOB->CRH = 0x33333333;
    GPIOA->CRL = 0x33333333;

    GPIOA->CRH &= ~0x00000FFF; 
    GPIOA->CRH |=  0x000004B0; 
}

void USART1_Init(void) {
    USART1->BRR = 0x0341;         
    USART1->CR1 |= USART_CR1_TE;   
    USART1->CR1 |= USART_CR1_RE;   
    USART1->CR1 |= USART_CR1_UE;   
}

void USART1_SendChar(uint8_t ch) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = (ch & 0xFF); 
}

void USART1_SendString(char* str) {
    while(*str) {
        USART1_SendChar(*str++);
    }
}

uint8_t USART1_GetChar(void) {
    if (USART1->SR & USART_SR_RXNE) {
        return (uint8_t)(USART1->DR & 0xFF); 
    }
    return 0; 
}

void Display_Digit_On_Segment(uint8_t segment, uint8_t number) {
    uint32_t code = seg_code[number];
    Display_Raw_Code_On_Segment(segment, code);
}

void Display_Raw_Code_On_Segment(uint8_t segment, uint8_t raw_code) {
    if (segment == 0) { 
        GPIOB->ODR = (GPIOB->ODR & ~0x007F) | raw_code;
    } 
    else if (segment == 1) { 
        GPIOB->ODR = (GPIOB->ODR & ~0x3F80) | (raw_code << 7);
    } 
    else if (segment == 2) { 
        GPIOA->ODR = (GPIOA->ODR & ~0x007F) | raw_code;
    }
}

void Clear_All_Displays(void) {
    GPIOB->ODR &= ~0x3FFF; 
    GPIOA->ODR &= ~0x007F; 
}
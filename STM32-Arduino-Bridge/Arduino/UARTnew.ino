#include <avr/io.h>

int main(void) {
    char keys[12] = {
        '1', '2', '3',
        '4', '5', '6',
        '7', '8', '9',
        '*', '0', '#'
    };

    DDRD |= (1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2);
    
    DDRB &= ~(1 << PB0);
    DDRD &= ~((1 << PD7) | (1 << PD6));

    PORTD |= (1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2);
    
    PORTB |= (1 << PB0);
    PORTD |= (1 << PD7) | (1 << PD6);

    UBRR0H = 0x00;
    UBRR0L = 103;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    while (1) {
        for (int r = 0; r < 4; r++) {
            PORTD |= (1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2);
            
            if (r == 0)      PORTD &= ~(1 << PD5);
            else if (r == 1) PORTD &= ~(1 << PD4);
            else if (r == 2) PORTD &= ~(1 << PD3);
            else if (r == 3) PORTD &= ~(1 << PD2);
            
            for (volatile uint16_t d = 0; d < 50; d++);

            for (int c = 0; c < 3; c++) {
                uint8_t is_pressed = 1;

                if (c == 0)      is_pressed = PINB & (1 << PB0);
                else if (c == 1) is_pressed = PIND & (1 << PD7);
                else if (c == 2) is_pressed = PIND & (1 << PD6);

                if (!is_pressed) {
                    for (volatile uint32_t d = 0; d < 40000; d++);

                    if (c == 0)      while (!(PINB & (1 << PB0)));
                    else if (c == 1) while (!(PIND & (1 << PD7)));
                    else if (c == 2) while (!(PIND & (1 << PD6)));

                    char key_to_send = keys[(r * 3) + c];

                    while (!(UCSR0A & (1 << UDRE0)));
                    UDR0 = key_to_send;
                }
            }
        }
        for (volatile uint32_t d = 0; d < 20000; d++);
    }
    return 0;
}
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define MSK(BIT) (uint8_t)(1u << BIT)
#define COL_MSK(R1,R2,R3,R4,R5) ((R1 ? 0 : MSK(0)) | \
								 (R2 ? 0 : MSK(1)) | \
								 (R3 ? 0 : MSK(2)) | \
								 (R4 ? 0 : MSK(3)) | \
								 (R5 ? 0 : MSK(4)))

const uint8_t columns[5] = { COL_MSK(1,0,0,0,0), COL_MSK(1,0,0,0,0), COL_MSK(1,1,1,1,1), COL_MSK(1,0,0,0,0), COL_MSK(1,0,0,0,0) };

volatile uint8_t G_u8SysTick;

static inline void SystemInit();
static inline void SystemSleep();

__attribute__ ((OS_main)) int main(void) {
	SystemInit();

	uint16_t column = 0;
	uint8_t portB_temp = MSK(0), column_temp = columns[0];
	while (1) {
		PORTB = 0;
		PORTD = column_temp;
		PORTB = portB_temp;

		column++;
		if (column == 5) {
			column = 0;
			portB_temp = MSK(0);
		}
		else {
			portB_temp = PORTB << 1;
		}
		column_temp = columns[column];

		SystemSleep();
	}
}

static inline void SystemInit() {
	PORTD = MSK(4) | MSK(3) | MSK(2) | MSK(1) | MSK(0);
	DDRA = MSK(1) | MSK(0);
	DDRB = MSK(7) | MSK(6) | MSK(5) | MSK(4) | MSK(3) | MSK(2) | MSK(1) | MSK(0);
	DDRD = MSK(6) | MSK(5) | MSK(4) | MSK(3) | MSK(2) | MSK(1) | MSK(0);

	TCCR0A = MSK(WGM01);
	OCR0A = 124;
	TIMSK = MSK(OCIE0A);

	MCUCR = MSK(SE);
	sei();
	TCCR0B = MSK(CS01);
}

static inline void SystemSleep() {
	static uint8_t u8ExpectedSysTick;

	while (u8ExpectedSysTick == G_u8SysTick)
		sleep_cpu();
	u8ExpectedSysTick++;
}

ISR(TIMER0_COMPA_vect) {
	G_u8SysTick++;
}

/*
P
1
2
3
4
5
  1 2 3 4 5 D

Darlington:
1 -> PB4
2 -> PB3
3 -> PB2
4 -> PB1
5 -> PB0

PNP:
1 -> PD0
2 -> PD1
3 -> PD2
4 -> PD3
5 -> PD4
*/

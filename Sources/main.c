/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"
#include "display.h"
#include <stdint.h>


/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))
#define PTA_COLS_MASK	(GPIO_PDOR_PDO(GPIO_PIN(8)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(10)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(6)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(11)))

#define PTA_ROWS_MASK	(GPIO_PDOR_PDO(GPIO_PIN(26)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(24)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(9)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(25)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(28)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(7)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(27)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(29)))

/* Constants specifying delay loop duration */
#define	tdelay1			10000
#define tdelay2 		20

static display_t display;

/* Configuration of the necessary MCU peripherals */
void SystemConfig() {
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;

	// Enable clock for PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	/* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3

	/* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	/* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
	PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN


	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );
}


/* Variable delay loop */
void delay(int t1, int t2)
{
	int i, j;

	for(i=0; i<t1; i++) {
		for(j=0; j<t2; j++);
	}
}

void select_column(unsigned int col_num)
{
	uint32_t selection = 0xFFFFFFFF;
	unsigned const GPIO_PINS_COUNT = 4;
	unsigned const GPIO_PINS[] = { 8, 10, 6, 11 };
	unsigned int bit = 1;

	for (unsigned bit_index = 0; bit_index < GPIO_PINS_COUNT; bit_index++) {
		if ((col_num & bit) == bit) {
			selection &= ~GPIO_PDOR_PDO(GPIO_PIN(GPIO_PINS[bit_index]));
		}

		bit *= 2;
	}

	PTA->PDOR |= PTA_COLS_MASK; // zero out all four gpio pins
	PTA->PDOR &= selection;
}

void select_rows(unsigned int rows)
{
	uint32_t selection = 0xFFFFFFFF;
	unsigned const GPIO_PINS_COUNT = 8;
	unsigned const GPIO_PINS[] = {26, 24, 9, 25, 28, 7, 27, 29};
	unsigned int bit = 1;

	for (unsigned bit_index = 0; bit_index < GPIO_PINS_COUNT; bit_index++) {
		if ((rows & bit) == bit) {
			selection &= ~GPIO_PDOR_PDO(GPIO_PIN(GPIO_PINS[bit_index]));
		}

		bit *= 2;
	}

	PTA->PDOR |= PTA_ROWS_MASK; // zero out all eight gpio pins
	PTA->PDOR &= selection;
}

void PIT0_IRQHandler()
{
	display_shift_text_left(&display);

	PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
}

void PIT0Init()
{

	PIT_MCR = 0x00; // turn on PIT
	PIT_LDVAL0 = 0x98967F; // 9 999 999 cycles, 200 ms
	PIT_TFLG0 |= PIT_TFLG_TIF_MASK; // clear interrupt
	PIT_TCTRL0 = PIT_TCTRL_TIE_MASK; // enable Timer 1 interrupts
	PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK; // start Timer 1

    NVIC_EnableIRQ(PIT0_IRQn); // enable interrupts from LPTMR0
}

int main(void)
{
	SystemConfig();
	PIT0Init();

	//PTA->PDOR |= GPIO_PDOR_PDO(0x3F000280); // turning the pixels of a particular row ON
	//PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));

	delay(tdelay1, tdelay2);

	char message[10] = "AaA";

	display_init(&display);
	display_set_text(&display, message);
	display.matrix[4] = 0x0F;

	unsigned ROWS_COUNT = 16;
	for (unsigned i = 0; i < ROWS_COUNT; i++) {
		select_column(i);
		select_rows(display.matrix[i]);

		PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );
		delay(100, 1);
		PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));

		if (i == ROWS_COUNT - 1) {
			i = -1;
		}
	}

	display_free(&display);
    /* Never leave main */
    return 0;
}

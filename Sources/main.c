/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"
#include "display.h"
#include "messages.h"
#include <stdint.h>
#include <stdbool.h>


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

#define BTN_SW2 0x400     // Port E, bit 10
#define BTN_SW4 0x8000000 // Port E, bit 27

/* Constants specifying delay loop duration */
#define	tdelay1			10000
#define tdelay2 		20

static display_t display;
static unsigned column_index;

/* Configuration of the necessary MCU peripherals */
void SystemConfig() {

	// Selects the clock source for the UART0 transmit and receive clock.
	//SIM->SOPT2 |= SIM_SOPT2_UART0SRC(0x01);
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTB_MASK;
	// Enable clock for PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Enable clock for UART
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;


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

    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // Button SW2
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // Button SW4

	PORTB->PCR[1] = ( 0 | PORT_PCR_MUX(0x02) );	// UART0_TX
	PORTB->PCR[2] = ( 0 | PORT_PCR_MUX(0x02) ); // UART0_RX


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

void select_gpio_pins(unsigned const *gpio_pins, const unsigned pins_count,
		uint32_t selection_mask, const unsigned num)
{
	uint32_t selection = 0xFFFFFFFF;
	unsigned int bit = 1;

	for (unsigned bit_index = 0; bit_index < pins_count; bit_index++) {
		if ((num & bit) == bit) {
			selection &= ~GPIO_PDOR_PDO(GPIO_PIN(gpio_pins[bit_index]));
		}

		bit *= 2;
	}

	PTA->PDOR |= selection_mask; // zero out all gpio pins
	PTA->PDOR &= selection; // select pins
}

void select_column(unsigned int col_num)
{
	unsigned const GPIO_PINS_COUNT = 4;
	unsigned const GPIO_PINS[] = { 8, 10, 6, 11 };
	select_gpio_pins(GPIO_PINS, GPIO_PINS_COUNT, PTA_COLS_MASK, col_num);
}

void select_rows(unsigned int rows)
{
	unsigned const GPIO_PINS_COUNT = 8;
	unsigned const GPIO_PINS[] = {26, 24, 9, 25, 28, 7, 27, 29};
	select_gpio_pins(GPIO_PINS, GPIO_PINS_COUNT, PTA_ROWS_MASK, rows);
}

/**
 * Handles PIT timer's IRQ.
 * It shifts all text by one column to the left.
 */
void PIT0_IRQHandler()
{
	display_shift_text_left(&display);

	PIT_TFLG0 |= PIT_TFLG_TIF_MASK; // clear interrupt
}

/**
 * Handles PIT timer's IRQ.
 * It draws a single column of the display matrix.
 */
void PIT1_IRQHandler()
{

	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );

	PTA->PDOR &= ~PTA_ROWS_MASK; // disable all row pins

	select_column(column_index);
	select_rows(display.matrix[column_index]);

	PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));

	column_index++;
	if (column_index == 16)
		column_index = 0;

	PIT_TFLG1 |= PIT_TFLG_TIF_MASK; // clear interrupt
}

void PITInit()
{
	PIT_MCR = 0x00; // turn on PIT

	PIT_LDVAL0 = 0x4C4B3F; // 4 999 999 cycles, 100 ms
	PIT_TFLG0 |= PIT_TFLG_TIF_MASK; // clear interrupt
	PIT_TCTRL0 = PIT_TCTRL_TIE_MASK; // enable Timer 1 interrupts
	PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK; // start Timer 1

    NVIC_EnableIRQ(PIT0_IRQn); // enable interrupts from PIT0

	PIT_LDVAL1 = 0x3E7F; // 15 999 cycles, 0.32  ms
	PIT_TFLG1 |= PIT_TFLG_TIF_MASK; // clear interrupt
	PIT_TCTRL1 = PIT_TCTRL_TIE_MASK; // enable Timer 1 interrupts
	PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK; // start Timer 1

    NVIC_EnableIRQ(PIT1_IRQn); // enable interrupts from PIT1
}
/**
void UART0Init()
{
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK); // Disable transmitter and receiver

	UART0->BDH = 0x00;
	UART0->BDL = 0x1A;	// Baud rate 115 200 Bd, 1 stop bit
	UART0->C4 = 0x0F;	// Oversampling ratio 16, match address mode disabled

	UART0->C1 = 0x00;	// 8 bits, and without a parity bit
	UART0->C3 = 0x00;
	UART0->MA1 = 0x00;	// no match address (mode disabled in C4)
	UART0->MA2 = 0x00;	// no match address (mode disabled in C4)
	UART0->S1 |= 0x1F;
	UART0->S2 |= 0xC0;

	UART0->C2 |= ( UART0_C2_TE_MASK | UART0_C2_RE_MASK );	// Enable transmitter and receiver
}

void SendChar(char ch)
{
    while (!(UART0->S1 & UART0_S1_TDRE_MASK) && !(UART0->S1 & UART0_S1_TC_MASK));
    UART0->D = ch;
}

char ReceiveChar(void)
{
	while (!(UART0->S1 & UART0_S1_RDRF_MASK));
	return UART0->D;
}

void SendString(char *s)
{
	for (int i = 0; s[i] != 0; i++) {
		SendChar(s[i]);
	}
}
*/
int main(void)
{
	SystemConfig();
	PITInit();
	//UART0Init();

	delay(tdelay1, tdelay2);

	char *messages[] = { "xondri07", "42", "69" };
	messenger_t messenger;
	messenger_init(&messenger, messages, 3);

	display_init(&display);
	display_set_text(&display, messenger_get_next(&messenger));

	bool btn_sw2_pressed = false;
	bool btn_sw4_pressed = false;

	while (1) {
		if (!btn_sw2_pressed && !(GPIOE_PDIR & BTN_SW2))
		{
			btn_sw2_pressed = true;
			char *msg = messenger_get_next(&messenger);
			display_set_text(&display, msg);
		}
		else if (GPIOE_PDIR & BTN_SW2) {
			btn_sw2_pressed = false;
		}

		if (!btn_sw4_pressed && !(GPIOE_PDIR & BTN_SW4))
		{
			btn_sw4_pressed = true;
			char *msg = messenger_get_previous(&messenger);
			display_set_text(&display, msg);
		}
		else if (GPIOE_PDIR & BTN_SW4) {
			btn_sw4_pressed = false;
		}
	}

	display_free(&display);

    return 0;
}

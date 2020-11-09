/*
 * main.c
 *
 *  Created on: 24 Oct 2020
 *      Author: Ladislav Ondris
 *      Email: xondri07@vutbr.cz
 *
 *  A simple application that displays moving text on a matrix display.
 *  It is controlled by Kinetis K60.
 */

/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"
#include "display.h"
#include "messages.h"
#include "string.h"
#include <stdint.h>
#include <stdbool.h>


/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

/** GPIO pins for controlling columns of the display.
 *  There are 16 columns, but only 4 pins.
 *  Pins represent binary number 0-15.
 */
#define PTA_COLS_MASK	(GPIO_PDOR_PDO(GPIO_PIN(8)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(10)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(6)) | \
						 GPIO_PDOR_PDO(GPIO_PIN(11)))

/** GPIO pins for controlling rows of the display.
 * There are 8 rows. Each pin activates a single row.
 * */
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
#define BTN_SW2_PCR_INDEX 10
#define BTN_SW4_PCR_INDEX 27

/* Contains display information such as the text that is being displayed. */
static display_t display;
/* Holds texts that can be displayed. */
static messenger_t messenger;
/* A variable keeping text inputed via UART to be displayed. */
static string_t string_buffer;
/* The column that is currently being displayed. It is controlled by PIT1's IRQ. */
static unsigned column_index;

/* Configuration of the necessary MCU peripherals */
void SystemConfig()
{
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK; // turn off watchdog

	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;
	// Enable clock for PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Enable clock for UART
    SIM->SCGC1 = SIM_SCGC1_UART5_MASK;

	// Port A
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	// Port E
	PORTE->PCR[8]  = PORT_PCR_MUX(0x03); // UART5_TX
    PORTE->PCR[9]  = PORT_PCR_MUX(0x03); // UART5_RX
	PORTE->PCR[28] = (0 | PORT_PCR_MUX(0x01)); // # EN

    int btns[2] = { BTN_SW2_PCR_INDEX, BTN_SW4_PCR_INDEX };
    for (int i = 0; i < 2; i++) {
		PORTE->PCR[btns[i]] = ( PORT_PCR_ISF(0x01) /* Zero-out ISF (Interrupt Status Flag) */
						| PORT_PCR_IRQC(0x0A) /* Interrupt enable on falling edge */
						| PORT_PCR_MUX(0x01) /* Pin Mux Control to GPIO */
						| PORT_PCR_PE(0x01) /* Pull resistor enable... */
						| PORT_PCR_PS(0x01)); /* ...select Pull-Up */
    }

	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);
	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD(GPIO_PIN(28));

	/* Enable IRQ from PORTE (that is used for buttons) */
	NVIC_ClearPendingIRQ(PORTE_IRQn);
	NVIC_EnableIRQ(PORTE_IRQn);
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

	NVIC_ClearPendingIRQ(PIT0_IRQn);
    NVIC_EnableIRQ(PIT0_IRQn); // enable interrupts from PIT0

	PIT_LDVAL1 = 0x3E7F; // 15 999 cycles, 0.32  ms
	PIT_TFLG1 |= PIT_TFLG_TIF_MASK; // clear interrupt
	PIT_TCTRL1 = PIT_TCTRL_TIE_MASK; // enable Timer 1 interrupts
	PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK; // start Timer 1

	NVIC_ClearPendingIRQ(PIT1_IRQn);
    NVIC_EnableIRQ(PIT1_IRQn); // enable interrupts from PIT1
}


unsigned char receive_char() {
    while(!(UART5->S1 & UART_S1_RDRF_MASK));
    return UART5->D;
}

void send_char(char c) {
    while(!(UART5->S1 & UART_S1_TDRE_MASK) && !(UART5->S1 & UART_S1_TC_MASK));
    UART5->D = c;
}

void send_string(char *s)
{
	for (int i = 0; s[i] != 0; i++) {
		send_char(s[i]);
	}
}

void send_line(char *s)
{
	send_string(s);
	send_string("\r\n");
}

bool received_char()
{
	return UART5->S1 & UART_S1_RDRF_MASK;
}


void UART5_RX_TX_IRQHandler()
{
	if (received_char()) {
		char c = UART5->D;
		send_char(c);

		/* Append char to existing buffer */
		string_add_char(&string_buffer, c);

		/* The received string is whole.
		 * It can be now displayed.
		 */
		if (c == '\r' || c == '\n') {
			send_line("");
			display_set_text(&display, string_buffer.string);

			string_free(&string_buffer);
			string_init(&string_buffer);

			send_string("Type text to display: ");
		}
	}
}

void UART5Init()
{
    UART5->C2  &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    UART5->BDH =  0x00;
    UART5->BDL =  0x1A; // Baud rate 115 200 Bd, 1 stop bit
    UART5->C4  =  0x0F; // Oversampling ratio 16, match address mode disabled
    UART5->C1  =  0x00; // 8 data bitu, bez parity
    UART5->C3  =  0x00;
    UART5->MA1 =  0x00; // no match address (mode disabled in C4)
    UART5->MA2 =  0x00; // no match address (mode disabled in C4)
    UART5->S2  |= 0xC0;
    UART5->C2  |= (UART_C2_TE_MASK | UART_C2_RE_MASK); // Zapnout vysilac i prijimac
	UART5->C2 |= UART_C2_RIE_MASK; // enable interrupts from receiver

	NVIC_ClearPendingIRQ(UART5_RX_TX_IRQn);
	NVIC_EnableIRQ(UART5_RX_TX_IRQn);
}

void PORTE_IRQHandler()
{
	delay(2000, 1); // Wait a few ms for the signal oscillation to stop

	// Interrupt from BTN_SW2 and log. 0 is on the input.
 	if ((PORTE_ISFR  & BTN_SW2) && !(GPIOE_PDIR & BTN_SW2)) {
		char *msg = messenger_get_next(&messenger);
		display_set_text(&display, msg);
	}
 	// Interrupt from BTN_SW2 and log. 0 is on the input.
	if ((PORTE_ISFR  & BTN_SW4) && !(GPIOE_PDIR & BTN_SW4)) {
		char *msg = messenger_get_previous(&messenger);
		display_set_text(&display, msg);
	}
	PORTE_ISFR = ~0; // Writes all ones to clear
}


int main(void)
{
	SystemConfig();
	PITInit();
	UART5Init();

	delay(10000, 20);

	send_line("\nApplication has started...");
	send_string("Type text to display: ");

	string_init(&string_buffer);

	char *messages[] = { "xondri07", "42", "69" };
	messenger_init(&messenger, messages, 3);

	display_init(&display);
	display_set_text(&display, messenger_get_next(&messenger));

	while (1);
	display_free(&display);
    return 0;
}

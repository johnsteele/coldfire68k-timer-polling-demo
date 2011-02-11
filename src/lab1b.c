/*****************************************************************************
* Author: John Steele
* Date: January 10, 2011
* Class: EE 427 
* lab1.c: Used for Motorola M5206eLITE 68K ColdFire eval board
*****************************************************************************/

/*
 * The MBAR (Module Base Address Register) defines where in memory the other 
 * peripheral registers appear in the memory space of the processor.
 */
#define MBAR (0x10000000) 

/*
 * TCN (Timer Counter Register) is a memory-mapped 16-bit up counter that you 
 * can read at any time. A read cycle to TCN yields the current timer value 
 * and does not affect the counting operation. 
 *
 * A write of any value to TCN causes it to reset to all zeros.
 * TCN register keeps the current count of the timer. Duh!
 */
#define TCN1 (MBAR+0x10C) 

/*****************************************************************************
 *			Timer Registers
 *
 * The ColdFire's timers are both composed of five memory-mapped registers. 
 * You deal with the timers by reading and writing to these registers.
 ******************************************************************************/
/*
 * TMR 1 (Timer Mode Register) is a 16-bit memory-mapped register. This register 
 * programs the various timer modes and is cleared by reset.
 * The TMR is used to initialize the timer and control the timer behavior. 
 * This is the most complicated register. It is also the one you need to be 
 * familiar with.
 */
#define TMR1 (MBAR+0x100) 

/* 
 * The TRR (Timer Reference Register) is a 16-bit register containing
 * the reference value that is compared with the free-running timer counter (TCN) 
 * as part of the output-compare function. TRR is a memory-mapped read/write register.
 *
 * The TRR takes a user-supplied value that is compared to the Timer Counter to determine 
 * when to stop and/or interrupt. The timer counts up to this number. You tell it how 
 * fast to count and what to do when it gets to the reference number.
 */
#define TRR1 (MBAR+0x104) // Timer 1 reference register

/* 
 * Timer configuration.
 * Used to set up the Timer Mode Register. 
 * Used to set and start the timer (1111111100011101).
 */
#define TMR_CFG (0xFF1D)

/* 
 * The 7-segment display is an LED digital display. dBug automatically initializes 
 * it. It is accessed through memory mapping since it's an external device on the 
 * eval board. 
 * 
 * The display is changed by writing a new value to the address 0x40000000.
 */
#define SEVEN_SEG_DISPLAY (0x40000000)

/*
 * The (Chip Select Control Register) is used for writing to the 7-segment display. 
 */
#define CSCR3 (MBAR+0x92)

/*
 * The (Chip Select Mask Register).
 */
#define CSMR3 (MBAR+0x8C)

/* 
 * Chip Select Control Mode for the display. Use it to initialize the CSCR3.
 *	AA - Auto-Acknowledge
 *	R - Read Enabled
 *	W - Write Enabled
 * See the MCF5206e User's Manual Section 9-32.
 */
#define CS_AA_R_W (0x0183)

/*
 * Values for the seven segment display consists of an 8-bit number, each bit
 * representing an on/off flag for a particular segment of the display.
 * The below are which bits are associated with which segment (numbers are zerobase
 * bit indexes):
 */
#define SEG_TOP_CENTER		(0x01)
#define SEG_TOP_RIGHT		(0x02)
#define SEG_BOTTOM_RIGHT	(0x04)
#define SEG_BOTTOM_CENTER	(0x08)
#define SEG_BOTTOM_LEFT		(0x10)
#define SEG_TOP_LEFT		(0x20)
#define SEG_MIDDLE		(0x40)
#define SEG_DOT			(0x80)

#define ZERO	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_TOP_LEFT|SEG_BOTTOM_CENTER|SEG_BOTTOM_RIGHT|SEG_BOTTOM_LEFT))
#define ONE	((unsigned char)(SEG_TOP_RIGHT|SEG_BOTTOM_RIGHT))
#define TWO	((unsigned char)(SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_MIDDLE|SEG_BOTTOM_LEFT|SEG_BOTTOM_CENTER))
#define THREE	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_MIDDLE|SEG_BOTTOM_RIGHT|SEG_BOTTOM_CENTER))
#define FOUR	((unsigned char) (SEG_TOP_LEFT|SEG_MIDDLE|SEG_TOP_RIGHT|SEG_BOTTOM_RIGHT))
#define FIVE	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_RIGHT|SEG_BOTTOM_CENTER))
#define SIX	((unsigned char) (SEG_TOP_LEFT|SEG_BOTTOM_LEFT|SEG_MIDDLE|SEG_BOTTOM_CENTER|SEG_BOTTOM_LEFT|SEG_BOTTOM_RIGHT))
#define SEVEN	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_BOTTOM_RIGHT))
#define EIGHT	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_CENTER|SEG_BOTTOM_RIGHT|SEG_BOTTOM_LEFT))
#define NINE	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_RIGHT))
#define A	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_RIGHT|SEG_BOTTOM_LEFT))
#define B	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_CENTER|SEG_BOTTOM_RIGHT|SEG_BOTTOM_LEFT))
#define C	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_LEFT|SEG_BOTTOM_CENTER|SEG_BOTTOM_LEFT))
#define D	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_RIGHT|SEG_TOP_LEFT|SEG_BOTTOM_CENTER|SEG_BOTTOM_RIGHT|SEG_BOTTOM_LEFT))
#define E	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_CENTER|SEG_BOTTOM_LEFT))
#define F	((unsigned char) (SEG_TOP_CENTER|SEG_TOP_LEFT|SEG_MIDDLE|SEG_BOTTOM_LEFT))

/*
 * A pointer to the seven-segment display. 
 * Used to write hexadecimal digits.
 */
unsigned char *pDisplay; 

/* Used to initialize the timer */
short *pTRR1;
short *pTMR1;
short *pTCN1; 

/**
 * @brief Initializes the seven segment display. 
 */
void seven_seg_init(void);
/**
 * @brief Initializes the timer. 
 */
void timer_init(void);

void main( void )
{
	int display_index = 0;
	unsigned char display [] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, A, B, C, D, E, F};
	pDisplay = (unsigned char *)SEVEN_SEG_DISPLAY;
	*pDisplay = 0x00;
	pTRR1    = (short *) TRR1;
	pTMR1    = (short *) TMR1;
	pTCN1    = (short *) TCN1;
	
	seven_seg_init ();
	
	while (1) { 
		// Reset the timer. 
		timer_init ();

		// Idle until timer 1 reaches ~1 second. 
		while (*pTCN1 != *pTRR1);

		// Display next character. 
		*pDisplay = display [display_index++ % 16];
	} 
}

/**
 * @brief Initializes the timer.
 */
void timer_init (void)
{
	/* Set timer frequency: 54000000/16/256/13184 ~= 1sec */
	*pTRR1 = (short)13183;

	/* Clear the timer counter */
	*pTCN1 = (short)0;

	/* Set and start the timer (1111111100011101) */
	*pTMR1 = (short)TMR_CFG;
}


/**
 * @brief Initializes the seven segment display. 
 */
void seven_seg_init (void)
{
	short *pCSCR3 = (short*) CSCR3;
	short *pCSMR3 = (short*) CSMR3;

	/* Set Chip Select Control Register 3 */
	*pCSCR3 = (short)CS_AA_R_W;

	/* Clear Chip Select Mask Register 3 */
	*pCSMR3 = (short)0;
}


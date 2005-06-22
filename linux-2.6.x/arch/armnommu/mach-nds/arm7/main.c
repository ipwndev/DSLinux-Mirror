
#include "asm/types.h"
#include "asm/arch/fifo.h"

extern void swiDelay( u32 duration );
extern void swiWaitForVBlank( void );

#define REG_IPCFIFOSEND	(*(volatile u32*) 0x04000188)
#define REG_IPCFIFORECV	(*(volatile u32*) 0x04100000)
#define REG_IPCFIFOCNT	(*(volatile u16*) 0x04000184)

#define NDS_IE		(*(volatile u32*)0x04000210)	/* Interrupt mask */
#define NDS_IF		(*(volatile u32*)0x04000214)	/* Interrup service */
#define NDS_IME		(*(volatile u32*)0x04000208)	/* Enable/disable */

#define DISP_SR		(*(volatile u16*)0x04000004)

#define DISP_VBLANK_IRQ	(1 << 3)

#define IRQ_VBLANK	(1 << 0)
#define IRQ_RECV	(1 << 18)

#define XKEYS		(*(volatile u16*)0x04000136)

#define TOUCH_CAL_X1 (*(volatile s16*)0x027FFCD8)
#define TOUCH_CAL_Y1 (*(volatile s16*)0x027FFCDA)
#define TOUCH_CAL_X2 (*(volatile s16*)0x027FFCDE)
#define TOUCH_CAL_Y2 (*(volatile s16*)0x027FFCE0)

#define SERIAL_CR      (*(volatile u16*)0x040001C0)
#define SERIAL_DATA    (*(volatile u16*)0x040001C2)

#define SERIAL_ENABLE   0x8000
#define SERIAL_BUSY     0x80

#define TSC_MEASURE_Y        0x94
#define TSC_MEASURE_BATTERY  0xA4
#define TSC_MEASURE_Z1       0xB4
#define TSC_MEASURE_Z2       0xC4
#define TSC_MEASURE_X        0xD4

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

static s32 TOUCH_WIDTH;
static s32 TOUCH_HEIGHT;

u16 touchRead(u32 command) {
	u16 result;
	while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);

	// Write the command and wait for it to complete
	SERIAL_CR = SERIAL_ENABLE | 0xA01;
	SERIAL_DATA = command;
	while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);

	// Write the second command and clock in part of the data
	SERIAL_DATA = 0;
	while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);
	result = SERIAL_DATA;

	// Clock in the rest of the data (last transfer)
	SERIAL_CR = SERIAL_ENABLE | 0x201;
	SERIAL_DATA = 0;
	while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);

	// Return the result
	return ((result & 0x7F) << 5) | (SERIAL_DATA >> 3);
}

void InterruptHandler(void)
{
	u16 buttons;
	u32 data;
	u16 x,y;

	if ( NDS_IF & IRQ_RECV )
	{
		/* Read any FIFO messages */
#if 0
		while ( ! ( REG_IPCFIFOCNT & (1<<3) ) )
		{
			data = REG_IPCFIFORECV;
			/* do something */
		}
#endif
	}
	if ( NDS_IF & IRQ_VBLANK )
	{

		/* read the X, Y and touch buttons */
		buttons = XKEYS;

		/* send to ARM9 */
		REG_IPCFIFOSEND = FIFO_BUTTONS | buttons;

		if (! (buttons & 0x40) )
		{
			x = touchRead(TSC_MEASURE_X);
			y = touchRead(TSC_MEASURE_Y);
			x = ( (x - TOUCH_CAL_X1) * (256-62) ) /
				(TOUCH_WIDTH) + 31;
			y = ( (y - TOUCH_CAL_Y1) * (192-62) ) /
				(TOUCH_HEIGHT) + 31;
			x = MIN(255,MAX(x,0));
			y = MIN(191,MAX(y,0));
			REG_IPCFIFOSEND = FIFO_TOUCH | 1 << 16 | x << 8 | y ;
		}
		else
		{
			REG_IPCFIFOSEND = FIFO_TOUCH ;
		}

		/* clear FIFO errors (just in case) */
		if ( REG_IPCFIFOCNT & (1<<14) )
			REG_IPCFIFOCNT |= (1<<15) | (1<<14);

	}

	/* Acknowledge Interrupts */
	NDS_IF = NDS_IF;
}

int main( void )
{
	/* Disable Interrupts */
	NDS_IME = 0;

	TOUCH_WIDTH  = TOUCH_CAL_X2 - TOUCH_CAL_X1;
	TOUCH_HEIGHT = TOUCH_CAL_Y2 - TOUCH_CAL_Y1;

	/* Enable VBLANK Interrupt */
	DISP_SR = DISP_VBLANK_IRQ;
	NDS_IE = IRQ_VBLANK;

	/* Enable FIFO */
	REG_IPCFIFOCNT = (1<<15) | (1<<3) | (1<<10) | (1<<14);

	/* Set interrupt handler */
	*(volatile u32*)(0x04000000-4) = (u32)&InterruptHandler;

	/* Enable Interrupts */
	NDS_IF = ~0;
	NDS_IME = 1;

	while(1)
	{
		swiWaitForVBlank();
	}
}
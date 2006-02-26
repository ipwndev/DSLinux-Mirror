#include "asm/types.h"
#include "asm/arch/fifo.h"
#include "asm/arch/ipcsync.h"
#include "asm/arch/shmemipc.h"
#include "asm/arch/wifi.h"

#include "sound.h"
#include "arm7.h"
#include "shmemipc-arm7.h"
#include "spi.h"
#include "wifi.h"
#include "time.h"

static s32 xscale, yscale;
static s32 xoffset, yoffset;

/* recieve outstanding FIFO commands from ARM9 */
static void recieveFIFOCommand(void)
{
	u32 data;
	u32 seconds = 0;
	int cmd;

	while (!(REG_IPCFIFOCNT & (1 << 8))) {
		data = REG_IPCFIFORECV;

		switch (data & 0xf0000000) {
		case FIFO_POWER:
			power_write(POWER_CONTROL, POWER0_SYSTEM_POWER);
			break;
		case FIFO_TIME:
			seconds = nds_get_time7();
			REG_IPCFIFOSEND =
			    (FIFO_TIME | FIFO_HIGH_BITS | (seconds >> 16));
			REG_IPCFIFOSEND =
			    (FIFO_TIME | FIFO_LOW_BITS | (seconds & 0xFFFF));
			break;
		case FIFO_SOUND:
			switch (data & 0x0f000000) {
			case FIFO_SOUND_FORMAT:
				sound_set_format(data & 0x3);
				break;
			case FIFO_SOUND_RATE:
				sound_set_rate(data & 0x00ffffff);
				break;
			case FIFO_SOUND_CHANNELS:
				sound_set_channels(data & 0xff);
				break;
			case FIFO_SOUND_DMA_ADDRESS:
				sound_set_address((data & 0x00ffffff) +
						  0x02000000);
				break;
			case FIFO_SOUND_DMA_SIZE:
				sound_set_size(data & 0x00ffffff);
				break;
			case FIFO_SOUND_TRIGGER:
				if (data & 1)
					sound_play();
				else
					sound_stop();
				break;
			case FIFO_SOUND_POWER:
				sound_set_power(data & 0x1);
				break;
			}
			break;
		case FIFO_WIFI:
			cmd = (data >> 18) & 0x3f;
			switch (cmd) {
			case FIFO_WIFI_CMD_UP:
				wifi_open();
				break;
			case FIFO_WIFI_CMD_DOWN:
				wifi_close();
				break;
			case FIFO_WIFI_CMD_MAC_QUERY:
				wifi_mac_query();
				break;
			case FIFO_WIFI_CMD_STATS_QUERY:
				wifi_stats_query();
				break;
			case FIFO_WIFI_CMD_SET_ESSID1:
				Wifi_SetSSID(((data >> 16) & 0x3),
					     (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_ESSID2:
				Wifi_SetSSID(4 + ((data >> 16) & 0x3),
					     (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_ESSID3:
				Wifi_SetSSID(8 + ((data >> 16) & 0x3),
					     (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_ESSID4:
				Wifi_SetSSID(12 + ((data >> 16) & 0x3),
					     (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_CHANNEL:
				Wifi_RequestChannel(data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY0:
				Wifi_SetWepKey(0, ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY0A:
				Wifi_SetWepKey(0, 4 + ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY1:
				Wifi_SetWepKey(1, ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY1A:
				Wifi_SetWepKey(1, 4 + ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY2:
				Wifi_SetWepKey(2, ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY2A:
				Wifi_SetWepKey(2, 4 + ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY3:
				Wifi_SetWepKey(3, ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEY3A:
				Wifi_SetWepKey(3, 4 + ((data >> 16) & 0x3),
					       (data >> 8) & 0xff, data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPKEYID:
				Wifi_SetWepKeyID(data & 0xff);
				break;
			case FIFO_WIFI_CMD_SET_WEPMODE:
				Wifi_SetWepMode(data & 0xff);
				break;
			case FIFO_WIFI_CMD_AP_QUERY:
				wifi_ap_query(data & 0xffff);
				break;
			case FIFO_WIFI_CMD_SCAN:
				wifi_start_scan();
				break;
			case FIFO_WIFI_CMD_SET_AP_MODE:
				Wifi_SetAPMode(data);
				break;
			case FIFO_WIFI_CMD_GET_AP_MODE:
				Wifi_GetAPMode();
				break;
			}
		}
	}
}

#define _MaxRetry 5
#define _MaxRange 30

/* sends touch state to ARM9 */
static void sendTouchState(u16 buttons)
{
	s16 x, y, px, py;
	static u8 lastx = 0, lasty = 0;

	if (buttons & TOUCH_RELEASED) {
		if (lasty != 255)
			REG_IPCFIFOSEND = FIFO_TOUCH;
		lastx = 255;
		lasty = 255;
	} else {		/* Some dude is smacking his fingerprint on the touchscreen. */
                // Code from devkitpro/libnds/source/arm7/touch.c
                x = touch_read_value(TSC_MEASURE_X, _MaxRetry, _MaxRange);
                y = touch_read_value(TSC_MEASURE_Y, _MaxRetry, _MaxRange);

                px = ( x * xscale - xoffset + xscale/2 ) >>19;
                py = ( y * yscale - yoffset + yscale/2 ) >>19;

                if ( px < 0) px = 0;
                if ( py < 0) py = 0;
                if ( px > (256 -1)) px = 256 -1;
                if ( py > (192 -1)) py = 192 -1;

                x = px;
                y = py;

		if (lastx + 6 > x && lastx < x + 6 &&
		    lasty + 6 > y && lasty < y + 6) {
			REG_IPCFIFOSEND = FIFO_TOUCH | 1 << 16 | x << 8 | y;
		}
		lastx = x;
		lasty = y;
	}
}

void InterruptHandler(void)
{
	u16 buttons;
	static u16 oldbuttons;
	u32 wif;

	wif = NDS_IF;

	if (wif & IRQ_RECV) {
		/* Acknowledge Interrupt */
		NDS_IF = IRQ_RECV;
		wif &= ~IRQ_RECV;

		recieveFIFOCommand();
	}

	if (wif & IRQ_VBLANK) {
		/* read X and Y, lid and touchscreen buttons */
		buttons = XKEYS;

		/* send button state to ARM9 */
		if (buttons != oldbuttons) {
			REG_IPCFIFOSEND = FIFO_BUTTONS | buttons;
			oldbuttons = buttons;
		}

		sendTouchState(buttons);

		/* clear FIFO errors (just in case) */
		if (REG_IPCFIFOCNT & (1 << 14))
			REG_IPCFIFOCNT |= (1 << 15) | (1 << 14);

		/* Acknowledge Interrupt */
		NDS_IF = IRQ_VBLANK;
	}

	if (wif & IRQ_TIMER0) {
		/* Acknowledge Interrupt */
		NDS_IF = IRQ_TIMER0;
		wif &= ~IRQ_TIMER0;
		wifi_timer_handler();
	}

	if (wif & IRQ_ARM9) {
		/* Acknowledge Interrupt */
		NDS_IF = IRQ_ARM9;
		wif &= ~IRQ_ARM9;

		switch (ipcsync_get_remote_status()) {
		case SHMEMIPC_REQUEST_FLUSH:
			shmemipc_serve_flush_request();
			break;
		case SHMEMIPC_FLUSH_COMPLETE:
			shmemipc_flush_complete();
			break;
		}

	}

	if (wif & IRQ_WIFI) {
		NDS_IF = IRQ_WIFI;
		wif &= ~IRQ_WIFI;

		wifi_interupt();
	}

	/* Acknowledge Interrupts */
	NDS_IF = wif;
}

int main(void)
{
	/* Disable Interrupts */
	NDS_IME = 0;

	/* Read calibration values, the arm9 will probably overwrite the originals later */
	// Code from devkitpro/libnds/source/arm7/touch.c
	xscale = ((PersonalData->calX2px - PersonalData->calX1px) << 19) / ((PersonalData->calX2) - (PersonalData->calX1));
	yscale = ((PersonalData->calY2px - PersonalData->calY1px) << 19) / ((PersonalData->calY2) - (PersonalData->calY1));
                
	xoffset = ((PersonalData->calX1 + PersonalData->calX2) * xscale  - ((PersonalData->calX1px + PersonalData->calX2px) << 19) ) / 2;
	yoffset = ((PersonalData->calY1 + PersonalData->calY2) * yscale  - ((PersonalData->calY1px + PersonalData->calY2px) << 19) ) / 2;

	/* Enable VBLANK Interrupt */
	DISP_SR = DISP_VBLANK_IRQ;
	NDS_IE = IRQ_VBLANK | IRQ_RECV | IRQ_ARM9 | IRQ_WIFI;

	/* Enable FIFO */
	REG_IPCFIFOCNT = (1 << 15) | (1 << 3) | (1 << 10) | (1 << 14);

	ipcsync_allow_local_interrupt();

	/* Set interrupt handler */
	*(volatile u32 *)(0x04000000 - 4) = (u32) & InterruptHandler;

	/* init the wifi stuff */
	wifi_init();

	/* Enable Interrupts */
	NDS_IF = ~0;
	NDS_IME = 1;

	while (1) {
		swiWaitForVBlank();
	}
}
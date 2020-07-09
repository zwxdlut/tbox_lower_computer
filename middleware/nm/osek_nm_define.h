#ifndef __OSEK_NM_DEFINE_H__
#define __OSEK_NM_DEFINE_H__

/******************************************************************************
 * Definitions
 ******************************************************************************/
 
 //send or receive error count
#define TXLIMIT 8
#define RXLIMIT 4

//nm data length
#define OSEKNM_DLC 8

/* defination of the bits
Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0  type
0     0     0   0/1   0   0    0     1   Alive 
0     0    0/1  0/1   0   0    1     0   Ring 
0     0     0   0/1   0   1    0     0   LimpHome 
bit4 sleep ind 0:clear 1:set
bit5 only for ring, sleep ack.
*/
//the type of the nm kind
#define NMMSGTYPE_ALIVE    (1<<0)
#define NMMSGTYPE_RING     (1<<1)
#define NMMSGTYPE_LIMPHOME (1<<2)
#define NMMSGTYPE_SI       (1<<4)
#define NMMSGTYPE_SA       (1<<5)

//sleep and sleep ack
#define SLEEP_IND_YES			1
#define SLEEP_IND_NO			0
#define SLEEP_ACK_YES			1
#define SLEEP_ACK_NO			0	
#define SLEEP_STATE_YES		1
#define SLEEP_STATE_NO		0

//nm address
#define TBOX_NM_ID_ADDRESS			0x424
#define TBOX_NM_NODE_ADDRESS		0x24
#define TBOX_NM_ID_BASE_ADDRESS	0x400
#define TBOX_NM_ID_MAX					0xff
#define TBOX_NM_ID_MIN					0x00

//nm state
#define NM_OFF      0
#define NM_ON       1
#define NM_SHUTDOWN 2
#define NM_BUSSLEEP 3
#define NM_AWAKE    4
#define NM_INIT     5
#define NM_LIMPHOME 6
#define NM_NORMAL   7
#define NM_RESET    8
#define NM_TWBS_NORMAL        9
#define NM_TWBS_LIMPHOME      10
#define NM_NORMAL_PREPSLEEP   11
#define NM_LIMPHOME_PREPSLEEP 12
#define NM_INIT_RESET         13
#define NM_INIT_LIMPHOME      14
#define NM_INIT_NORMAL        15
#define NM_INIT_BUSSLEEP      17

//time definition
#define NM_TIMETYPE_TTYP     100
#define NM_TIMETYPE_TMAX     260
#define NM_TIMETYPE_TERROR   1000
#define NM_TIMETYPE_TWBS     5000
#define NM_CALLBACK_TIMER		 10
#define NM_TTYP_COUNTER		 	 (NM_TIMETYPE_TTYP/NM_CALLBACK_TIMER)
#define NM_TMAX_COUNTER			 (NM_TIMETYPE_TMAX/NM_CALLBACK_TIMER)

//time for the startup
#define CAN_ACK_TIME					150    // time between enable can and can send the first signal
#define CAN_INIT_TIME					200		 // time between enable can and can send the alive signal
#define MSG_START_TIME				700    // time that all message are sent at least once in a cycle
#define WAKE_UP_LOCAL_TIME		0			 // time send the alive message after wake up
#define WAKE_UP_REMOTE_TIME		50		 // time send the alive message after remote wake up
#define APP_ACTIVE_TIME				80		 // time send the first app message after alive message

//present and limphome node number
#define NM_PRESENTNUM   20
#define NM_LIMPHOMENUM  20

//counter for the timer
#define TIMER_100MS			10
#define TIMER_200MS			20

//state of ring
#define STATE_STABLE				0			// establish the ring
#define STATE_UNSTABLE			1			// do not establish the ring

#endif

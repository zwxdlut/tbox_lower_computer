#include "osek_nm.h"
#include "can.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 
typedef struct {
	unsigned char source_id;
	unsigned char dest_id;
	unsigned char opcode;
}	NM_message;
 
static unsigned char nm_state = NM_OFF;
static void send_nm_message(const char address, const char opcode);
static void send_alive_message(const char address, const char sleep);
static void send_limphome_message(const char address, const char sleep);
static void reset_nm_state(void);
static void dispose_alive_message(const NM_message* const nm_message);
static void dispose_ring_message(const NM_message* const nm_message);
static void dispose_limphome_message(const NM_message* const nm_message);
static void set_typical_alarm(void);
static void cancel_typical_alarm(void);
static unsigned char get_typical_alarm(void);
static void set_max_alarm(void);
static void cancel_max_alarm(void);
static unsigned char get_max_alarm(void);
static void insert_nm_node(unsigned char id);

static unsigned char present_node[NM_PRESENTNUM];
static unsigned char limphome_node[NM_LIMPHOMENUM];

const unsigned char can_channel = 0;
// send flag is the flag that judge when send the alive message, 
// when receive the ack message from other node, set flag to 0;
static unsigned char send_alive_flag = 1;
static unsigned char tx_error_counter, rx_error_counter;
static unsigned char logical_successor = TBOX_NM_ID_MAX;
static unsigned char logical_predecessor = TBOX_NM_ID_MIN;
static unsigned char nm_max_id = TBOX_NM_NODE_ADDRESS;
static unsigned char nm_min_id = TBOX_NM_NODE_ADDRESS;
static unsigned char typical_timer_counter;
static unsigned char typical_timer_alarm;
static unsigned char max_timer_alarm;
static unsigned char max_timer_counter;
static int current_counter;
static unsigned char ring_state = STATE_UNSTABLE;
static unsigned char sleep_state = SLEEP_STATE_NO;


/******************************************************************************
 * Functions
 ******************************************************************************/

void osek_nm_init(void)
{
	reset_nm_state();
	nm_state = NM_ON;
}
 
void osek_nm_uninit(void)
{
	nm_state = NM_OFF;
}

void presleep_state(void) 
{
	nm_state = NM_NORMAL_PREPSLEEP;
}

void reset_nm_state(void)
{
	tx_error_counter = 0;
	rx_error_counter = 0;
	ring_state = STATE_UNSTABLE;
	sleep_state = SLEEP_STATE_NO;
	send_alive_flag = 1;
	logical_successor = TBOX_NM_ID_MAX;
	logical_predecessor = TBOX_NM_ID_MIN;
	nm_max_id = TBOX_NM_NODE_ADDRESS;
	nm_min_id = TBOX_NM_NODE_ADDRESS;
	send_alive_message(TBOX_NM_NODE_ADDRESS, SLEEP_IND_NO);
	set_typical_alarm();
	insert_nm_node(TBOX_NM_NODE_ADDRESS);
}

void send_nm_message(const char address, const char opcode)
{
	uint8_t message[OSEKNM_DLC] = { address, opcode, 0, 0, 0, 0, 0, 0};
	can_send(can_channel, TBOX_NM_ID_ADDRESS, message, OSEKNM_DLC);
}

void send_alive_message(const char address, const char sleep)
{
	if(sleep) {
		send_nm_message(address, NMMSGTYPE_ALIVE|NMMSGTYPE_SI);
	} else {
		send_nm_message(address, NMMSGTYPE_ALIVE);
	}
}

void send_ring_message(const char address, const char sleep, const char sleep_ack)
{
	if(sleep) {
		if (sleep_ack) {
			send_nm_message(address, NMMSGTYPE_RING|NMMSGTYPE_SI|NMMSGTYPE_SA);
		} else {
			send_nm_message(address, NMMSGTYPE_RING|NMMSGTYPE_SI);
		}
	} else {
		if (sleep_ack) {
			send_nm_message(address, NMMSGTYPE_RING|NMMSGTYPE_SA);
		} else {
			send_nm_message(address, NMMSGTYPE_RING);
		}
	}
}

void send_limphome_message(const char address, const char sleep)
{
	if(sleep) {
		send_nm_message(address, NMMSGTYPE_LIMPHOME|NMMSGTYPE_SI);
	} else {
		send_nm_message(address, NMMSGTYPE_LIMPHOME);
	}
}

void make_predecessor_successor() 
{
	// tbox is the first node
	if (logical_predecessor == TBOX_NM_ID_MIN) {
		logical_predecessor = present_node[current_counter]; // predecessor is the last node
	}
	
	if (logical_successor == TBOX_NM_ID_MAX) {
		logical_successor = present_node[0]; //successor is the first node
	}
}

void typical_timer_function()
{
	cancel_typical_alarm();
	// first time when the timeout
	if(ring_state == STATE_UNSTABLE) {
		make_predecessor_successor();
		ring_state = STATE_STABLE;
		if (TBOX_NM_NODE_ADDRESS == present_node[0]) {
			send_ring_message(logical_successor, SLEEP_IND_NO, SLEEP_ACK_NO);
		}
	}
	
	//in our case, gateway send the ack
	send_ring_message(logical_predecessor, sleep_state, SLEEP_ACK_NO);
	set_max_alarm();
}

void max_timer_function()
{
	reset_nm_state();
}


/*
	10ms timer callback
	when ring is not stable, make the predecessor and successor
*/

void osek_nm_time_callback(void)
{
	//for yema requirement
	if(send_alive_flag) {
		send_alive_message(TBOX_NM_NODE_ADDRESS, SLEEP_IND_NO);
		return;
	} else {

	}
	
	//
	if (get_typical_alarm()) {
			typical_timer_counter++;
			if (NM_TTYP_COUNTER == typical_timer_counter) {
				cancel_typical_alarm();
				typical_timer_function();
			}
	}

	if (get_max_alarm()) {
		max_timer_counter++;
		if (NM_TMAX_COUNTER == max_timer_counter) {
			cancel_max_alarm();
			max_timer_function();
		}
	}
}

void osek_nm_indication(const char id, const unsigned char* const buf)
{

	const NM_message nm_message = {
		.source_id = id,
		.dest_id = buf[0],
		.opcode = buf[1]
	};
	
	// if receive the nm message do not send alive message.
	send_alive_flag = 0;
	// cancel typical timer
	cancel_typical_alarm();
	cancel_max_alarm();
	
	switch(nm_message.opcode) {
		case NMMSGTYPE_ALIVE:
			dispose_alive_message(&nm_message);
			break;
		case NMMSGTYPE_RING:
			dispose_ring_message(&nm_message);
			break;
		case NMMSGTYPE_LIMPHOME:
			dispose_limphome_message(&nm_message);
			break;
		default:
			break;
	}
}


void insert_nm_node(unsigned char id) {
	
    //printf("insert nm node %d, %d.\n", id, current_counter);
	int i = 0;
	for (i = 0; i <= current_counter; i++) {
		if (present_node[i] == 0) {
			//printf("==0\n");
			present_node[i] = id;
			break;
		} else {
			if (present_node[i] > id) {
				for (int j = current_counter; j >= i; j--) {
					present_node[j+1] = present_node[j];
				}
				present_node[i] = id;
                break;
			} else {

            }
            
		}
	}
	current_counter++;
}

void remove_nm_node(unsigned char id) {
	for(int i = 0; i <= current_counter; i++) {
		if (present_node[i] == id) {
			for(int j = i; j < current_counter; j++) {
				present_node[j] = present_node[j+1];
			}
		}
	}
	present_node[current_counter] = 0;
	current_counter--;
}

unsigned char find_nm_node(unsigned char id) {
	for (int i = 0; i < current_counter; i++) {
		if (present_node[i] == id) {
			return 1;
		}
	}
	return 0;
}

void dispose_alive_message(const NM_message* const nm_message)
{
	//find max id
	if (nm_message->dest_id > nm_max_id) {
		nm_max_id = nm_message->dest_id;
	}
	//find min id
	if (nm_message->dest_id < nm_min_id) {
		nm_min_id = nm_message->dest_id;
	}
	// confirm predecessor
	if((nm_message->dest_id > logical_predecessor) && (nm_message->dest_id < TBOX_NM_NODE_ADDRESS)){
		logical_predecessor = nm_message->source_id;
	}
	//confirm successor
	if((nm_message->source_id > TBOX_NM_NODE_ADDRESS) && (nm_message->source_id < logical_successor)) {
		logical_successor = nm_message->source_id;
	}
	
	//push the node id to the array
	if (!find_nm_node(nm_message->source_id)) {
		insert_nm_node(nm_message->source_id);
	}
	
	// set the state to unstable.
	ring_state = STATE_UNSTABLE;
}

void dispose_ring_message(const NM_message* const nm_message)
{
	//skipped and send alive message
	if (((nm_message->dest_id > TBOX_NM_NODE_ADDRESS) && (nm_message->source_id < TBOX_NM_NODE_ADDRESS)) ||
				((nm_message->dest_id < nm_message->source_id) && (nm_message->source_id < TBOX_NM_NODE_ADDRESS)) ||
				((nm_message->dest_id < nm_message->source_id) && (nm_message->dest_id > TBOX_NM_NODE_ADDRESS))) {
		reset_nm_state();
		return;
	}
				
	cancel_typical_alarm();
	cancel_max_alarm();
	//update logic predecessor node.
	if(nm_message->dest_id == TBOX_NM_NODE_ADDRESS) {
		logical_predecessor = nm_message->source_id;
		if (nm_message->opcode & NMMSGTYPE_SI) {
			//let mcu sleep
			sleep_state = SLEEP_IND_YES;
			presleep_state();
		}
		set_typical_alarm();
	} else {
		set_max_alarm();
	}
	
}

void dispose_limphome_message(const NM_message* const nm_message)
{
	
}


void set_typical_alarm(void)
{
	typical_timer_alarm = 1;
	typical_timer_counter = 0;
}

void cancel_typical_alarm(void)
{
	typical_timer_alarm = 0;
	typical_timer_counter = 0;
}

unsigned char get_typical_alarm(void)
{
	return typical_timer_alarm;
}

void set_max_alarm(void)
{
	max_timer_alarm = 1;
	max_timer_counter = 0;
}

void cancel_max_alarm(void)
{
	max_timer_alarm = 0;
	max_timer_counter = 0;
}

unsigned char get_max_alarm(void)
{
	return max_timer_alarm;
}


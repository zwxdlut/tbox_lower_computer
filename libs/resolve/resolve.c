/*
 * resolve.c

 *
 *  Created on: 2018Äê8ÔÂ30ÈÕ
 *      Author: Administrator
 */

#include "resolve.h"

#include <stdbool.h>
#include <string.h>
#include <assert.h>

#if defined USING_OS_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
static SemaphoreHandle_t g_upstream_mutex = NULL;
#endif

#include "bitfield/8byte.h"
#include "gen_can_matrix.h"
#include "gen_resolve.h"
#include "bitfield/bitfield.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static uint8_t                      g_upstream_buf[GB32960_DATA_LEN] = {GB32960_INIT_VALUES};
static uint32_t                     g_id = 0;
static uint8_t                      g_data[8];
static resolve_transmit_callback_t  g_transmit_callback  = NULL;
static resolve_delay_callback_t     g_delay_callback  = NULL;
static uint8_t                      g_cell_vol_flag[((CELL_VOL_120_START_BIT - CELL_VOL_001_START_BIT) / CELL_VOL_001_BIT_LEN + 1) / 8];
static uint8_t                      g_probe_temp_flag[((PROBE_TEMP_040_START_BIT - PROBE_TEMP_001_START_BIT) / PROBE_TEMP_001_BIT_LEN + 1) / 8];

#if defined HUACHEN
static uint32_t                     g_click = 0;
static uint8_t                      g_press_data[8];
static uint8_t                      g_no_press_data[8];
#endif

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/*******************************************************************************
 * Functions
 ******************************************************************************/
void resolve_init(const resolve_transmit_callback_t _transmit_callback, const resolve_delay_callback_t _delay_callback)
{
	g_transmit_callback = _transmit_callback;
	g_delay_callback = _delay_callback;
#if defined USING_OS_FREERTOS
	g_upstream_mutex = xSemaphoreCreateMutex();
#endif
	memset(g_cell_vol_flag, 0, sizeof(g_cell_vol_flag));
}

void resolve_deinit(void)
{
#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_upstream_mutex);
#endif
}

void resolve_msg_to_upstream(const uint32_t _id, const uint8_t* const _data, const uint8_t _len)
{
	assert(NULL != _data);
	gen_resolve_msg_to_upstream(_id, _data, _len);
}

void resolve_downstream_to_msg(const uint16_t _id, const uint8_t* const _data, const uint16_t _len)
{
	assert(NULL != _data);

	g_id = 0;

#if defined YEMA
	memset(g_data, 0, sizeof(g_data));
	gen_resolve_downstream_to_msg(_id, _data, _len);
    if(NULL != g_transmit_callback)
    {
		for(uint8_t i = 0 ; i < 3; i++)
		{
	    	g_transmit_callback(g_id, g_data, sizeof(g_data));
	    	g_delay_callback(50);
		}
    }
#elif defined HUACHEN
	g_click = 0;
	*((uint64_t *)(g_data)) = 0xFFFF8FFFFFFFFFFF;
	*((uint64_t *)(g_press_data)) = 0xFFFF8FFFFFFFFFFF;
	*((uint64_t *)(g_no_press_data)) = 0xFFFF8FFFFFFFFFFF;
	gen_resolve_downstream_to_msg(_id, _data, _len);
    if(NULL != g_transmit_callback)
    {
		if (1 == g_click)
		{
			g_transmit_callback(g_id, g_press_data, sizeof(g_press_data));
			g_delay_callback(500);
			g_transmit_callback(g_id, g_no_press_data, sizeof(g_no_press_data));
		}
		else
		{
			g_transmit_callback(g_id, g_data, sizeof(g_data));
		}
    }
#endif
}

uint16_t resolve_get_upstream(uint8_t *const _data, const uint16_t _len)
{
	assert(NULL != _data);

	uint16_t len = 0;

	len = _len > GB32960_DATA_LEN ? GB32960_DATA_LEN : _len;
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_upstream_mutex, portMAX_DELAY);
#endif
	memcpy(_data, g_upstream_buf, len);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_upstream_mutex );
#endif

	return len;
}

uint32_t resolve_get_upstream_item(const uint16_t _start_bit, const uint16_t _bit_len)
{
	uint32_t data = 0;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_upstream_mutex, portMAX_DELAY);
#endif
	data = get_bitfield(g_upstream_buf, GB32960_DATA_LEN, _start_bit, _bit_len);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_upstream_mutex );
#endif
	
	return data;
}

void resolve_set_upstream_item(const uint16_t _start_bit, const uint16_t _bit_len, const uint32_t _item)
{
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_upstream_mutex, portMAX_DELAY);
#endif
	set_bitfield(_item, _start_bit, _bit_len, g_upstream_buf, GB32960_DATA_LEN);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_upstream_mutex );
#endif
}

/**
 * @brief Resolve message to upstream callback function.
 *
 * @param [in] _signal_start_bit Message signal start bit.
 * @param [in] _signal_bit_len   Message signal item bit length.
 * @param [in] _data             Message data.
 * @param [in] _len              Messge  data length.
 * @param [in] _from_list        Covert from table list.
 * @param [in] _to_list          Covert to table list.
 * @param [in] _list_num         Table list number.
 * @param [in] _resolution       Covert resolution.
 * @param [in] _offset           Covert offset.
 * @param [in] _item_start_bit   Upstream item start bit.
 * @param [in] _item_bit_len     Upstream item bit length.
 */
void gen_resolve_msg_to_upstream_callback(const uint8_t _signal_start_bit,
                                          const uint8_t _signal_bit_len,
                                          const uint8_t *const _data,
                                          const uint8_t _len,
                                          const uint32_t *const _from_list,
                                          const uint32_t *const _to_list,
                                          const uint8_t _list_num,
                                          const float _resolution,
                                          const int32_t _offset,
                                          const uint16_t _item_start_bit,
                                          const uint8_t _item_bit_len)
{
    uint64_t data = 0;
    uint64_t temp = 0;
    uint8_t  i = 0;
    bool     hit = false;

    /* Get CAN signal */
#if defined INTEL
	memcpy(&data, _data, _len > sizeof(uint64_t) ? sizeof(uint64_t) : _len);
    data = eightbyte_get_bitfield(data, _signal_start_bit, _signal_bit_len, false);
#elif defined MOTOROLA
    data = get_bitfield(_data, _len,_signal_start_bit, _signal_bit_len);
#endif

    /* Convert */
    if(NULL != _from_list && NULL != _to_list)
    {
    	for(i = 0; i < _list_num; i++)
    	{
    		if(data == _from_list[i])
    		{
    			data = _to_list[i];
    			hit = true;
    			break;
    		}
    	}
    }
    if(!hit)
    	data = data * _resolution + _offset;

#if defined YEMA
	/* Correct speed */
    if(SPEED_START_BIT == _item_start_bit &&
#if defined INTEL
    		1 == eightbyte_get_bitfield(*((uint64_t*)_data), SIGNAL_VehicleSpeedValid_START_BIT, SIGNAL_VehicleSpeedValid_BIT_LEN, false);
#elif defined MOTOROLA
    		1 == get_bitfield(_data, _len, SIGNAL_VehicleSpeedValid_START_BIT, SIGNAL_VehicleSpeedValid_BIT_LEN))
#endif
		data = bitmask(SPEED_BIT_LEN);
#elif defined HUACHEN
	uint32_t lamp = 0;

	if(LT_LAMP_START_BIT == _item_start_bit)
	{
		if (0x00 == data)
		{
			// LT_LAMP:close RT_LAMP:close
			data = 0x02;
			lamp = 0x02;
			set_bitfield(lamp, RT_LAMP_START_BIT, _item_bit_len, g_upstream_buf, GB32960_DATA_LEN);
		}
		else if (0x01 == data)
		{
			// LT_LAMP:open RT_LAMP:close
			data = 0x01;
			lamp = 0x02;
			set_bitfield(lamp, RT_LAMP_START_BIT, _item_bit_len, g_upstream_buf, GB32960_DATA_LEN);
		}
		else if (0x02 == data)
		{
			// LT_LAMP:close RT_LAMP:open
			lamp = 0x01;
			set_bitfield(lamp, RT_LAMP_START_BIT, _item_bit_len, g_upstream_buf, GB32960_DATA_LEN);
		}
	}
#endif
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_upstream_mutex, portMAX_DELAY);
#endif
	/* Correct charging status */
	if(CHARGING_STATUS_START_BIT == _item_start_bit)
	{
		temp =  get_bitfield(g_upstream_buf, GB32960_DATA_LEN, SPEED_START_BIT, SPEED_BIT_LEN);
		if(0x01 == data && bitmask(SPEED_BIT_LEN) != temp && 30 < temp)
			data = 0x02;
	}

	/* Calculate cell number */
	else if(CELL_VOL_001_START_BIT <= _item_start_bit && CELL_VOL_120_START_BIT >= _item_start_bit)
	{
		temp =  get_bitfield(g_upstream_buf, GB32960_DATA_LEN, CELL_NUM_START_BIT, CELL_NUM_BIT_LEN);
		i = (_item_start_bit - CELL_VOL_001_START_BIT) / CELL_VOL_001_BIT_LEN;
		if(0 == (g_cell_vol_flag[i / 8] & (1 << (i % 8))))
		{
			if(bitmask(CELL_VOL_001_BIT_LEN) != data)
			{
				temp++;
				g_cell_vol_flag[i / 8] |= 1 << (i % 8);
			}
		}
		else
		{
			if(bitmask(CELL_VOL_001_BIT_LEN) == data)
			{
				temp--;
				g_cell_vol_flag[i / 8] &= ~(1 << (i % 8));
			}
		}
		set_bitfield(temp, CELL_NUM_START_BIT, CELL_NUM_BIT_LEN, g_upstream_buf, GB32960_DATA_LEN);
	}

	/* Calculate probe temperature number */
	else if(PROBE_TEMP_001_START_BIT <= _item_start_bit && PROBE_TEMP_040_START_BIT >= _item_start_bit)
	{
		temp =  get_bitfield(g_upstream_buf, GB32960_DATA_LEN, PROBE_TEMP_NUM_START_BIT, PROBE_TEMP_NUM_BIT_LEN);
		i = (_item_start_bit - PROBE_TEMP_001_START_BIT) / PROBE_TEMP_001_BIT_LEN;
		if(0 == (g_probe_temp_flag[i / 8] & (1 << (i % 8))))
		{
			if(bitmask(PROBE_TEMP_001_BIT_LEN) != data)
			{
				temp++;
				g_probe_temp_flag[i / 8] |= 1 << (i % 8);
			}
		}
		else
		{
			if(bitmask(PROBE_TEMP_001_BIT_LEN) == data)
			{
				temp--;
				g_probe_temp_flag[i / 8] &= ~(1 << (i % 8));
			}
		}
		set_bitfield(temp, PROBE_TEMP_NUM_START_BIT, PROBE_TEMP_NUM_BIT_LEN, g_upstream_buf, GB32960_DATA_LEN);
	}

	/* Set upstream item(big endian) */
	set_bitfield(data, _item_start_bit, _item_bit_len, g_upstream_buf, GB32960_DATA_LEN);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_upstream_mutex );
#endif
}

/**
 * @brief Resolve downstream to message callback function.
 *
 * @param _item_start_bit   Downstream item start bit.
 * @param _item_bit_len     Downstream item bit length.
 * @param _data             Downstream data.
 * @param _len              Downstream data length.
 * @param _from_list        Covert from table list.
 * @param _to_list          Covert to table list.
 * @param _list_num         Table list number.
 * @param _resolution       Covert resolution.
 * @param _offset           Covert offset.
 * @param _id               Message ID.
 * @param _signal_start_bit Message signal start bit.
 * @param _signal_bit_len   Message signal bit length.
 */
void gen_resolve_downstream_to_msg_callback(const uint8_t _item_start_bit,
                                            const uint8_t _item_bit_len,
                                            const uint8_t *const _data,
                                            const uint16_t _len,
                                            const uint32_t *const _from_list,
                                            const uint32_t *const _to_list,
                                            const uint8_t _list_num,
                                            const float _resolution,
                                            const int32_t _offset,
                                            const uint32_t _id,
                                            const uint8_t _signal_start_bit,
                                            const uint8_t _signal_bit_len)
{
    uint64_t data = 0;
	uint8_t  i = 0;
    bool     hit = false;

    /* Get downstream item(little endian)*/
	memcpy(&data, _data, _len > sizeof(uint64_t) ? sizeof(uint64_t) : _len);
	data = eightbyte_get_bitfield(data, _item_start_bit, _item_bit_len, false);

	/* Convert */
    if(NULL != _from_list && NULL != _to_list)
    {
    	for(i = 0; i < _list_num; i++)
    	{
    		if(data == _from_list[i])
    		{
    			data = _to_list[i];
    			hit = true;
    			break;
    		}
    	}
    }
    if(!hit)
    	data = data * _resolution + _offset;

    /* Set CAN signal */
    g_id = _id;
#if defined INTEL
#if defined HUACHEN
	if (MSG_ID_MHU_SET_COMFORT_FUNC_I == _id)
	{
		if (SIGNAL_SET_AirCondition_START_BIT == _signal_start_bit  ||
			SIGNAL_SET_FrontDefrost_START_BIT == _signal_start_bit  ||
			SIGNAL_SET_RearDefrost_START_BIT == _signal_start_bit   ||
			SIGNAL_SET_MaxAc_START_BIT == _signal_start_bit         ||
			SIGNAL_SET_AutoAC_START_BIT == _signal_start_bit        ||
			SIGNAL_SET_Recirculation_START_BIT == _signal_start_bit ||
			SIGNAL_SET_Ionizer_START_BIT == _signal_start_bit       ||
			SIGNAL_SET_OffButton_START_BIT == _signal_start_bit)
		{
			if (0x01 == data)
			{
				g_click = 1;
				eightbyte_set_bitfield(0x00, _signal_start_bit, _signal_bit_len, (uint64_t*)g_press_data);
				eightbyte_set_bitfield(0x01, _signal_start_bit, _signal_bit_len, (uint64_t*)g_no_press_data);
			}
		}
		else if (SIGNAL_SET_TempDriver_START_BIT == _signal_start_bit    ||
				 SIGNAL_SET_TempPassenger_START_BIT == _signal_start_bit ||
				 SIGNAL_SET_BlowSetting_START_BIT == _signal_start_bit)
		{
			eightbyte_set_bitfield(data, _signal_start_bit, _signal_bit_len, (uint64_t*)g_data);
		}
		else if (SIGNAL_SET_FlapPosition_START_BIT == _signal_start_bit)
		{
			g_click = 1;
			eightbyte_set_bitfield(data, _signal_start_bit, _signal_bit_len, (uint64_t*)g_press_data);
		}
		return;
	}
#endif
    eightbyte_set_bitfield(data, _signal_start_bit, _signal_bit_len, (uint64_t*)g_data);
#elif defined MOTOROLA
    set_bitfield(data, _signal_start_bit, _signal_bit_len, g_data, sizeof(g_data));
#endif
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

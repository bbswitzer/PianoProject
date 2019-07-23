#include "bluetooth.h"
#include "midi.h"
#include <esp_gatt_defs.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_main.h>
#include <esp_gatts_api.h>

uint8_t attrStr[] ={ 0x00 }; //value range of a characteristic
esp_attr_value_t gattsAttrVal =
{
	.attr_max_len = 0xFF, //max value of a characteristic
	.attr_len     = sizeof(attrStr),
	.attr_value   = attrStr,
};
uint8_t serviceUUID128[32] =
{
	0x00, 0xC7, 0xC4, 0x4E, 0xE3, 0x6C, 0x51, 0xA7, 0x33, 0x4B, 0xE8, 0xED, 0x5A, 0x0E, 0xB8, 0x03,
	0xF3, 0x6B, 0x10, 0x9D, 0x66, 0xF2, 0xA9, 0xA1, 0x12, 0x41, 0x68, 0x38, 0xDB, 0xE5, 0x72, 0x77
};
uint8_t rawAdvData[] =
{
		0x02, 0x01, 0x06,
		0x11, 0x07, 0x00, 0xC7, 0xC4, 0x4E, 0xE3, 0x6C, 0x51, 0xA7, 0x33, 0x4B, 0xE8, 0xED, 0x5A, 0x0E, 0xB8, 0x03,
};
uint8_t rawScanRspData[] ={ 0x0c, 0x09, 'P','l','a','y','e','r',' ','P','I','A','N','O' };


esp_bt_uuid_t characteristicUUID;
esp_bt_uuid_t descriptorUUID;
esp_gatt_perm_t permissions = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
esp_gatt_char_prop_t property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
esp_ble_adv_params_t BLEAdvParams;

static void gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gattsInterface, esp_ble_gatts_cb_param_t* params);
void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* params);

void initializeBluetooth()
{
	//writing the parameters for the bluetooth on BLEAdvParams, which a struct
	BLEAdvParams.adv_int_min        = 0x20;
	BLEAdvParams.adv_int_max        = 0x30;
	BLEAdvParams.adv_type           = ADV_TYPE_IND;
	BLEAdvParams.own_addr_type      = BLE_ADDR_TYPE_PUBLIC;
	BLEAdvParams.channel_map        = ADV_CHNL_ALL;
	BLEAdvParams.adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
	characteristicUUID.len = ESP_UUID_LEN_128;
	for(int i=0; i < 16; i++)
		characteristicUUID.uuid.uuid128[i] = serviceUUID128[i + 16];

	btStart();
	esp_bluedroid_init();
	esp_bluedroid_enable();
	esp_ble_gatts_register_callback(gattsEventHandler); //log the callback function for gatts even handling
	esp_ble_gap_register_callback(gapEventHandler); //log the callback function for gap event handling
	esp_ble_gatts_app_register(0);
}

static void gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gattsInterface, esp_ble_gatts_cb_param_t* params)
{
	switch(event)
	{
	case ESP_GATTS_REG_EVT: //create service
		if(params->reg.status != ESP_GATT_OK)
			return;

		esp_gatt_srvc_id_t serviceID;
		serviceID.is_primary = true;
		serviceID.id.inst_id = 0x00;
		serviceID.id.uuid.len = ESP_UUID_LEN_128;
		for(int i=0; i < 16; i++)
			serviceID.id.uuid.uuid.uuid128[i] = serviceUUID128[i];

		esp_ble_gap_set_device_name("Player Piano");
		esp_ble_gap_config_adv_data_raw(rawAdvData, sizeof(rawAdvData));
		esp_ble_gap_config_scan_rsp_data_raw(rawScanRspData, sizeof(rawScanRspData));
		esp_ble_gatts_create_service(gattsInterface, &serviceID, 4);
		break;

	case ESP_GATTS_READ_EVT: //when main device requests data
		esp_gatt_rsp_t response;
		memset(&response, 0, sizeof(esp_gatt_rsp_t));
		response.attr_value.handle = params->read.handle;
		response.attr_value.len = 1;
		response.attr_value.value[0] = 0;
		esp_ble_gatts_send_response(gattsInterface, params->read.conn_id, params->read.trans_id, ESP_GATT_OK, &response);
		break;

	case ESP_GATTS_WRITE_EVT: //when main device sends data
		extern boolean acceptMidi;
		if(acceptMidi)
			decodeBluetooth(params->write.len, params->write.value);
		break;

	case ESP_GATTS_CREATE_EVT: //start service and characteristic
		esp_ble_gatts_add_char(params->create.service_handle, &characteristicUUID, permissions, property, &gattsAttrVal, NULL);
		esp_ble_gatts_start_service(params->create.service_handle);
		break;

	case ESP_GATTS_DISCONNECT_EVT:
		esp_ble_gap_start_advertising(&BLEAdvParams);
		break;
	case ESP_GATTS_CONNECT_EVT: //connection
	{
		esp_ble_conn_update_params_t conn_params ={ 0 };
		memcpy(conn_params.bda, params->connect.remote_bda, sizeof(esp_bd_addr_t));
		conn_params.latency = 0;
		conn_params.max_int = 0x30;
		conn_params.min_int = 0x20;
		conn_params.timeout = 500;
		esp_ble_gap_update_conn_params(&conn_params);
		break;
	}
	default:
		break;
	}
}

void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* params)
{
	switch(event) {
	case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
		esp_ble_gap_start_advertising(&BLEAdvParams);
		break;
	case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
		esp_ble_gap_start_advertising(&BLEAdvParams);
		break;
	case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
		esp_ble_gap_start_advertising(&BLEAdvParams);
		break;
	default:
		break;
	}
}
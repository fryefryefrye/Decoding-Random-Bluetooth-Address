

#include<WiFi.h>
#include<WiFiUdp.h>
#include<ArduinoOTA.h>
#include<time.h>
#define timezone 8

#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#include "irk.h"

#define IRK_LIST_NUMBER 2
char * IrkListName[IRK_LIST_NUMBER] = {"A","B"};
uint8_t irk[IRK_LIST_NUMBER][ESP_BT_OCTET16_LEN]= 
{
	//IRK of A
	{0x92,0xE1,0x70,0x7B,0x84,0xDC,0x21,0x4D,0xA6,0x33,0xDC,0x3A,0x3A,0xB2,0x08,0x3F}
	//IRK of B
	,{0x2E,0xB7,0xB3,0xD4,0xDC,0x5C,0x16,0x73,0xA7,0x9B,0x75,0x0E,0xEC,0xEB,0x60,0x2D}
};

char *time_str;   
char H1,H2,M1,M2,S1,S2;

unsigned long TenthSecondsSinceStart = 0;
unsigned long SecondsSinceStart = 0;
uint32_t duration = 60;

const char* ssid = "xxx";
const char* password = "xxx";

void NonStopTask();

void TenthSecondsSinceStartTask();
void OnTenthSecond();
void OnSecond();
void SacnBleDevice();
float calcDistByRSSI(int rssi);
void BT_Init();

esp_ble_scan_params_t ble_scan_params;


void setup() 
{       
	delay(50);                      
	Serial.begin(115200);

	// WiFi.disconnect();
	// WiFi.mode(WIFI_STA);

	// Serial.print("Is connection routing, please wait");  
	// WiFi.begin(ssid, password); //Wifi接入到网络
	// Serial.println("\nConnecting to WiFi");
	// //如果Wifi状态不是WL_CONNECTED，则表示连接失败
	// while (WiFi.status() != WL_CONNECTED) {  
	// 	Serial.print("."); 
	// 	delay(1000);    //延时等待接入网络
	// }



	// //设置时间格式以及时间服务器的网址
	// configTime(timezone * 3600, 0, "pool.ntp.org", "time.nist.gov");
	// Serial.println("\nWaiting for time");
	// while (!time(nullptr)) {
	// 	Serial.print(".");
	// 	delay(1000);    
	// }
	// Serial.println("");

	// ArduinoOTA.onStart([]() {
	// 	String type;
	// 	if (ArduinoOTA.getCommand() == U_FLASH) {
	// 		type = "sketch";
	// 	} else { // U_SPIFFS
	// 		type = "filesystem";
	// 	}

	// 	// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
	// 	Serial.println("Start updating " + type);
	// });
	// ArduinoOTA.onEnd([]() {
	// 	Serial.println("\nEnd");
	// });
	// ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
	// 	Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	// });
	// ArduinoOTA.onError([](ota_error_t error) {
	// 	Serial.printf("Error[%u]: ", error);
	// 	if (error == OTA_AUTH_ERROR) {
	// 		Serial.println("Auth Failed");
	// 	} else if (error == OTA_BEGIN_ERROR) {
	// 		Serial.println("Begin Failed");
	// 	} else if (error == OTA_CONNECT_ERROR) {
	// 		Serial.println("Connect Failed");
	// 	} else if (error == OTA_RECEIVE_ERROR) {
	// 		Serial.println("Receive Failed");
	// 	} else if (error == OTA_END_ERROR) {
	// 		Serial.println("End Failed");
	// 	}
	// });
	// ArduinoOTA.begin();
	// Serial.println("Ready");
	// Serial.print("IP address: ");
	// Serial.println(WiFi.localIP());

	ble_scan_params.scan_type              = BLE_SCAN_TYPE_PASSIVE;
	ble_scan_params.own_addr_type          = BLE_ADDR_TYPE_PUBLIC;
	ble_scan_params.scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL;
	ble_scan_params.scan_interval          = 0x50;
	ble_scan_params.scan_window            = 0x30;
	ble_scan_params.scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE;


	BT_Init();


}

void loop() 
{
	NonStopTask();
}

void NonStopTask()
{
	ArduinoOTA.handle();

	TenthSecondsSinceStartTask();
}


unsigned long LastMillis = 0;
void TenthSecondsSinceStartTask()
{
	unsigned long CurrentMillis = millis();
	if ((CurrentMillis - LastMillis) > 100)
	{
		LastMillis = CurrentMillis;
		TenthSecondsSinceStart++;
		OnTenthSecond();
	}
}

void OnSecond()
{
	SecondsSinceStart++;
	time_t now = time(nullptr); //获取当前时间
	time_str = ctime(&now);
	H1 = time_str[11];
	H2 = time_str[12];
	M1 = time_str[14];
	M2 = time_str[15];
	S1 = time_str[17];
	S2 = time_str[18];
	//printf("%c%c:%c%c:%c%c\n",H1,H2,M1,M2,S1,S2);
	//Serial.printf(time_str);

	struct   tm     *timenow;
	timenow   =   localtime(&now);
	unsigned char Hour = timenow->tm_hour;
	unsigned char Minute = timenow->tm_min;

	if (SecondsSinceStart == 2)
	{
		esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
		if (scan_ret){
			//ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
		};
	}


	if ((SecondsSinceStart%60 == 3)&&(SecondsSinceStart > 3))
	{
		esp_ble_gap_start_scanning(duration);
	}
}

void OnTenthSecond()
{
	if (TenthSecondsSinceStart%10 == 0)
	{
		OnSecond();
	}
}


void SacnBleDevice()
{
	//int count = foundDevices.getCount();
	//printf("found %d Devices\r\n",count);
}

float calcDistByRSSI(int rssi)
{
	int iRssi = abs(rssi);
	float power = (iRssi-59)/(10*2.0);
	return pow(10, power);
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	esp_err_t ret;
	uint8_t *adv_name = NULL;
	uint8_t *pManufacturer = NULL;

	uint8_t DataLen = 0;
	esp_ble_gap_cb_param_t *scan_result;

	//ESP_LOGV(GATTS_TABLE_TAG, "GAP_EVT, event:%d %s", event,gapEventToString(event));
	//ESP_LOGV(GATTS_TABLE_TAG, "GAP_EVT, event:%d", event);

	switch (event) 
	{
		case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        //the unit of the duration is second
        esp_ble_gap_start_scanning(duration);
        break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(__func__, "scan start failed, error status = %x", param->scan_start_cmpl.status);
            break;
        }
        ESP_LOGI(__func__, "scan start success");

        break;

	case ESP_GAP_BLE_SCAN_RESULT_EVT: 

			scan_result = (esp_ble_gap_cb_param_t *)param;

			if(scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) 
			{
				esp_log_buffer_hex(__func__, scan_result->scan_rst.bda, 6);
				printf("BD Address 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\r\n"
					,scan_result->scan_rst.bda[0]
				,scan_result->scan_rst.bda[1]
				,scan_result->scan_rst.bda[2]
				,scan_result->scan_rst.bda[3]
				,scan_result->scan_rst.bda[4]
				,scan_result->scan_rst.bda[5]);


					for (byte i = 0; i < IRK_LIST_NUMBER; i++)
					{
						if (btm_ble_addr_resolvable(scan_result->scan_rst.bda, irk[i]))
						{
							printf("MacAdd = %02X %02X %02X %02X %02X %02X Found:%s\r\n"
								, scan_result->scan_rst.bda[0]
								, scan_result->scan_rst.bda[1]
								, scan_result->scan_rst.bda[2]
								, scan_result->scan_rst.bda[3]
								, scan_result->scan_rst.bda[4]
								, scan_result->scan_rst.bda[5]
								, IrkListName[i]);

							return;
						}
					}
			}


		break;
	
	default:
		ESP_LOGI(GATTS_TABLE_TAG, "event not process");
		break;
	}
}


void BT_Init()
{
	//ESP_LOGE(GATTS_TABLE_TAG, "GATTS Starting...");

	btStart();

	esp_err_t errRc = ESP_OK;

	if(nvs_flash_init() != ESP_OK)
	{
		printf("nvs_flash_init failed \r\n");
	}  

	esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();

	if (bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
		if(esp_bluedroid_init() != ESP_OK)
		{
			printf("esp_bluedroid_init failed \r\n");
		}  
	}

	if (bt_state != ESP_BLUEDROID_STATUS_ENABLED) {
		if(esp_bluedroid_enable() != ESP_OK)
		{
			printf("esp_bluedroid_enable failed \r\n");
		}  
	}

	esp_err_t ret;

	ret = esp_ble_gap_register_callback(gap_event_handler);
	if (ret){
		//ESP_LOGE(GATTS_TABLE_TAG, "gap register error, error code = %x", ret);
		return;
	}

}
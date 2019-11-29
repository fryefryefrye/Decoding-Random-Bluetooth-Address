#include <SPI.h>
#include <RF24.h>
#include <RF24BLE.h>
#include <printf.h>

#include "irk.h"



#define IRK_LIST_NUMBER 2char * IrkListName[IRK_LIST_NUMBER] = {"A","B"};uint8_t irk[IRK_LIST_NUMBER][ESP_BT_OCTET16_LEN]= {	//IRK of A	{0x92,0xE1,0x70,0x7B,0x84,0xDC,0x21,0x4D,0xA6,0x33,0xDC,0x3A,0x3A,0xB2,0x08,0x3F}	//IRK of B	,{0x17,0x0A,0xE5,0xA7,0xEF,0x8C,0xA8,0xBA,0x06,0xC1,0x54,0xEF,0x9A,0x7A,0x34,0xD0}};



#define MAC_LEN 6
#define RECV_PAYLOAD_SIZE 28

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
RF24BLE BLE(radio);
/**********************************************************/

void BleDataCheckTask();
unsigned char input[32]={0};

void setup()
{
    Serial.begin(115200);
    Serial.println(F("RF24_BLE_address"));
    printf_begin();
}




void loop()
{
	BleDataCheckTask();
} // Loop





void BleDataCheckTask()
{
	byte status=BLE.recvPacket((uint8_t*)input,RECV_PAYLOAD_SIZE,1);

	unsigned char AdMac[MAC_LEN];
	//0x40 = Advertising package with a random private address. 
	if(input[0]==0x40)
	{
		//Get the MAC address. Reverse order in BT payload.
		for (byte i = 0; i < MAC_LEN; i++)
		{
			AdMac[MAC_LEN-1-i] = input[i+2];
		}
		printf("Check = %02X %02X %02X %02X %02X %02X\r\n"
			,AdMac[0],AdMac[1],AdMac[2],AdMac[3],AdMac[4],AdMac[5]);

		for (byte i = 0; i < IRK_LIST_NUMBER; i++)
		{
			//Check with all IRK we got one by one.
			if(btm_ble_addr_resolvable(AdMac,irk[i]))
			{
				printf("MacAdd= %02X %02X %02X %02X %02X %02X Belongs to:%s\r\n"
					,AdMac[0],AdMac[1],AdMac[2],AdMac[3],AdMac[4],AdMac[5]
				,IrkListName[i]);
			}
		}
		return;
	}
}



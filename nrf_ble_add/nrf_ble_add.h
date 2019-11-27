#include <SPI.h>
#include "RF24.h"
#include "RF24BLE.h"
#include <printf.h>

#include "irk.h"


#define MAC_LEN 6
#define IRK_LIST_NUMBER 2char * IrkListName[IRK_LIST_NUMBER] = {"A","B"};uint8_t irk[IRK_LIST_NUMBER][ESP_BT_OCTET16_LEN]= {	{0x92,0xE1,0x70,0x7B,0x84,0xDC,0x21,0x4D,0xA6,0x33,0xDC,0x3A,0x3A,0xB2,0x08,0x3F}	,{0x17,0x0A,0xE5,0xA7,0xEF,0x8C,0xA8,0xBA,0x06,0xC1,0x54,0xEF,0x9A,0x7A,0x34,0xD0}};
//EXPECTED receive packet length must be same as that of the length of the sent packet
#define RECV_PAYLOAD_SIZE 28


/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
RF24BLE BLE(radio);
/**********************************************************/

void BleDataCheckTask();

void SecondsSinceStartTask();
void OnSeconds();
bool ParseData(unsigned char * data,unsigned char len,unsigned char code,unsigned char * results);
unsigned long SecondsSinceStart;
unsigned char input[32]={0};

void setup()
{



    Serial.begin(115200);
    Serial.println(F("RF24_BLE_address"));
    printf_begin();

	SPI.begin();
	radio.begin();
	radio.setPALevel(RF24_PA_MIN);


	radio.closeReadingPipe(1);
	radio.closeReadingPipe(2);
	radio.closeReadingPipe(3);
	radio.closeReadingPipe(4);
	radio.closeReadingPipe(5);
	radio.setAutoAck(1, false);
	radio.setAutoAck(2, false);
	radio.setAutoAck(3, false);
	radio.setAutoAck(4, false);
	radio.setAutoAck(5, false);
	radio.setAutoAck(0, false);


	BLE.recvRestar(0);//recv
}




void loop()
{
	BleDataCheckTask();
    SecondsSinceStartTask();
} // Loop


bool ParseData(unsigned char * data,unsigned char len,unsigned char code,unsigned char * results)
{
	for (byte i = 0; i < RECV_PAYLOAD_SIZE; i++)
	{
		if (data[i] == len)
		{
			if (data[i+1] == code)
			{
				byte j;
				for (j = 0; j < len; j++)
				{
					results[j] = data[i+j+2];
				}
				results[j-1] = 0;
				return true;
			}
		}
	}
	return false;
}


void BleDataCheckTask()
{
	byte status=BLE.recvPacket((uint8_t*)input,RECV_PAYLOAD_SIZE);

	unsigned char AdMac[MAC_LEN];
	if(input[0]==0x40)
	{
		for (byte i = 0; i < MAC_LEN; i++)
		{
			AdMac[MAC_LEN-1-i] = input[i+2];
		}
		printf("Check = %02X %02X %02X %02X %02X %02X\r\n"
			,AdMac[0],AdMac[1],AdMac[2],AdMac[3],AdMac[4],AdMac[5]);

		for (byte i = 0; i < IRK_LIST_NUMBER; i++)
		{
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

unsigned long LastMillis = 0;
void SecondsSinceStartTask()
{
    unsigned long CurrentMillis = millis();
    if (abs(CurrentMillis - LastMillis) > 1000)
    {
        LastMillis = CurrentMillis;
        SecondsSinceStart++;
		OnSeconds();
		//printf("SecondsSinceStart = %d \r\n",SecondsSinceStart);
    }
}


void OnSeconds()
{
	BLE.recvChannelRoll();
}

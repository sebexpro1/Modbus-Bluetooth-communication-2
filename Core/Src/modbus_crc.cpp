

#include "stdint.h"
#include <stdio.h>
#include "main.h"
#include "modbus_crc.h"

extern uint8_t RxData_on_array[64];
extern uint8_t RxData_off_array[64];
extern uint8_t RxData_on_array_AT[64];
extern uint8_t RxData_off_array_AT[64];
extern UART_HandleTypeDef huart4;
extern uint8_t RxData[64];
extern uint8_t TxData[64];
extern uint8_t AT_active[32];
extern uint8_t ATMode;
extern uint8_t BTconnectMode;
extern int bytes;
/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_lo ^ *buffer++; /* calculate the CRC  */
        crc_lo = crc_hi ^ table_crc_hi[i];
        crc_hi = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

void modbus_on_compare_array(){
	RxData_on_array[0] = 0x27;
	RxData_on_array[1] = 0xd;
	RxData_on_array[2] = 0xa;
	RxData_on_array[3] = 0x39;
	RxData_on_array[4] = 0x44;
	RxData_on_array[5] = 0x44;
	RxData_on_array[6] = 0x45;
	RxData_on_array[7] = 0x43;
	RxData_on_array[8] = 0x54;
	RxData_on_array[9] = 0x20;
	RxData_on_array[10] = 0x20;
	RxData_on_array[11] = 0x27;
}
void modbus_on_compare_array_AT(){

	//	RxData_on_array_AT[2] = 0x43; //C
	//	RxData_on_array_AT[3] = 0x4f; //O
	//	RxData_on_array_AT[4] = 0x4e; //N
	//	RxData_on_array_AT[5] = 0x4e; //N
	//	RxData_on_array_AT[6] = 0x45; //E
		RxData_on_array_AT[7] = 0x43; //C
		RxData_on_array_AT[8] = 0x54; //T


}
void modbus_off_compare_array_AT(){

	//	RxData_off_array_AT[2] = 0x44;  //D
		//RxData_off_array_AT[3] = 0x49;  //I
	//	RxData_off_array_AT[4] = 0x53;  //S
		//RxData_off_array_AT[5] = 0x43;  //C
		//RxData_off_array_AT[6] = 0x4f;  //O
		//RxData_off_array_AT[7] = 0x4e;  //N
		//RxData_off_array_AT[8] = 0x4e;  //N
		//RxData_off_array_AT[9] = 0x45;  //E
		RxData_off_array_AT[10] = 0x43; //C
		RxData_off_array_AT[11] = 0x54; //T


}
void modbus_off_compare_array(){
	RxData_off_array[0] = 0x27;
	RxData_off_array[1] = 0xd;
	RxData_off_array[2] = 0xa;
	RxData_off_array[3] = 0x39;
	RxData_off_array[4] = 0x44;
	RxData_off_array[5] = 0x44;
	RxData_off_array[6] = 0x4f;
	RxData_off_array[7] = 0x4e;
	RxData_off_array[8] = 0x4e;
	RxData_off_array[9] = 0x45;
	RxData_off_array[10] = 0x43;
	RxData_off_array[11] = 0x54;
	RxData_off_array[12] = 0x20;
	RxData_off_array[13] = 0x20;
	RxData_off_array[14] = 0x27;
}

int compareArrays(uint8_t arr1[], uint8_t arr2[], int size, int i_start) {
    for (int i = i_start; i < size; ++i) {
        if (arr1[i] != arr2[i]) {
            return 0;
        }
    }
    return 1;
}

int clearArray(uint8_t arr1[], int size) {
    for (int i = 0; i < size; ++i) {
        arr1[i] = 0;
    }
    return 1;
}

void BTM222_init_reset()
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0,GPIO_PIN_SET);
	HAL_Delay(150);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_Delay(150);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0,GPIO_PIN_SET);
	//ATMode = 1;
	//BTconnectMode = 0;
}

void ATMode_on()
{
	uint8_t buff_tosend[8];
	uint8_t buff_tosend2[8];
	sprintf((char *)buff_tosend, "+++\n");

	HAL_Delay(1100);
	HAL_UART_Transmit(&huart4,buff_tosend, 8, 1000);
	//HAL_UART_Receive(&huart4, AT_active, 8, 1000);
	HAL_Delay(1000);
	//clearArray(RxData, 32);
	//sprintf((char *)buff_tosend2, "ATP?\n");
	//HAL_UART_Transmit(&huart4,buff_tosend2, 8, 1000);
	//HAL_UART_Receive(&huart4, AT_active, 8, 1000);

}

void ATMode_Pcom()
{
	uint8_t buff_tosen[8];
	uint8_t buff_tosen2[8];
	sprintf((char *)buff_tosen, "ATP?\n");

	HAL_Delay(1100);
	HAL_UART_Transmit(&huart4,buff_tosen, 8, 1000);
	//HAL_UART_Receive(&huart4, AT_active, 8, 1000);
	HAL_Delay(1000);
	//clearArray(RxData, 32);
	//sprintf((char *)buff_tosend2, "ATP?\n");
	//HAL_UART_Transmit(&huart4,buff_tosend2, 8, 1000);
	//HAL_UART_Receive(&huart4, AT_active, 8, 1000);

}

uint8_t readHoldingRegs (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
	uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested

	TxData[0] = SLAVE_ID;  // slave ID
	TxData[1] = RxData[1];  // function code
	TxData[2] = numRegs*2;  // Byte count
	int indx = 3;
	TxData[indx++] = 0;
	TxData[indx++] = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14))&0xFF;
	startAddr++;
	TxData[indx++] = 0;
	TxData[indx++] = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14))&0xFF;
	startAddr++;
	for (int i=0; i<(numRegs-2); i++)
	{


		TxData[indx++] = (Holding_Registers_Database[startAddr]>>8)&0xFF;
		TxData[indx++] = (Holding_Registers_Database[startAddr])&0xFF;
		startAddr++;




		}
	//}
		//if(indx == 7){
		//	TxData[indx++] = 0;
		//	TxData[indx++] = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14))&0xFF;
		//	startAddr++;
		//}

		uint16_t crc = crc16(TxData, indx);
		TxData[indx] = crc&0xFF;   // CRC LOW
		TxData[indx+1] = (crc>>8)&0xFF;  // CRC HIGH
		bytes = indx;

	return 1;
}

uint8_t writeSingleReg (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);

	Holding_Registers_Database[startAddr] = (RxData[4]<<8)|RxData[5];
	if(RxData[3] && ((RxData[4]<<8|RxData[5]) != 0)){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_SET);// dioda 1 dodaj warunek

	}
	if(RxData[3] && ((RxData[4]<<8|RxData[5]) == 0)){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET);// dioda 1 dodaj warunek

		}

	TxData[0] = SLAVE_ID;    // slave ID
	TxData[1] = RxData[1];   // function code
	TxData[2] = RxData[2];   // Start Addr HIGH Byte
	TxData[3] = RxData[3];   // Start Addr LOW Byte
	TxData[4] = RxData[4];   // Reg Data HIGH Byte
	TxData[5] = RxData[5];   // Reg Data LOW  Byte

	uint16_t crc = crc16(TxData, 6);
	TxData[6] = crc&0xFF;   // CRC LOW
	TxData[7] = (crc>>8)&0xFF;  // CRC HIGH

	return 1;
}

void sendData (uint8_t *data, int size)
{
	HAL_UART_Transmit(&huart4, data, size, 1000);
}


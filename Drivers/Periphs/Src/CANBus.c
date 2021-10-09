#include "CANBus.h"
#include "stm32f4xx_hal_can.h" 
#include <stdio.h> 

uint32_t TxMailbox;											
static CAN_HandleTypeDef *hcan1;							//header CAN used throughout the file. 
CAN_TxHeaderTypeDef pHeader;    							//header for message transmissions used throughout the file. 
uint32_t receive_number = 0; 

static void floatTo4Bytes(float val, uint8_t bytes_array[4]);

void CANBus_Init() {
	hcan1 = initializeHCAN(); 								//initialize hcan1 fields first 
	HAL_CAN_MspInit(hcan1); 								//initialization functions 
	HAL_CAN_Init(hcan1); 									
	/*														going to forego filtering, want to receive all messages*/ 
    HAL_CAN_Start(hcan1);
    HAL_CAN_ActivateNotification(hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	pHeader.DLC=5; 											//give message size of 1 byte
	pHeader.IDE=CAN_ID_STD; 								//set identifier to standard
	pHeader.RTR=CAN_RTR_DATA; 								//set data type to remote transmission request?
}

CAN_HandleTypeDef* initializeHCAN() { 
  CAN_HandleTypeDef hcan; 
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 45;
  hcan.Init.Mode = CAN_MODE_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  hcan.state = HAL_CAN_STATE_READY; 
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  return &hcan; 
}

void CANBus_Read() {
	printf("hello");															//testing for UART
	uint32_t RxFifoLevel = 0; 
	while (RxFifoLevel == 0) { 													//while there are no messages 
		uint32_t RxFifoLevel = HAL_CAN_GetRxFifoFillLevel(hcan1, CAN_RX_FIFO0); // get the fifo level and make sure we can add it in 
	}
	//finally, read the message (call it). 

}

void CANBus_Send(CANId_t id, CANPayload_t payload){
    uint8_t txdata[5];
	pHeader.StdId=id; //define a standard identifier, used for message identification by filters (switch this for the other microcontroller)
    switch (id) {
		case TRIP:
            HAL_CAN_AddTxMessage(hcan1, &pHeader, &payload.data.b, &TxMailbox);  //function to add message for transmition

		case ALL_CLEAR:
            HAL_CAN_AddTxMessage(hcan1, &pHeader, &payload.data.b, &TxMailbox);  //function to add message for transmition

		case CONTACTOR_STATE:
            HAL_CAN_AddTxMessage(hcan1, &pHeader, &payload.data.b, &TxMailbox);  //function to add message for transmition

		case CURRENT_DATA:
			floatTo4Bytes(payload.data.f, &txdata[0]);
            HAL_CAN_AddTxMessage(hcan1, &pHeader, txdata, &TxMailbox);  //function to add message for transmition

		case VOLT_DATA:
		case TEMP_DATA:
			txdata[0] = payload.idx;
			floatTo4Bytes(payload.data.f, &txdata[1]);
            HAL_CAN_AddTxMessage(hcan1, &pHeader, txdata, &TxMailbox);  //function to add message for transmition

		case SOC_DATA:
			floatTo4Bytes(payload.data.f, &txdata[0]);
            HAL_CAN_AddTxMessage(hcan1, &pHeader, txdata, &TxMailbox);  //function to add message for transmition

		case WDOG_TRIGGERED:
            HAL_CAN_AddTxMessage(hcan1, &pHeader, &payload.data.b, &TxMailbox);  //function to add message for transmition

		case CAN_ERROR:
            HAL_CAN_AddTxMessage(hcan1, &pHeader, &payload.data.b, &TxMailbox);  //function to add message for transmition
	}
}


static void floatTo4Bytes(float val, uint8_t bytes_array[4]) {
	uint8_t temp;
	// Create union of shared memory space
	union {
			float float_variable;
			uint8_t temp_array[4]; 
	} u;
	// Overite bytes of union with float variable
	u.float_variable = val;
	// Assign bytes to input array
	memcpy(bytes_array, u.temp_array, 4); //THIS CODE IS USELESS BECAUSE TEMP ARRAY HASNT BEEN INITIALIZED!!!! what is the point of this??? 
	temp = bytes_array[3];
	bytes_array[3] = bytes_array[0];
	bytes_array[0] = temp;
	temp = bytes_array[2];
	bytes_array[2] = bytes_array[1];
	bytes_array[1] = temp;
} 



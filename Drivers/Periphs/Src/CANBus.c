#include "CANBus.h"

uint32_t TxMailbox;
CAN_HandleTypeDef *hcan1;
CAN_TxHeaderTypeDef pHeader;    //header for message transmissions

static void floatTo4Bytes(uint8_t f, uint8_t bytes[4]);

void CANBus_Init(CAN_HandleTypeDef *hcan){
    HAL_CAN_Start(hcan);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	hcan1 = hcan; //make global variable so it can be used elsewhere
	pHeader.DLC=5; //give message size of 1 byte
	pHeader.IDE=CAN_ID_STD; //set identifier to standard
	pHeader.RTR=CAN_RTR_DATA; //set data type to remote transmission request?
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

void CANBus_Read(uint8_t id, uint8_t *data){
    if(RxFlag){
        //read RxHeader object
        id = pRxHeader.StdId; //YOU SHOULD INITIALIZE THIS SOMEWHERE
        data = RxData;
    }
    RxFlag = 0;
}

static void floatTo4Bytes(uint8_t val, uint8_t bytes_array[4]) {
	uint8_t temp;
	// Create union of shared memory space
	union {
			float float_variable;
			uint8_t temp_array[4];
	} u;
	// Overite bytes of union with float variable
	u.float_variable = val;
	// Assign bytes to input array
	memcpy(bytes_array, u.temp_array, 4);
	temp = bytes_array[3];
	bytes_array[3] = bytes_array[0];
	bytes_array[0] = temp;
	temp = bytes_array[2];
	bytes_array[2] = bytes_array[1];
	bytes_array[1] = temp;	
}

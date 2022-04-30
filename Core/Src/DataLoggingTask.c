#include "Tasks.h"
#include "SDCard.h"

#define MOUNTCYCLES 500

void DataLoggingTask(void* argument){
    if (SDCard_Init() != FR_OK); //TODO: ERROR CHECKING HERE
    int cntr = 0;

    while (1){
        if (SDCard_Sort_Write_Data() == FR_OK) cntr++; //increment counter if data was written
        if (cntr > MOUNTCYCLES) {
            if (SDCard_CloseFileSystem() != FR_OK); //TODO: ERROR CHECKING HERE
            cntr = 0;
            if (SDCard_OpenFileSystem() != FR_OK); //TODO: ERROR CHECKING HERE)
        }
    }
}

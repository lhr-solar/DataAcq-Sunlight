*******
Drivers
=======
*******

SD Card
=======
Purpose
    The purpose of this driver is to write/read files on an SD Card.

Usage
    First ``SDCard_Init()`` must be called before any other SD Card driver functions are called. 
    In order to write to or read a file, ``SDCard_OpenFileSystem()`` must be called. 
    The user can write to a specific file using ``SDCard_Write``, and can read from a file using ``SDCard_Read``.
    For information regarding the SD card's total drive space and available space, use ``SDCard_GetStatistics()``.

    When all operations with the SD Card are finished, ``SDCard_CloseFileSystem()`` must be called.

Additional Considerations
    SD Card functions that print information do so over ``UART``.
    If ``SDCard_CloseFileSystem()`` isn't called, then errors might occur.
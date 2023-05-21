/********************************************************************************
* Filename    : hex_file_reader.h

* Author      : Parth Bhatt

* Date        : May 19, 2023

* Description : The module to extract the data from hex files and fill it up in a
                dummy memory space. It supports reading hex files with non
                contiguous memory. The data is saved in chunks in HexMemory_t 
                datatype. If the memory in the hex file is contiguous, the 
                function will only return one a struct with single chunk.

* Note        : Current version can only read one hex file at a time. The hex data
                will be erased if the second hex file is read. -- May 21, 2023

********************************************************************************/

#ifndef __HEX_FILE_READER_H__
#define __HEX_FILE_READER_H__

/*******************************************************************************
* Include Files
********************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
* Constants and macros
********************************************************************************/
#define MAX_NUM_MEMORY_CHUNKS   10

/*******************************************************************************
* Data types
********************************************************************************/
typedef struct {
    uint32_t address;
    uint16_t size;
    uint8_t* location;
} MemoryChunk_t;

typedef struct
{
    MemoryChunk_t chunks[MAX_NUM_MEMORY_CHUNKS];
    uint16_t memChunksInHexFile;
} HexMemory_t;

typedef enum
{
    HEX_READER_STATUS_FAILED_TO_OPEN_FILE,
    HEX_READER_STATUS_FAILED_TO_LOAD_DATA,
    HEX_READER_STATUS_MEM_BUFFER_OVERFLOW,
    HEX_READER_STATUS_FINISHED_READING_FILE
} HexReaderStatus_t;

/*******************************************************************************
* Global Variables
********************************************************************************/

/*******************************************************************************
* Functions
********************************************************************************/

HexReaderStatus_t GetHexMemory(char* file_in, HexMemory_t* mem_out);

#endif
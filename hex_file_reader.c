/********************************************************************************
* Filename    : hex_file_reader.c

* Author      : Parth Bhatt

* Date        : May 19, 2023

* Description : 

********************************************************************************/

/*******************************************************************************
* Include Files
********************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hex_file_reader.h"

/*******************************************************************************
* Constants and macros
********************************************************************************/
#define DISPLAY_DEBUG_CODE      0

#define MAX_MEMORY_BYTES        8192
#define MAX_BYTES_PER_RECORD    32

#define START_CODE_POS          0
#define START_CODE_LEN          1

#define BYTE_COUNT_POS          (START_CODE_POS + START_CODE_LEN)
#define BYTE_COUNT_LEN          2

#define ADDRESS_POS             (BYTE_COUNT_POS + BYTE_COUNT_LEN)
#define ADDRESS_LEN             4

#define RECORD_TYPE_POS         (ADDRESS_POS + ADDRESS_LEN)
#define RECORD_TYPE_LEN         2

#define DATA_POS                (RECORD_TYPE_POS + RECORD_TYPE_LEN)
#define DATA_LEN_PER_BYTE       2

#define CHECKSUM_POS(x)         (x - CHECKSUM_LEN - 1)
#define CHECKSUM_LEN            2

#define DATA_LEN(x)             ((x - (DATA_POS + CHECKSUM_LEN + 1)) / 2)

/*******************************************************************************
* Data types (Structs, enum)
********************************************************************************/

typedef enum
{
    RECORD_TYPE_DATA                = 0,
    RECORD_TYPE_EOF                 = 1,
    RECORD_TYPE_EXT_SEG_ADD         = 2,
    RECORD_TYPE_START_SEG_ADD       = 3,
    RECORD_TYPE_EXT_LINEAR_ADD      = 4,
    RECORD_TYPE_START_LINEAR_ADD    = 5,
} RecordType_t;

typedef struct
{
    uint8_t         byteCount;
    uint16_t        address;
    RecordType_t    recordType;
    uint8_t         data[MAX_BYTES_PER_RECORD];
    uint8_t         checksum;
    uint8_t         datalen;
} HexRecord_t;

/*******************************************************************************
* Static Variables
********************************************************************************/
//#error "Add limit to avoid overflow in all places"
static uint8_t memory[MAX_MEMORY_BYTES];
static uint8_t* memPtr;
static uint16_t memCounter = 0;

/*******************************************************************************
* Static Functions Declaration
********************************************************************************/

static void LoadMemoryData(HexRecord_t* record, HexMemory_t* mem);
static HexRecord_t ParseRecord(char* line);
static uint32_t GetNum(char* line, uint16_t startLoc, uint16_t len);
static void DisplayRecord(HexRecord_t record);

/*******************************************************************************
* Public Functions
********************************************************************************/
bool GetHexMemory(char* file_in, HexMemory_t* mem_out)
{
    FILE* hexFile = fopen(file_in, "r");
    memPtr = &memory[0];
    memset(mem_out, 0, sizeof(HexMemory_t));

    if (hexFile == NULL)
    {
        return false;
    }
    
    char line[1024];
    uint8_t i = 0;
    while (fgets(line, sizeof(line), hexFile) != NULL)
    {
        HexRecord_t record = ParseRecord(line);

        #if(DISPLAY_DEBUG_CODE==1)
        DisplayRecord(record);
        #endif

        LoadMemoryData(&record, mem_out);
        if(i>3)
        {
            break;
        }
        i++;
    }
    fclose(hexFile);
    
    return true;
}

/*******************************************************************************
* Static Functions
********************************************************************************/

static void LoadMemoryData(HexRecord_t* record, HexMemory_t* mem)
{
    if( record->recordType == RECORD_TYPE_EXT_LINEAR_ADD )
    {
        if(mem->memChunksInHexFile != 0)
        {
            mem->chunks[mem->memChunksInHexFile-1].size = memCounter-1;
            memCounter = 0;
        }
        mem->chunks[mem->memChunksInHexFile].address = 0;
        mem->chunks[mem->memChunksInHexFile].location = memPtr;

        int8_t idx = 0;        
        while(idx < record->datalen)
        {
            mem->chunks[mem->memChunksInHexFile].address <<= 8;
            mem->chunks[mem->memChunksInHexFile].address |=  record->data[idx];
            idx++;
        }
        mem->memChunksInHexFile++;
    }
    else if ( record->recordType == RECORD_TYPE_DATA )
    {
        for(uint8_t i = 0; i < record->datalen; i++)
        {
            if(memCounter >= MAX_MEMORY_BYTES)
            {
                break;
            }
            *memPtr = record->data[i];
            memPtr++;
            memCounter++;
        }
    }
}

static HexRecord_t ParseRecord(char* line)
{
    uint16_t lineLen = strlen(line);
    HexRecord_t record;
    
    record.byteCount = (uint8_t) GetNum(line, BYTE_COUNT_POS, BYTE_COUNT_LEN);
    record.address = (uint8_t) GetNum(line, ADDRESS_POS, ADDRESS_LEN);
    record.recordType = (uint8_t) GetNum(line, RECORD_TYPE_POS, RECORD_TYPE_LEN);
    
    uint16_t dataIdx = DATA_POS;
    record.datalen = DATA_LEN(lineLen);
    for(uint16_t i = 0; i < record.datalen; i++)
    {
        record.data[i] = GetNum(line, dataIdx, DATA_LEN_PER_BYTE);
        dataIdx += DATA_LEN_PER_BYTE;
    }

    record.checksum = GetNum(line, CHECKSUM_POS(lineLen), CHECKSUM_LEN);

    return record;
}

static uint32_t GetNum(char* line, uint16_t startLoc, uint16_t len)
{
    char tempStr[32];
    memset(tempStr, 0, 32);
    memcpy(&tempStr[0], &line[startLoc], len);

    return (uint32_t)strtol(tempStr, NULL, 16);
}

static void DisplayRecord(HexRecord_t record)
{
    static bool printTitle = true;
    if(printTitle)
    {
        printf("ByteCnt\tAddress\tRecType\tChcksm\tDatalen\tData\n");
        printTitle = false;
    }
    printf("0x%x\t0x%x\t0x%x\t0x%x\t%d\t", 
            record.byteCount,
            record.address,
            record.recordType,
            record.checksum,
            record.datalen);
        
    for(uint16_t i = 0; i < record.datalen; i++)
    {
        printf("0x%x ", record.data[i]);
    }
    printf("\n");
}

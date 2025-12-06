#include "headers/blockchain_helpers.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "headers/sha256.h"
#include <limits.h>
#include "headers/blockchain_core.h"
#include "headers/wallet_service.h"
#include "headers/mining_utils.h"

void CalculateHash(char* data, char* outputBuffer) {
    sha256_easy_hash_hex(data, strlen(data), outputBuffer);
    outputBuffer[64] = '\0';
}

void RawDataToHash(struct Block* block, char* data, int maxSizeInBytes) {
    data[0] = '\0';
    int id = block->id;
    int creationTime = block->creationTime;
    char headerPart[1000];
    if (sprintf_s(headerPart, sizeof(headerPart), "%d%d%s", id, creationTime, block->previousHash) < 0)
        return;

    strcat_s(data, maxSizeInBytes, headerPart);

    for (int i=0; i<block->transactionCount; i++) {
        char temp[200] = "";
        if (sprintf_s(temp,sizeof(temp), "%d%s%s", block->transactions[i].amount, block->transactions[i].receiver, block->transactions[i].sender)<0)
            return;
        if (strcat_s(data,maxSizeInBytes,temp)!=0)
            return;
    }
}

void MineBlock(struct Blockchain* blockchain) {
    struct Block* block = blockchain->tail;
    char staticData[2000];
    RawDataToHash(block, staticData, sizeof(staticData));

    int nonce = 0;
    char currentHash[65];
    char dataToHash[2100];

    printf("\n[MINING] Started mining block. ID: %d\n",block->id);
    while (nonce < INT_MAX) {

        sprintf(dataToHash, "%s%d", staticData, nonce);
        CalculateHash(dataToHash, currentHash);

        if (nonce%100000 == 0) {
            printf("\r[MINING] Mining in progress... current nonce: %d , hash: %65s",nonce, currentHash);
            fflush(stdout);
        }

        if (IsHashValid(currentHash)) {
            printf("\n[MINING] Block mined! Nonce: %d Hash: %s\n", nonce, currentHash);

            block->nonce = nonce;
            strcpy(block->Hash, currentHash);
            break;
        }

        nonce++;
    }
}

bool IsHashValid(char* hash) {
    for (int i = 0; i < DIFFICULTY_ZEROS; i++) {
        if (hash[i] != '0') return false;
    }
    return true;
}
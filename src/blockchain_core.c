#include "headers/blockchain_core.h"
#include "headers/blockchain_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "headers/wallet_service.h"
#include "headers/mining_utils.h"

void addTransactionToChain(struct Transaction* transaction, struct Blockchain* blockchain) {
    int counter = blockchain->tail->transactionCount;
    if (counter>=10) {
        if (!TryFinalizeBlock(blockchain))
        {  printf("[ERROR] Block finalization failed."); return; }
        counter = 0;
    }
    blockchain->tail->transactions[counter] = *transaction;
    blockchain->tail->transactionCount++;
}
int GetChainLength(struct Blockchain* blockchain) {
    int count=0;
    struct Block* currentBlock = blockchain->head;
    while (currentBlock!=NULL) {
        count++;
        currentBlock = currentBlock->next;
    }
    return count;
}
void createBlock(int id, char* previousHash, struct Blockchain* blockchain) {
    struct Block* block = malloc(sizeof(struct Block));
    if (block == NULL) return;

    block->id = id;
    block->transactionCount = 0;
    block->creationTime = (int)time(NULL);
    if (blockchain->head==NULL || blockchain->tail==NULL) {
        //to pierwszy blok
        blockchain->head = blockchain->tail = block;
        strcpy(block->previousHash, "0000000000000000000000000000000000000000000000000000000000000000");

        char data[2000] = "";
        block->nonce = 0; // Genesis zazwyczaj ma nonce 0
        RawDataToHash(block, data, 2000);

        char dataWithNonce[2100];
        sprintf(dataWithNonce, "%s%d", data, 0);

        CalculateHash(dataWithNonce, block->Hash);
        block->next = NULL;
    }
    else {
        strcpy(block->previousHash, previousHash);
        blockchain->tail->next = block;
        blockchain->tail = block;
        strcpy(block->Hash, "");
        block->next = NULL;
    }
}
void FinalizeTail(struct Blockchain *blockchain, char* rewardAddress) {
    struct Block* currentTail = blockchain->tail;
    int transactionsToFinalize = 10 - blockchain->tail->transactionCount;
    for (int i = 0; i < transactionsToFinalize; i++) {
        struct Transaction nullTransaction = CreateTransaction(0, "CONSOLE", "CONSOLE");
        addTransactionToChain(&nullTransaction, blockchain);
    }
    if (TryFinalizeBlock(blockchain)) {
        char address[50] = "";
        strcpy(address, rewardAddress);
        struct Transaction newTransaction = CreateTransaction(50, address, "CONSOLE");
        printf("\nReward: 50$");
        addTransactionToChain(&newTransaction, blockchain);
    }
}

bool TryFinalizeBlock(struct Blockchain *blockchain) {
    MineBlock(blockchain);
    createBlock(blockchain->tail->id+1, blockchain->tail->Hash, blockchain);

    return true;
}
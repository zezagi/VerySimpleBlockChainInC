#pragma once
#include <stdbool.h>

struct Transaction  {
    int amount;
    char receiver[50];
    char sender[50];
};

struct Block {
    int id;
    int creationTime;
    struct Transaction transactions[10];
    int transactionCount;
    int nonce;
    char previousHash[65];
    char Hash[65];
    struct Block *next;
};

struct Blockchain {
    struct Block *head;
    struct Block *tail;
};

void addTransactionToChain(struct Transaction* transaction, struct Blockchain* blockchain);
void createBlock(int id, char* previousHash, struct Blockchain* blockchain);
int GetChainLength(struct Blockchain* blockchain);
bool TryFinalizeBlock(struct Blockchain *blockchain);
void FinalizeTail(struct Blockchain* blockchain, char* rewardAddress);
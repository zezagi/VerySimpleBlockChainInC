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
    unsigned long previousHash;
    unsigned long Hash;
    struct Block *next;
};

struct Blockchain {
    struct Block *head;
    struct Block *tail;
};
struct walletInfo {
    int money;
    char address[50];
};

struct Transaction CreateTransaction(int amount, char receiver[50], char sender[50]);
void addTransactionToChain(struct Transaction* transaction, struct Blockchain* blockchain);
void createBlock(int id, unsigned long previousHash, struct Blockchain* blockchain);
unsigned long CalculateHash(char* data);
bool TryGetWalletInfo(struct walletInfo* walletInfo, struct Blockchain blockchain);
bool TryFinalizeBlock(struct Blockchain *blockchain);
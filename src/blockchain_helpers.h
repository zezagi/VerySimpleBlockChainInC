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
struct walletInfo {
    int money;
    char address[50];
};

struct Transaction CreateTransaction(int amount, char receiver[50], char sender[50]);
void addTransactionToChain(struct Transaction* transaction, struct Blockchain* blockchain);
void createBlock(int id, char* previousHash, struct Blockchain* blockchain);
void CalculateHash(char* data, char* outputBuffer);
bool TryGetWalletInfo(struct walletInfo* walletInfo, struct Blockchain blockchain);
bool TryFinalizeBlock(struct Blockchain *blockchain);
int BlocksInBlockchain(struct Blockchain* blockchain);
bool TryLoadBlockchain(struct Blockchain* blockchainToLoad);
bool TrySaveBlockchain(struct Blockchain* blockchainToSave);
void FinalizeTail(struct Blockchain* blockchain);
void DebugTransaction(struct Blockchain* blockchain, char receiver[50]);
void RawDataToHash(struct Block* block, char* data, int maxSizeInBytes);
void MineBlock(struct Blockchain* blockchain);
#include "blockchain_helpers.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

void DebugTransaction(struct Blockchain* blockchain, char receiver[50]) {
    struct Transaction transaction = CreateTransaction(500, receiver, "CONSOLE");
    addTransactionToChain(&transaction, blockchain);
}
void FinalizeTail(struct Blockchain *blockchain) {
    struct Block* currentTail = blockchain->tail;
    int transactionsToFinalize = 10 - blockchain->tail->transactionCount;
    for (int i = 0; i < transactionsToFinalize; i++) {
        struct Transaction nullTransaction = CreateTransaction(0, "CONSOLE", "CONSOLE");
        addTransactionToChain(&nullTransaction, blockchain);
    }
    TryFinalizeBlock(blockchain);
}

bool TrySaveBlockchain(struct Blockchain* blockchainToSave) {
    FILE *file = fopen("blockchain.txt", "wb");
    struct Block* temp = blockchainToSave->head;
    if (file == NULL) return false;
    while (temp!=NULL) {
        if (!fwrite(temp, sizeof(struct Block), 1, file))
            return false;
        temp = temp->next;
    }
    fclose(file);
    return true;
}

bool TryLoadBlockchain(struct Blockchain* blockchainToLoad) {
    FILE *file = fopen("blockchain.txt", "rb");
    if (file == NULL) return false;
    while (true) {
        struct Block tempToCheck;
        if (!fread(&tempToCheck, sizeof(struct Block), 1, file)) {
            fclose(file);
            if (feof(file)) return true;
            return false;
        }

        if (tempToCheck.transactionCount < 0 || tempToCheck.transactionCount > 10) {
            printf("[SECURITY] Block ID: %d is not secure.", tempToCheck.id);
            fclose(file);
            return false;
        }

        for (int i = 0; i < tempToCheck.transactionCount; i++) {
            tempToCheck.transactions[i].receiver[49] = '\0';
            tempToCheck.transactions[i].sender[49] = '\0';
        }
        //SECURITY - PODSTAWA CALEGO SENSU ISTNIENIA
        char data[2000] = "";
        RawDataToHash(&tempToCheck, data, sizeof(data));
        unsigned long validHash = CalculateHash(data);

        if (tempToCheck.Hash != validHash) {
            printf("[SECURITY] Block ID: %d is not secure.", tempToCheck.id);
            fclose(file);
            return false;
        }
        if (tempToCheck.id != 0) {
            if (blockchainToLoad->tail!=NULL) {
                if (tempToCheck.previousHash != blockchainToLoad->tail->Hash) {
                    printf("[SECURITY] Block ID: %d is not secure.", tempToCheck.id);
                    fclose(file);
                    return false;
                }

            }
        }

        struct Block* temp = malloc(sizeof(struct Block));
        if (temp==NULL) { fclose(file); return false; }
        *temp = tempToCheck;
        temp->next = NULL;
        if (blockchainToLoad->head == NULL) {
            blockchainToLoad->head = blockchainToLoad->tail = temp;
        }
        else {
            blockchainToLoad->tail->next = temp;
            blockchainToLoad->tail = temp;
        }

    }
}
void RawDataToHash(struct Block* block, char* data, int maxSizeInBytes) {
    data[0] = '\0';
    int id = block->id;
    int creationTime = block->creationTime;
    unsigned long previousHash = block->previousHash;
    if (sprintf_s(data,maxSizeInBytes, "%d%d%lu", id, creationTime, previousHash)<0)
        return;

    for (int i=0; i<block->transactionCount; i++) {
        char temp[200] = "";
        if (sprintf_s(temp,sizeof(temp), "%d%s%s", block->transactions[i].amount, block->transactions[i].receiver, block->transactions[i].sender)<0)
            return;
        if (strcat_s(data,maxSizeInBytes,temp)!=0)
            return;
    }
}
bool TryFinalizeBlock(struct Blockchain *blockchain) {
    char buffer[2000] = "";
    RawDataToHash(blockchain->tail, buffer, sizeof(buffer));
    unsigned long newHash = CalculateHash(buffer);
    blockchain->tail->Hash = newHash;
    createBlock(blockchain->tail->id+1, newHash, blockchain);
    return true;
}
bool TryGetWalletInfo(struct walletInfo* walletInfo, struct Blockchain blockchain) {
    struct Block* currentChecking=blockchain.head;
    bool foundAccount=false;
    while (currentChecking!=NULL) {
        if (currentChecking->Hash==0){ currentChecking = currentChecking->next; continue; }
        for (int i=0; i<currentChecking->transactionCount; i++) {
            //kazda transakcje sprawdzam
            if (strcmp(walletInfo->address, currentChecking->transactions[i].sender)==0) {
                walletInfo->money -= currentChecking->transactions[i].amount;
                foundAccount = true;
            }
            else if (strcmp(walletInfo->address, currentChecking->transactions[i].receiver)==0) {
                walletInfo->money += currentChecking->transactions[i].amount;
                foundAccount = true;
            }
        }
        currentChecking = currentChecking->next;
    }
    return foundAccount;
}
unsigned long CalculateHash(char* data) {
    unsigned long wynik = 0;
    for (int i=0; data[i]!='\0'; i++) {
        wynik+=data[i] + wynik*13;
    }
    return wynik;
}
struct Transaction CreateTransaction(int amount, char receiver[50], char sender[50]) {
    struct Transaction transaction;
    transaction.amount = amount;
    strcpy(transaction.receiver, receiver);
    strcpy(transaction.sender, sender);
    return transaction;
}
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

int BlocksInBlockchain(struct Blockchain* blockchain) {
    int count=0;
    struct Block* currentBlock = blockchain->head;
    while (currentBlock!=NULL) {
        count++;
        currentBlock = currentBlock->next;
    }
    return count;
}

void createBlock(int id, unsigned long previousHash, struct Blockchain* blockchain) {
    struct Block* block = malloc(sizeof(struct Block));
    block->id = id;
    block->transactionCount = 0;
    block->creationTime = (int)time(NULL);
    if (blockchain->head==NULL || blockchain->tail==NULL) {
        //to pierwszy blok
        blockchain->head = blockchain->tail = block;
        char data[2000] ="";
        RawDataToHash(block, data, 2000);
        long unsigned hash = CalculateHash(data);
        block->Hash = hash;
        block->previousHash=0;
        block->next = NULL;
    }
    else {
        block->previousHash = blockchain->tail->Hash;
        blockchain->tail->next = block;
        blockchain->tail = block;
        block->Hash = 0;
        block->next = NULL;
    }
}
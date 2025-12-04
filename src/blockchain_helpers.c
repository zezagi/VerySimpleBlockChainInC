#include "blockchain_helpers.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "sha256.h"
#include <limits.h>

#define DIFFICULTY_ZEROS 3

bool IsHashValid(char* hash) {
    for (int i = 0; i < DIFFICULTY_ZEROS; i++) {
        if (hash[i] != '0') return false;
    }
    return true;
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

    blockchainToLoad->head = NULL;
    blockchainToLoad->tail = NULL;

    while (true) {
        struct Block tempToCheck;
        size_t readCount = fread(&tempToCheck, sizeof(struct Block), 1, file);

        if (readCount == 0) {
            fclose(file);
            if (feof(file)) return true;
            return false;
        }

        tempToCheck.previousHash[64] = '\0';
        tempToCheck.Hash[64] = '\0';
        for (int i = 0; i < tempToCheck.transactionCount; i++) {
            tempToCheck.transactions[i].receiver[49] = '\0';
            tempToCheck.transactions[i].sender[49] = '\0';
        }

        bool isWorkingBlock = (strcmp(tempToCheck.Hash, "") == 0);


        if (!isWorkingBlock) {
            if (tempToCheck.transactionCount < 0 || tempToCheck.transactionCount > 10) {
                printf("[SECURITY] Block ID: %d corrupted transaction count.\n", tempToCheck.id);
                fclose(file); return false;
            }

            char data[2000] = "";
            RawDataToHash(&tempToCheck, data, sizeof(data));
            char dataWithNonce[2100] = "";
            sprintf(dataWithNonce, "%s%d", data, tempToCheck.nonce);

            char validHash[65];
            CalculateHash(dataWithNonce, validHash);

            // Sprawdzamy Proof of Work (tylko dla wykopanych)
            if (tempToCheck.id != 0 && !IsHashValid(validHash)) {
                printf("[SECURITY] Block ID: %d not mined correctly! Hash in file: %s\n", tempToCheck.id, tempToCheck.Hash);
                fclose(file); return false;
            }

            if (strcmp(tempToCheck.Hash, validHash) != 0) {
                printf("[SECURITY] Block ID: %d Hash mismatch!\n", tempToCheck.id);
                fclose(file); return false;
            }
        }

        // Sprawdzanie łańcucha (Previous Hash)
        if (tempToCheck.id != 0 && blockchainToLoad->tail != NULL) {
            if (strcmp(tempToCheck.previousHash, blockchainToLoad->tail->Hash) != 0) {
                printf("[SECURITY] Block ID: %d Broken Chain!\n", tempToCheck.id);
                fclose(file); return false;
            }
        }

        struct Block* temp = malloc(sizeof(struct Block));
        if (temp == NULL) { fclose(file); return false; }
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
bool TryFinalizeBlock(struct Blockchain *blockchain) {
    MineBlock(blockchain);
    createBlock(blockchain->tail->id+1, blockchain->tail->Hash, blockchain);
    return true;
}
bool TryGetWalletInfo(struct walletInfo* walletInfo, struct Blockchain blockchain) {
    struct Block* currentChecking=blockchain.head;
    bool foundAccount=false;
    while (currentChecking!=NULL) {
        if (strcmp(currentChecking->Hash, "")==0){ currentChecking = currentChecking->next; continue; }
        for (int i=0; i<currentChecking->transactionCount; i++) {
            //kazda transakcje sprawdzam
            if (strcmp(walletInfo->address, currentChecking->transactions[i].sender)==0) {
                walletInfo->money -= currentChecking->transactions[i].amount;
                foundAccount = true;
            }
            if (strcmp(walletInfo->address, currentChecking->transactions[i].receiver)==0) {
                walletInfo->money += currentChecking->transactions[i].amount;
                foundAccount = true;
            }
        }
        currentChecking = currentChecking->next;
    }
    return foundAccount;
}
void CalculateHash(char* data, char* outputBuffer) {
    sha256_easy_hash_hex(data, strlen(data), outputBuffer);
    outputBuffer[64] = '\0';
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
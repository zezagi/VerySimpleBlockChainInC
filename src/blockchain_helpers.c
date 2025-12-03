#include "blockchain_helpers.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
bool TryFinalizeBlock(struct Blockchain *blockchain) {
    char buffer[2000] = "";
    int id = blockchain->tail->id;
    int creationTime = blockchain->tail->creationTime;
    unsigned long previousHash = blockchain->tail->previousHash;
    sprintf(buffer, "%d%d%lu", id, creationTime, previousHash);

    for (int i=0; i<blockchain->tail->transactionCount; i++) {
        char temp[200] = "";
        sprintf(temp, "%d%s%s", blockchain->tail->transactions[i].amount, blockchain->tail->transactions[i].receiver, blockchain->tail->transactions[i].sender);
        strcat(buffer,temp);
    }

    unsigned long newHash = CalculateHash(buffer);
    blockchain->tail->Hash = newHash;
    createBlock(blockchain->tail->id+1, newHash, blockchain);
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
        if (!TryFinalizeBlock(&blockchain))
        {  printf("[ERROR] Block finalization failed."); return; }
    }
    blockchain->tail->transactions[counter] = *transaction;
    blockchain->tail->transactionCount++;
}

void createBlock(int id, unsigned long previousHash, struct Blockchain* blockchain) {
    struct Block* block = malloc(sizeof(struct Block));
    block->id = id;
    block->transactionCount = 0;
    block->creationTime = (int)time(NULL);
    if (blockchain->head==NULL || blockchain->tail==NULL) {
        //to pierwszy blok
        blockchain->head = blockchain->tail = block;
        block->Hash=0;
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
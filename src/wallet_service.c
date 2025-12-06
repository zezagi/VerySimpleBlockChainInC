#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/blockchain_core.h"
#include "headers/wallet_service.h"
#include <stdbool.h>

struct Transaction CreateTransaction(int amount, char receiver[50], char sender[50]) {
    struct Transaction transaction;
    transaction.amount = amount;
    strcpy(transaction.receiver, receiver);
    strcpy(transaction.sender, sender);
    return transaction;
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
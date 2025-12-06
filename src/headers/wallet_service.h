#pragma once
#include <stdbool.h>

struct walletInfo {
    int money;
    char address[50];
};

struct Transaction CreateTransaction(int amount, char receiver[50], char sender[50]);
bool TryGetWalletInfo(struct walletInfo* walletInfo, struct Blockchain blockchain);
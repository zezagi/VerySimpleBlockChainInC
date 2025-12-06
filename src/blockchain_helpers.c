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


void DebugTransaction(struct Blockchain* blockchain, char receiver[50]) {
    struct Transaction transaction = CreateTransaction(500, receiver, "CONSOLE");
    addTransactionToChain(&transaction, blockchain);
}


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
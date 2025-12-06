#pragma once
#include "blockchain_core.h"
#include <stdbool.h>

#define DIFFICULTY_ZEROS 3

void RawDataToHash(struct Block* block, char* data, int maxSizeInBytes);
void MineBlock(struct Blockchain* blockchain);
void CalculateHash(char* data, char* outputBuffer);
bool IsHashValid(char* hash);
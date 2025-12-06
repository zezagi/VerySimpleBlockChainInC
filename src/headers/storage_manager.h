#pragma once
#include "blockchain_helpers.h"

bool TryLoadBlockchain(struct Blockchain* blockchainToLoad);
bool TrySaveBlockchain(struct Blockchain* blockchainToSave);
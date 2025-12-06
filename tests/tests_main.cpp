#include <gtest/gtest.h>

// Trik łączenia C z C++
extern "C" {
#include "../src/headers/blockchain_helpers.h"
}

TEST(BlockchainLogika, TworzenieTransakcji) {
    // Arrange (Przygotowanie)
    struct Transaction t;
    char sender[] = "Janusz";
    char receiver[] = "Grazyna";
    int kwota = 500;

    // Act (Działanie - tu używasz swojej logiki, np. konstruktora jeśli masz)
    // Symulujemy ręczne wypełnienie, albo użyj createTransaction() jeśli napisałeś
    t.amount = kwota;
    strcpy(t.sender, sender);
    strcpy(t.receiver, receiver);

    // Assert (Sprawdzenie)
    EXPECT_EQ(t.amount, 500);
    EXPECT_STREQ(t.sender, "Janusz"); // STREQ służy do porównywania napisów (String Equal)
    EXPECT_STREQ(t.receiver, "Grazyna");
}
TEST(BlockchainLogika, TworzenieBloku) {
    // Arrange
    struct Blockchain chain;
    chain.head = NULL;
    chain.tail = NULL;

    // Act
    createBlock(1, 0, &chain);

    // Assert
    ASSERT_NE(chain.head, nullptr); // Sprawdź czy head NIE jest nullem
    EXPECT_EQ(chain.head->id, 1);
    EXPECT_EQ(chain.head->previousHash, 0);
}
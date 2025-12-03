#include <stdio.h>
#include "blockchain_helpers.h"
#include <stdbool.h>
#include <string.h>

enum MenuStage {MainMenu, AccountDashboard};
struct walletInfo walletInfo;

int main(void) {
    struct Blockchain blockchain;
    blockchain.head = NULL;
    blockchain.tail = NULL;
    createBlock(0, 0, &blockchain);

    enum MenuStage menuStage=MainMenu;
    int mainMenuSelection;

    char receiverID[50];
    int transactionMoney;
    while (true) {
        switch (menuStage) {
            case MainMenu:
                printf("\n=========== [ MAIN MENU ] ===========\n");
                printf("1) - Open wallet\n");
                printf("2) - EXIT\n");
                printf("> ");
                scanf(" %d", &mainMenuSelection);

                switch (mainMenuSelection) {
                    case 1:
                        printf("\nEnter wallet ID: ");
                        printf("> ");
                        if (scanf(" %50s", &walletInfo.address)) {
                            walletInfo.money = 0;
                            if (!TryGetWalletInfo(&walletInfo, blockchain)) {
                                printf("\nSorry, this wallet doesn't exist!");
                            }
                            else {
                                menuStage = AccountDashboard;
                            }
                        }
                        break;
                    case 2:
                        break;
                    default: return 0;
                }

                break;
            case AccountDashboard:
                printf("\n=========== [ YOUR ACCOUNT ] ===========");
                printf("\nYour wallet ID: [ %50s ]", walletInfo.address);
                printf("\nMoney: %d$", walletInfo.money);
                printf("\n=========== [ OPTIONS ] ===========");
                printf("\n1) - Send money");
                printf("\n2) - Refresh");
                printf("\n3) - Log out");
                printf("\n> ");
                scanf_s(" %d", &mainMenuSelection);
                switch (mainMenuSelection) {
                    case 1:
                        printf("\nAvailable money: %d", walletInfo.money);
                        printf("\nEnter reciever's wallet ID: ");
                        printf("\n> ");
                        scanf("%50s", receiverID);
                        printf("\nEnter transaction money: %d", walletInfo.money);
                        scanf_s("%d", &transactionMoney);
                        if (transactionMoney>walletInfo.money) {
                            printf("\n[ERROR] Not enough money!");
                            break;
                        }
                        struct Transaction transaction = CreateTransaction(transactionMoney, receiverID, walletInfo.address);
                        addTransactionToChain(&transaction, &blockchain);
                        printf("Transaction created successfully! Your money: %d", walletInfo.money-transactionMoney);
                        break;
                    case 2:
                        menuStage = AccountDashboard;
                        continue;
                        break;
                    case 3:
                        menuStage = MainMenu;
                        strcpy(walletInfo.address, "00000000000000000000000000000000000000000000000000");
                        walletInfo.money = 0;
                        continue;
                        break;
                }
                break;
        }
    }
    return 0;
}
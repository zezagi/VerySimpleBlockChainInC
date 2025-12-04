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
    if (!TryLoadBlockchain(&blockchain))
        createBlock(0, "", &blockchain);

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
                        if (scanf(" %50s", walletInfo.address)) {
                            walletInfo.money = 0;
                            if (!TryGetWalletInfo(&walletInfo, blockchain)) {
                                printf("\nThis is new wallet!");
                                menuStage = AccountDashboard;
                            }
                            else {
                                menuStage = AccountDashboard;
                            }
                        }
                        break;
                    case 2:
                        return 0;
                        break;
                    default: return 0;
                }

                break;
            case AccountDashboard:
                printf("\n=========== [ YOUR ACCOUNT ] ===========");
                printf("\nYour wallet ID: [ %50s ]", walletInfo.address);
                printf("\nMoney: %d$", walletInfo.money);
                printf("\nBlocks count: %d", BlocksInBlockchain(&blockchain));
                printf("\n=========== [ OPTIONS ] ===========");
                printf("\n1) - Send money");
                printf("\n2) - Refresh");
                printf("\n3) - Log out and save");
                printf("\n4) - Finalize block");
                printf("\n5) - [DEBUG] Add 500$");
                printf("\n> ");
                scanf_s(" %d", &mainMenuSelection);
                switch (mainMenuSelection) {
                    case 1:
                        printf("\nAvailable money: %d", walletInfo.money);
                        printf("\nEnter reciever's wallet ID: ");
                        printf("\n> ");
                        scanf("%50s", receiverID);
                        printf("\nEnter transaction money: ");
                        printf("\n> ");
                        scanf_s("%d", &transactionMoney);
                        if (transactionMoney>walletInfo.money) {
                            printf("\n[ERROR] Not enough money!");
                            break;
                        }
                        struct Transaction transaction = CreateTransaction(transactionMoney, receiverID, walletInfo.address);
                        walletInfo.money-=transactionMoney;
                        addTransactionToChain(&transaction, &blockchain);
                        printf("Transaction created successfully! Your money: %d", walletInfo.money);
                        break;
                    case 2:
                        walletInfo.money=0;
                        TryGetWalletInfo(&walletInfo, blockchain);
                        menuStage = AccountDashboard;
                        continue;
                        break;
                    case 3:
                        menuStage = MainMenu;
                        strcpy(walletInfo.address, "00000000000000000000000000000000000000000000000000");
                        walletInfo.money = 0;
                        TrySaveBlockchain(&blockchain);
                        continue;
                        break;
                    case 4:
                        FinalizeTail(&blockchain);
                        break;
                    case 5:
                        DebugTransaction(&blockchain, walletInfo.address);
                        break;
                }
                break;
        }
    }
    return 0;
}
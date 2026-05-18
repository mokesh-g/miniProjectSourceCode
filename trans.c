#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRANS 10

// structure for transaction history
struct Transaction
{
    char type[20];
    double amount;
};

// client structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;

    struct Transaction history[MAX_TRANS];
    int transCount;
};

// function prototypes
unsigned int enterChoice(void);

void addRecord(FILE *fPtr);
void updateRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void showAccount(FILE *fPtr);
void showHistory(FILE *fPtr);
void textFile(FILE *fPtr);

int main()
{
    FILE *cfPtr;

    // open binary file
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        // create file if not exists
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            return 1;
        }

        // initialize empty records
        struct clientData blank = {0, "", "", 0.0, {}, 0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    unsigned int choice;

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            addRecord(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            deleteRecord(cfPtr);
            break;

        case 4:
            showAccount(cfPtr);
            break;

        case 5:
            showHistory(cfPtr);
            break;

        case 6:
            textFile(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);

    return 0;
}

// menu
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n===== BANK MANAGEMENT SYSTEM =====\n");
    printf("1. Add New Account\n");
    printf("2. Deposit / Withdraw\n");
    printf("3. Delete Account\n");
    printf("4. View Account\n");
    printf("5. View Transaction History\n");
    printf("6. Export Accounts to Text File\n");
    printf("7. Exit\n");

    printf("Enter choice: ");
    scanf("%u", &choice);

    return choice;
}

// add new account
void addRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, {}, 0};

    unsigned int accountNum;

    printf("Enter Account Number (1-100): ");
    scanf("%u", &accountNum);

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter Last Name First Name Balance:\n");

    scanf("%14s%9s%lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = accountNum;

    strcpy(client.history[0].type, "Initial");
    client.history[0].amount = client.balance;
    client.transCount = 1;

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully.\n");
}

// update account
void updateRecord(FILE *fPtr)
{
    struct clientData client;

    unsigned int account;
    double amount;
    int option;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("1. Deposit\n");
    printf("2. Withdraw\n");

    printf("Choose option: ");
    scanf("%d", &option);

    printf("Enter amount: ");
    scanf("%lf", &amount);

    if (option == 1)
    {
        client.balance += amount;

        if (client.transCount < MAX_TRANS)
        {
            strcpy(client.history[client.transCount].type,
                   "Deposit");

            client.history[client.transCount].amount = amount;
            client.transCount++;
        }
    }
    else if (option == 2)
    {
        if (amount > client.balance)
        {
            printf("Insufficient balance.\n");
            return;
        }

        client.balance -= amount;

        if (client.transCount < MAX_TRANS)
        {
            strcpy(client.history[client.transCount].type,
                   "Withdraw");

            client.history[client.transCount].amount = amount;
            client.transCount++;
        }
    }
    else
    {
        printf("Invalid option.\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Transaction successful.\n");
}

// delete account
void deleteRecord(FILE *fPtr)
{
    struct clientData blank = {0, "", "", 0.0, {}, 0};

    unsigned int account;

    printf("Enter account number to delete: ");
    scanf("%u", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted.\n");
}

// show account details
void showAccount(FILE *fPtr)
{
    struct clientData client;

    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\n===== ACCOUNT DETAILS =====\n");

    printf("Account No : %u\n", client.acctNum);
    printf("Name       : %s %s\n",
           client.firstName,
           client.lastName);

    printf("Balance    : %.2lf\n",
           client.balance);
}

// show transaction history
void showHistory(FILE *fPtr)
{
    struct clientData client;

    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\n===== TRANSACTION HISTORY =====\n");

    for (int i = 0; i < client.transCount; i++)
    {
        printf("%d. %-10s %.2lf\n",
               i + 1,
               client.history[i].type,
               client.history[i].amount);
    }
}

// export accounts
void textFile(FILE *fPtr)
{
    FILE *writePtr;

    struct clientData client;

    writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Cannot create text file.\n");
        return;
    }

    rewind(fPtr);

    fprintf(writePtr,
            "%-6s%-15s%-15s%-10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u%-15s%-15s%-10.2lf\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("Accounts exported to accounts.txt\n");
}

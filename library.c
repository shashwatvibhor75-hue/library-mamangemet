#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BOOKS 100
#define MAX_LOANS 200

struct Book
{
    char name[100];
    int id;
    float price;
    char author[100];
    char genre[50];
};

struct Loan
{
    int sap;
    int bookID;
    char bookName[100];
    char borrowDate[11];
};

struct Book books[MAX_BOOKS];
struct Loan loans[MAX_LOANS];
int count = 0, loanCount = 0;

/* Clears extra characters left in the input buffer */
void clearInputBuffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

/* Reads a complete line */
void readLine(char text[], int size)
{
    fgets(text, size, stdin);
    text[strcspn(text, "\n")] = '\0';
}

/* Loads the book catalogue from books.txt */
void loadBooks()
{
    FILE *fp;
    char line[400];

    count = 0;
    fp = fopen("books.txt", "r");

    if (fp == NULL)
    {
        printf("\nError: books.txt not found!\n");
        return;
    }

    while (count < MAX_BOOKS && fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "%99[^|]|%d|%f|%99[^|]|%49[^\n]",
                   books[count].name, &books[count].id,
                   &books[count].price, books[count].author,
                   books[count].genre) == 5)
        {
            count++;
        }
    }
    fclose(fp);
}

/* Saves the current catalogue to books.txt */
void saveBooks()
{
    FILE *fp = fopen("books.txt", "w");
    int i;

    if (fp == NULL)
    {
        printf("\nError saving books.txt!\n");
        return;
    }

    for (i = 0; i < count; i++)
        fprintf(fp, "%s|%d|%.2f|%s|%s\n", books[i].name,
                books[i].id, books[i].price, books[i].author,
                books[i].genre);

    fclose(fp);
}

/* Loads active borrowing records from loans.txt */
void loadLoans()
{
    FILE *fp;
    char line[300];

    loanCount = 0;
    fp = fopen("loans.txt", "r");

    if (fp == NULL)
        return; /* File is created when the first book is borrowed */

    while (loanCount < MAX_LOANS && fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "%d|%d|%99[^|]|%10[^\n]",
                   &loans[loanCount].sap, &loans[loanCount].bookID,
                   loans[loanCount].bookName,
                   loans[loanCount].borrowDate) == 4)
            loanCount++;
    }
    fclose(fp);
}

/* Saves active borrowing records to loans.txt */
void saveLoans()
{
    FILE *fp = fopen("loans.txt", "w");
    int i;

    if (fp == NULL)
    {
        printf("\nError saving loans.txt!\n");
        return;
    }

    for (i = 0; i < loanCount; i++)
        fprintf(fp, "%d|%d|%s|%s\n", loans[i].sap,
                loans[i].bookID, loans[i].bookName,
                loans[i].borrowDate);

    fclose(fp);
}

/* Displays all books */
void displayBooks()
{
    int i;

    if (count == 0)
    {
        printf("\nNo books available.\n");
        return;
    }

    printf("\n==================== BOOK CATALOGUE ====================\n");
    printf("%-25s %-10s %-10s %-25s %-15s\n",
           "Book Name", "Book ID", "Price", "Author", "Genre");
    printf("-------------------------------------------------------------------------------\n");

    for (i = 0; i < count; i++)
        printf("%-25s %-10d Rs.%-7.2f %-25s %-15s\n",
               books[i].name, books[i].id, books[i].price,
               books[i].author, books[i].genre);

    printf("-------------------------------------------------------------------------------\n");
}

/* Finds a book using its ID */
int findBook(int id)
{
    int i;
    for (i = 0; i < count; i++)
        if (books[i].id == id)
            return i;
    return -1;
}

/* Checks whether a book is currently borrowed */
int isBookBorrowed(int id)
{
    int i;
    for (i = 0; i < loanCount; i++)
        if (loans[i].bookID == id)
            return 1;
    return 0;
}

/* Checks whether this student already has this book */
int studentHasBook(int sap, int id)
{
    int i;
    for (i = 0; i < loanCount; i++)
        if (loans[i].sap == sap && loans[i].bookID == id)
            return 1;
    return 0;
}

/* Returns today's date in YYYY-MM-DD format */
void getToday(char date[])
{
    time_t now = time(NULL);
    struct tm *today = localtime(&now);
    strftime(date, 11, "%Y-%m-%d", today);
}

/* Converts YYYY-MM-DD into a time value */
time_t convertDate(char date[])
{
    int year, month, day;
    struct tm dateParts = {0};

    if (sscanf(date, "%4d-%2d-%2d", &year, &month, &day) != 3)
        return (time_t)-1;

    dateParts.tm_year = year - 1900;
    dateParts.tm_mon = month - 1;
    dateParts.tm_mday = day;
    dateParts.tm_isdst = -1;

    return mktime(&dateParts);
}

/* Calculates the number of days between two dates */
int daysBetween(char start[], char end[])
{
    time_t first = convertDate(start);
    time_t second = convertDate(end);

    if (first == (time_t)-1 || second == (time_t)-1 || second < first)
        return -1;

    return (int)(difftime(second, first) / (60 * 60 * 24));
}

/* Student borrows a book */
void rentBook(int sap)
{
    int id, index;

    loadBooks();
    loadLoans();

    if (count == 0)
    {
        printf("\nNo books available to borrow.\n");
        return;
    }

    displayBooks();
    printf("\nEnter the Book ID you want to borrow: ");

    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("\nInvalid Book ID!\n");
        return;
    }
    clearInputBuffer();

    index = findBook(id);

    if (index == -1)
    {
        printf("\nBook not found!\n");
        return;
    }

    if (isBookBorrowed(id))
    {
        printf("\nSorry! This book is already borrowed by another student.\n");
        return;
    }

    if (studentHasBook(sap, id))
    {
        printf("\nYou have already borrowed this book.\n");
        return;
    }

    if (loanCount >= MAX_LOANS)
    {
        printf("\nBorrowing record limit reached.\n");
        return;
    }

    loans[loanCount].sap = sap;
    loans[loanCount].bookID = id;
    strcpy(loans[loanCount].bookName, books[index].name);
    getToday(loans[loanCount].borrowDate);
    loanCount++;

    saveLoans();

    printf("\nYou have successfully borrowed \"%s\".\n", books[index].name);
    printf("Student SAP ID: %d\n", sap);
    printf("Borrow date: %s\n", loans[loanCount - 1].borrowDate);
    printf("Please return the book within 14 days to avoid a fine.\n");
}

/* Student purchases a book (original feature retained) */
void purchaseBook()
{
    int id, index;
    char paid;

    loadBooks();
    if (count == 0)
    {
        printf("\nNo books available to purchase.\n");
        return;
    }

    displayBooks();
    printf("\nEnter the Book ID you want to purchase: ");

    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("\nInvalid Book ID!\n");
        return;
    }
    clearInputBuffer();

    index = findBook(id);
    if (index == -1)
    {
        printf("\nBook not found!\n");
        return;
    }

    printf("\nBook Name: %s\n", books[index].name);
    printf("Price: Rs. %.2f\n", books[index].price);
    printf("\nHave you paid? (y/n): ");

    scanf(" %c", &paid);
    clearInputBuffer();

    if (paid == 'y' || paid == 'Y')
    {
        printf("\nPurchase successful!\n");
        printf("Thank you for purchasing \"%s\".\n", books[index].name);
    }
    else if (paid == 'n' || paid == 'N')
        printf("\nPurchase cancelled.\n");
    else
        printf("\nInvalid choice!\n");
}

/* Original explore/filter feature retained */
void exploreLibrary()
{
    char author[100] = "", genre[50] = "", name[100] = "";
    char answer;
    int id = -1, useAuthor = 0, useGenre = 0, useName = 0;
    int usePrice = 0, useID = 0, i, found = 0;
    float minPrice = 0, maxPrice = 0;

    loadBooks();
    if (count == 0)
    {
        printf("\nNo books available to explore.\n");
        return;
    }

    printf("\n========== EXPLORE THE LIBRARY ==========\n");

    printf("\nDo you want to filter by author? (y/n): ");
    scanf(" %c", &answer);
    clearInputBuffer();
    if (answer == 'y' || answer == 'Y')
    {
        useAuthor = 1;
        printf("Enter author name: ");
        readLine(author, sizeof(author));
    }

    printf("\nDo you want to filter by genre? (y/n): ");
    scanf(" %c", &answer);
    clearInputBuffer();
    if (answer == 'y' || answer == 'Y')
    {
        useGenre = 1;
        printf("Enter genre: ");
        readLine(genre, sizeof(genre));
    }

    printf("\nDo you want to filter by book name? (y/n): ");
    scanf(" %c", &answer);
    clearInputBuffer();
    if (answer == 'y' || answer == 'Y')
    {
        useName = 1;
        printf("Enter book name: ");
        readLine(name, sizeof(name));
    }

    printf("\nDo you want to filter by price range? (y/n): ");
    scanf(" %c", &answer);
    clearInputBuffer();
    if (answer == 'y' || answer == 'Y')
    {
        usePrice = 1;
        printf("Enter minimum price: ");
        if (scanf("%f", &minPrice) != 1)
        {
            clearInputBuffer();
            printf("\nInvalid price!\n");
            return;
        }
        printf("Enter maximum price: ");
        if (scanf("%f", &maxPrice) != 1)
        {
            clearInputBuffer();
            printf("\nInvalid price!\n");
            return;
        }
        clearInputBuffer();
        if (minPrice < 0 || maxPrice < minPrice)
        {
            printf("\nInvalid price range!\n");
            return;
        }
    }

    printf("\nDo you want to filter by Book ID? (y/n): ");
    scanf(" %c", &answer);
    clearInputBuffer();
    if (answer == 'y' || answer == 'Y')
    {
        useID = 1;
        printf("Enter Book ID: ");
        if (scanf("%d", &id) != 1)
        {
            clearInputBuffer();
            printf("\nInvalid Book ID!\n");
            return;
        }
        clearInputBuffer();
    }

    printf("\n================ SEARCH RESULTS ================\n");
    printf("%-25s %-10s %-10s %-25s %-15s\n",
           "Book Name", "Book ID", "Price", "Author", "Genre");
    printf("-------------------------------------------------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        int match = 1;
        if (useAuthor && strcmp(author, books[i].author) != 0) match = 0;
        if (useGenre && strcmp(genre, books[i].genre) != 0) match = 0;
        if (useName && strcmp(name, books[i].name) != 0) match = 0;
        if (usePrice && (books[i].price < minPrice || books[i].price > maxPrice)) match = 0;
        if (useID && books[i].id != id) match = 0;

        if (match)
        {
            printf("%-25s %-10d Rs.%-7.2f %-25s %-15s\n",
                   books[i].name, books[i].id, books[i].price,
                   books[i].author, books[i].genre);
            found++;
        }
    }

    printf("-------------------------------------------------------------------------------\n");
    if (found == 0)
        printf("\nSorry! No books match your search criteria.\n");
    else
        printf("\nTotal books found: %d\n", found);
}

/* Student returns their borrowed book; fine is calculated from dates */
void returnBook(int sap)
{
    int id, i, days, fine;
    char today[11];

    loadLoans();
    printf("\nEnter the Book ID you want to return: ");

    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("\nInvalid Book ID!\n");
        return;
    }
    clearInputBuffer();

    for (i = 0; i < loanCount; i++)
        if (loans[i].sap == sap && loans[i].bookID == id)
            break;

    if (i == loanCount)
    {
        printf("\nNo active borrowing record found for this SAP ID and book.\n");
        return;
    }

    getToday(today);
    days = daysBetween(loans[i].borrowDate, today);

    if (days < 0)
    {
        printf("\nDate error in borrowing record. Please contact the admin.\n");
        return;
    }

    /* Fine policy retained from the original program:
       First 14 days free, next 7 days Rs. 10/day,
       and every day after 21 days Rs. 20/day. */
    if (days <= 14)
        fine = 0;
    else if (days <= 21)
        fine = (days - 14) * 10;
    else
        fine = (7 * 10) + ((days - 21) * 20);

    printf("\nBook returned successfully!\n");
    printf("Book Name: %s\n", loans[i].bookName);
    printf("Borrow date: %s\n", loans[i].borrowDate);
    printf("Return date: %s\n", today);
    printf("Days kept: %d\n", days);
    printf("Fine: Rs. %d\n", fine);

    if (fine == 0)
        printf("No fine! Thank you for returning on time.\n");
    else
        printf("Please pay the fine amount.\n");

    /* Remove the active loan record after return */
    for (; i < loanCount - 1; i++)
        loans[i] = loans[i + 1];
    loanCount--;
    saveLoans();
}

/* Admin adds a new book */
void addBook()
{
    struct Book newBook;

    loadBooks();
    if (count >= MAX_BOOKS)
    {
        printf("\nBook catalogue is full.\n");
        return;
    }

    printf("\nEnter new book name: ");
    readLine(newBook.name, sizeof(newBook.name));
    printf("Enter Book ID: ");
    if (scanf("%d", &newBook.id) != 1)
    {
        clearInputBuffer();
        printf("\nInvalid ID!\n");
        return;
    }
    clearInputBuffer();

    if (findBook(newBook.id) != -1)
    {
        printf("\nA book with this ID already exists.\n");
        return;
    }

    printf("Enter price: ");
    if (scanf("%f", &newBook.price) != 1 || newBook.price < 0)
    {
        clearInputBuffer();
        printf("\nInvalid price!\n");
        return;
    }
    clearInputBuffer();

    printf("Enter author: ");
    readLine(newBook.author, sizeof(newBook.author));
    printf("Enter genre: ");
    readLine(newBook.genre, sizeof(newBook.genre));

    books[count] = newBook;
    count++;
    saveBooks();
    printf("\nBook added successfully!\n");
}

/* Admin removes a book; a currently borrowed book cannot be removed */
void removeBook()
{
    int id, index, i;

    loadBooks();
    loadLoans();
    displayBooks();

    printf("\nEnter the Book ID you want to remove: ");
    if (scanf("%d", &id) != 1)
    {
        clearInputBuffer();
        printf("\nInvalid Book ID!\n");
        return;
    }
    clearInputBuffer();

    index = findBook(id);
    if (index == -1)
    {
        printf("\nBook not found!\n");
        return;
    }
    if (isBookBorrowed(id))
    {
        printf("\nThis book is currently borrowed. It cannot be removed.\n");
        return;
    }

    for (i = index; i < count - 1; i++)
        books[i] = books[i + 1];
    count--;
    saveBooks();
    printf("\nBook removed successfully!\n");
}

/* Admin credentials are stored in admin.txt as username|password */
int adminLogin(char username[], char password[])
{
    FILE *fp = fopen("admin.txt", "r");
    char savedUser[50], savedPassword[50];

    if (fp == NULL)
    {
        printf("\nadmin.txt not found. Create it with: admin|123\n");
        return 0;
    }

    if (fscanf(fp, "%49[^|]|%49s", savedUser, savedPassword) != 2)
    {
        fclose(fp);
        printf("\nInvalid admin.txt format.\n");
        return 0;
    }
    fclose(fp);

    return strcmp(username, savedUser) == 0 &&
           strcmp(password, savedPassword) == 0;
}

/* Main program */
int main()
{
    int role, sap, choice;
    char username[50], password[50];

    printf("========================================\n");
    printf("       LIBRARY MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf("1. Student Login\n");
    printf("2. Admin Login\n");
    printf("Enter your choice: ");

    if (scanf("%d", &role) != 1)
    {
        printf("\nInvalid choice.\n");
        return 0;
    }
    clearInputBuffer();

    if (role == 1)
    {
        printf("\nEnter your Student SAP ID: ");
        if (scanf("%d", &sap) != 1)
        {
            printf("\nInvalid SAP ID.\n");
            return 0;
        }
        clearInputBuffer();
        if (sap <= 0)
        {
            printf("\nInvalid SAP ID.\n");
            return 0;
        }

        printf("\nStudent login successful! Welcome, SAP ID %d.\n", sap);

        do
        {
            printf("\n=============== STUDENT MENU ===============\n");
            printf("1. Borrow a Book\n");
            printf("2. Purchase a Book\n");
            printf("3. Explore Library\n");
            printf("4. Return a Book\n");
            printf("5. Exit\n");
            printf("Enter your choice: ");

            if (scanf("%d", &choice) != 1)
            {
                clearInputBuffer();
                printf("\nInvalid input! Please enter a number.\n");
                continue;
            }
            clearInputBuffer();

            switch (choice)
            {
                case 1: rentBook(sap); break;
                case 2: purchaseBook(); break;
                case 3: exploreLibrary(); break;
                case 4: returnBook(sap); break;
                case 5: printf("\nLogged out. Thank you!\n"); break;
                default: printf("\nInvalid choice! Please try again.\n");
            }
        } while (choice != 5);
    }
    else if (role == 2)
    {
        printf("\nEnter admin username: ");
        readLine(username, sizeof(username));
        printf("Enter admin password: ");
        readLine(password, sizeof(password));

        if (!adminLogin(username, password))
        {
            printf("\nIncorrect admin username or password.\n");
            return 0;
        }

        printf("\nAdmin login successful!\n");

        do
        {
            printf("\n================ ADMIN MENU ================\n");
            printf("1. Display Books\n");
            printf("2. Add a Book\n");
            printf("3. Remove a Book\n");
            printf("4. Explore Library\n");
            printf("5. Exit\n");
            printf("Enter your choice: ");

            if (scanf("%d", &choice) != 1)
            {
                clearInputBuffer();
                printf("\nInvalid input! Please enter a number.\n");
                continue;
            }
            clearInputBuffer();

            switch (choice)
            {
                case 1: loadBooks(); displayBooks(); break;
                case 2: addBook(); break;
                case 3: removeBook(); break;
                case 4: exploreLibrary(); break;
                case 5: printf("\nAdmin logged out.\n"); break;
                default: printf("\nInvalid choice! Please try again.\n");
            }
        } while (choice != 5);
    }
    else
        printf("\nInvalid login option.\n");

    return 0;
}

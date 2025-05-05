#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define SIZE 9
#define UNASSIGNED 0

int grid[SIZE][SIZE];
int solution[SIZE][SIZE];
int originalGrid[SIZE][SIZE];
int cursorX = 0, cursorY = 0;
int mistakes = 0, hintsUsed = 0;
time_t startTime;

void clearScreen() {
    system("cls");
}

void setCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int isSafe(int row, int col, int num) {
    for (int x = 0; x < SIZE; x++)
        if (grid[row][x] == num) return 0;
    for (int x = 0; x < SIZE; x++)
        if (grid[x][col] == num) return 0;

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j + startCol] == num) return 0;

    return 1;
}

int solveSudoku() {
    int row, col, isEmpty = 0;

    for (row = 0; row < SIZE; row++) {
        for (col = 0; col < SIZE; col++) {
            if (grid[row][col] == UNASSIGNED) {
                isEmpty = 1;
                break;
            }
        }
        if (isEmpty) break;
    }

    if (!isEmpty) return 1;

    for (int num = 1; num <= 9; num++) {
        if (isSafe(row, col, num)) {
            grid[row][col] = num;
            if (solveSudoku()) return 1;
            grid[row][col] = UNASSIGNED;
        }
    }
    return 0;
}

void generatePuzzle() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = UNASSIGNED;

    for (int box = 0; box < 3; box++) {
        int nums[9] = {1,2,3,4,5,6,7,8,9};
        for (int i = 0; i < 9; i++) {
            int j = rand() % 9;
            int temp = nums[i];
            nums[i] = nums[j];
            nums[j] = temp;
        }
        int idx = 0;
        for (int i = box*3; i < box*3+3; i++)
            for (int j = box*3; j < box*3+3; j++)
                grid[i][j] = nums[idx++];
    }

    solveSudoku();

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            solution[i][j] = grid[i][j];

    int cellsToRemove = 40 + rand() % 11;
    while (cellsToRemove > 0) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (grid[row][col] != UNASSIGNED) {
            grid[row][col] = UNASSIGNED;
            cellsToRemove--;
        }
    }

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            originalGrid[i][j] = grid[i][j];
}

void printGame() {
    clearScreen();
    printf("\n  SUDOKU - Mistakes: %d/3  Hints: %d/2  Time: ", mistakes, hintsUsed);
    int elapsed = (int)(time(NULL) - startTime);
    printf("%02d:%02d\n\n", elapsed/60, elapsed%60);

    printf("   1 2 3   4 5 6   7 8 9\n");
    printf(" +-------+-------+-------+\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%c|", 'A' + i);
        for (int j = 0; j < SIZE; j++) {
            if (cursorX == j && cursorY == i)
                printf("[");
            else
                printf(" ");

            if (grid[i][j] == UNASSIGNED)
                printf(" ");
            else
                printf("%d", grid[i][j]);

            if (cursorX == j && cursorY == i)
                printf("]");
            else
                printf(" ");

            if (j % 3 == 2) printf("|");
        }
        printf("\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
    printf("\nUse arrow keys to move, Enter to select, H for hint, S to solve, C to change puzzle, ESC to quit\n");
}

void provideHint() {
    if (hintsUsed < 2) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] == UNASSIGNED) {
                    grid[i][j] = solution[i][j];
                    hintsUsed++;
                    return;
                }
            }
        }
    }
}

void solvePuzzle() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = solution[i][j];
        }
    }
}

void displaySolvedPuzzle() {
    clearScreen();
    printf("\n  SOLVED SUDOKU\n\n");

    printf("   1 2 3   4 5 6   7 8 9\n");
    printf(" +-------+-------+-------+\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%c|", 'A' + i);
        for (int j = 0; j < SIZE; j++) {
            printf(" %d ", solution[i][j]);
            if (j % 3 == 2) printf("|");
        }
        printf("\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
    printf("\nPress any key to continue...\n");
    _getch();
}

void playGame() {
    startTime = time(NULL);
    mistakes = 0;
    hintsUsed = 0;

    srand(time(NULL));
    generatePuzzle();

    int gameRunning = 1;
    while (gameRunning && mistakes < 3) {
        printGame();

        int key = _getch();
        if (key == 224) {
            key = _getch();
            switch (key) {
                case 72: if (cursorY > 0) cursorY--; break;
                case 80: if (cursorY < 8) cursorY++; break;
                case 75: if (cursorX > 0) cursorX--; break;
                case 77: if (cursorX < 8) cursorX++; break;
            }
        }
        else if (key == 13) {
            if (originalGrid[cursorY][cursorX] == 0) {
                printf("Enter number (1-9): ");
                int num = _getch() - '0';
                if (num >= 1 && num <= 9) {
                    if (num == solution[cursorY][cursorX]) {
                        grid[cursorY][cursorX] = num;
                    } else {
                        mistakes++;
                    }
                }
            }
        }
        else if (key == 27) {
            gameRunning = 0;
        }
        else if (key == '0') {
            if (originalGrid[cursorY][cursorX] == 0) {
                grid[cursorY][cursorX] = UNASSIGNED;
            }
        }
        else if (key == 'H' || key == 'h') {
            provideHint();
        }
        else if (key == 'S' || key == 's') {
            solvePuzzle();
            displaySolvedPuzzle();
            int finalTime = (int)(time(NULL) - startTime);
            clearScreen();
            printf("\nYou chose to solve the puzzle automatically.\n");
            printf("Time taken: %02d:%02d\n", finalTime/60, finalTime%60);
            printf("Mistakes made: %d/3\n", mistakes);
            printf("Your score: 0\n");
            printf("\nPress any key to exit...");
            _getch();
            return;
        }
        else if (key == 'C' || key == 'c') {
            generatePuzzle();
        }

        int isComplete = 1;
        for (int i = 0; i < SIZE && isComplete; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] != solution[i][j]) {
                    isComplete = 0;
                    break;
                }
            }
        }

        if (isComplete) {
            int finalTime = (int)(time(NULL) - startTime);
            int score = 100 - (finalTime/3) - (mistakes*5);
            if (score < 0) score = 0;

            clearScreen();
            printf("\nCongratulations! You solved the puzzle!\n");
            printf("Time taken: %02d:%02d\n", finalTime/60, finalTime%60);
            printf("Mistakes made: %d/3\n", mistakes);
            printf("Your score: %d/100\n", score);
            printf("\nPress any key to exit...");
            _getch();
            return;
        }
    }

    if (mistakes >= 3) {
        clearScreen();
        printf("\nGame Over! Too many mistakes.\n");
        printf("Press any key to exit...");
        _getch();
    }
}

int main() {
    playGame();
    return 0;
}

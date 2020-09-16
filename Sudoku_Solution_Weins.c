/**
 * A pthread program that checks a 9 x 9 Sudoku Puzzle for correctness
 * by creating threads to check columns, rows, and subgrids of the puzzle.
 * 
 * Most Unix/Linux/OS X users
 * gcc Sudoku_Solution_Weins.c -o <prog_name> -lpthread
 * 
 * To run the program with the default file SudokuPuzzle.txt use:
 * ./<prog_name> 
 * 
 * To run the program with a different file use:
 * ./<prog_name> <file name>
 * 
 * @author Jessica Weins
 * @version 1.0
 * Professor Weiying Zhu
 * CS 3600
 * Assignment #3
 * 15 September 2020
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_CHILD_THREADS 9

typedef int bool;

#define TRUE 1
#define FALSE 0

int sudokuPuzzle[9][9];
bool columns[9];
bool columnsValid;
bool rows[9];
bool rowsValid;
bool subGrids[9];
bool subGridsValid;

void *columnRunner(void *param);
void *rowRunner(void *param);
void *subgridRunner(void *param);

typedef struct{
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
    int subGrid;
} run_param;

int main(int argc, char *argv[])
{
    int i, row, column, character;

    /* Structures for the columns */
    pthread_t tid_column[9];
    pthread_attr_t attr_column[9];
    run_param thrParam_column[NUM_CHILD_THREADS];
    
    /* Structures for the rows */
    pthread_t tid_row[9];
    pthread_attr_t attr_row[9];
    run_param thrParam_row[NUM_CHILD_THREADS];
    
    /* Structures for the subgrids */
    pthread_t tid_subgrid[9];
    pthread_attr_t attr_subgrid[9];
    run_param thrParam_subgrid[NUM_CHILD_THREADS];


    /* Open sudoku puzzle .txt file containing answers seperated by tabs */
    FILE *puzzleAnswers;
    if(argc == 2){
        /*/* Open file entered as an argument in the run command */
        puzzleAnswers = fopen(argv[1], "r");
    }else{
        /* Use default SudokuPuzzle.txt */
        puzzleAnswers = fopen("SudokuPuzzle.txt", "r");
    }

    /* Reads in each character from the txt.file and fills out sudokuPuzzle[9][9] */
    if(puzzleAnswers == 0){
        printf("Could not open file");
    }else {
        row = 0;
        column = 0;
        while( (character = fgetc(puzzleAnswers)) != EOF){
            if(character != '\t'){
                if(column < 9){
                    sudokuPuzzle[row][column] = character;
                    column++;
                }else{
                    column = 0;
                    row++;
                }              
            }
        }
        fclose(puzzleAnswers);
    }

    /* sets values and attributes for columns, rows, and subgrids*/
    for(i = 0; i < 9; i++){
        /* Columns */
        thrParam_column[i].leftColumn = i;
        thrParam_column[i].rightColumn = i;
        thrParam_column[i].topRow = 0;
        thrParam_column[i].bottomRow = 8;

        /* Rows */
        thrParam_row[i].leftColumn = 0;
        thrParam_row[i].rightColumn = 8;
        thrParam_row[i].topRow = i;
        thrParam_row[i].bottomRow = i;
        
        /*  Top three subgrids */
        if(i < 3){
            thrParam_subgrid[i].leftColumn = 3 * i;
            thrParam_subgrid[i].rightColumn = 2 + (3 * i);
            thrParam_subgrid[i].topRow = 0;
            thrParam_subgrid[i].bottomRow = 2;
            thrParam_subgrid[i].subGrid = i;
        }
        /* Middle three subgrids */
        else if(i < 6){
            thrParam_subgrid[i].leftColumn = 3 * (i - 3);
            thrParam_subgrid[i].rightColumn = 2 + (3 * (i - 3));
            thrParam_subgrid[i].topRow = 3;
            thrParam_subgrid[i].bottomRow = 5;
            thrParam_subgrid[i].subGrid = i;
        }
        /* Bottom three subgrids */
        else{
            thrParam_subgrid[i].leftColumn = 3 * (i - 6);
            thrParam_subgrid[i].rightColumn = 2 + (3 * (i - 6));
            thrParam_subgrid[i].topRow = 6;
            thrParam_subgrid[i].bottomRow = 8;
            thrParam_subgrid[i].subGrid = i;
        }

        pthread_attr_init(&(attr_column[i]));
        pthread_attr_init(&(attr_row[i]));
        pthread_attr_init(&(attr_subgrid[i]));
    }

    /* Create the threads for columns */
    for(i = 0; i < 9; i++){
        pthread_create(&(tid_column[i]), &(attr_column[i]), columnRunner, &(thrParam_column[i]));
        pthread_create(&(tid_row[i]), &(attr_row[i]), rowRunner, &(thrParam_row[i]));
        pthread_create(&(tid_subgrid[i]), &(attr_subgrid[i]), subgridRunner, &(thrParam_subgrid[i]));
    }

    /* Wait for the threads to exit */
    for(i = 0; i < 9; i++){
        pthread_join(tid_column[i], NULL);
        pthread_join(tid_row[i], NULL);
        pthread_join(tid_subgrid[i], NULL);
    }

    /* The parent thread displays information from the work done by the child threads */
    printf("\n\n");
    columnsValid = TRUE;
    for(i = 0; i < 9; i++){
        if(columns[i] == TRUE){
            printf("\nColumn: %lx valid", tid_column[i]);
        }else{
            printf("\nColumn: %lx invalid", tid_column[i]);
            columnsValid = FALSE;
        }
    }

    printf("\n");
    rowsValid = TRUE;
    for(i = 0; i < 9; i++){
        if(rows[i] == TRUE){
            printf("\nRow: %lx valid", tid_row[i]);
        }else{
            printf("\nRow: %lx invalid", tid_row[i]);
            rowsValid = FALSE;
        }
    }

    printf("\n");
    subGridsValid = TRUE;
    for(i = 0; i < 9; i++){
        if(subGrids[i] == TRUE){
            printf("\nSubgrid: %lx valid", tid_subgrid[i]);
        }else{
            printf("\nSubgrid: %lx invalid", tid_subgrid[i]);
            subGridsValid = FALSE;
        }
    }

    if(columnsValid == TRUE && rowsValid == TRUE && subGridsValid == TRUE){
        printf("\n\nSudoku Puzzle: valid\n");
    }else{
        printf("\n\nSudoku Puzzle: invalid\n");
    }

    return 0;
}

/* Function for each child thread to check the validity of assigned column of the Sudoku Puzzle */
void *columnRunner(void *param)
{
    run_param *inP;
    pthread_t self;
    int columnL,columnR, bottomR, topR;
    int num, row, count;
    long int puzzleAnswer, numberChoices;

    inP =(run_param *)param;
    topR = inP->topRow;
    bottomR = inP->bottomRow;
    columnL = inP->leftColumn;
    columnR = inP->rightColumn;

    int numbers[9] = {1,2,3,4,5,6,7,8,9};

    self = pthread_self();

    /* Intialize the columns[columnR] as TRUE meaning all 9 numbers are present in the column*/
    columns[columnR] = TRUE;

    for(row = 0; row < 9; row ++){
        count = 0;
        for(num = 0 ; num < 9; num++){
            puzzleAnswer = (sudokuPuzzle[row][columnR]) - 48;
            numberChoices = numbers[num];

            if(numberChoices == puzzleAnswer){
                numbers[num] = 0;
                break;
            }else{
                count++;
            }

            if(count == 9){
                /* Set the value in columns[columnR] to be FALSE since not all 9 numbers are present in the column*/
                columns[columnR] = FALSE;   
            }
        }
    }

    if(columns[columnR] == TRUE){
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!", self, topR, bottomR, columnL, columnR);
    }else{
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!", self, topR, bottomR, columnL, columnR);
    }
}

/* Function for each child thread to check the validity of assigned row of the Sudoku Puzzle */
void *rowRunner(void *param)
{
    run_param *inP;
    pthread_t self;

    int columnL,columnR, bottomR, topR;
    int num, column, count;
    int puzzleAnswer, numberChoices;

    inP =(run_param *)param;
    topR = inP->topRow;
    bottomR = inP->bottomRow;
    columnL = inP->leftColumn;
    columnR = inP->rightColumn;

    int numbers[9] = {1,2,3,4,5,6,7,8,9};

    self = pthread_self();

    /* Intialize the rows[topR] as TRUE meaning all 9 numbers are present in the row*/
    rows[topR] = TRUE;

    for(column = 0; column  < 9; column ++){
        count = 0;
        for(num = 0 ; num < 9; num++){
            puzzleAnswer = (sudokuPuzzle[topR][column]) - 48;
            numberChoices = numbers[num];

            if(numberChoices == puzzleAnswer){
                numbers[num] = 0;
                break;
            }else{
                count++;
            }

            if(count == 9){
                /* Set the value in rows[topR] to be FALSE since not all 9 numbers are present in the row*/
                rows[topR] = FALSE;
            }
        }
    }

    if(rows[topR] == TRUE){
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!", self, topR, bottomR, columnL, columnR);
    }else{
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!", self, topR, bottomR, columnL, columnR);
    }
}

/* Function for each child thread to check the validity of assigned subgrid of the Sudoku Puzzle */
void *subgridRunner(void *param)
{
    run_param *inP;
    pthread_t self;

    int columnL,columnR, bottomR, topR, subgridNum;
    int num, column, row, count;
    int puzzleAnswer, numberChoices;

    inP =(run_param *)param;
    topR = inP->topRow;
    bottomR = inP->bottomRow;
    columnL = inP->leftColumn;
    columnR = inP->rightColumn;
    subgridNum = inP->subGrid;

    int numbers[9] = {1,2,3,4,5,6,7,8,9};

    self = pthread_self();

    /* Intialize the subGrids[subgridNum] as TRUE meaning all 9 numbers are present in the subgrid*/
    subGrids[subgridNum] = TRUE;

    for(column = columnL; column  <= columnR; column ++){
        for(row = topR; row <= bottomR; row++){
            count = 0;
            for(num = 0 ; num < 9; num++){
                puzzleAnswer = (sudokuPuzzle[row][column]) - 48;
                numberChoices = numbers[num];

                if(numberChoices == puzzleAnswer){
                    numbers[num] = 0;
                    break;
                }else{
                    count++;
                }

                if(count == 9){
                    /* Set the value in subGrids[subgridNum] to be FALSE since not all 9 numbers are present in the subgrid*/
                    subGrids[subgridNum] = FALSE;
                }
            }
        }
    }

    if(subGrids[subgridNum] == TRUE){
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!", self, topR, bottomR, columnL, columnR);
    }else{
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!", self, topR, bottomR, columnL, columnR);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PLAYER 1
#define CPU 2
#define GIVEUP 3

#define NONE 0
#define MISS -1
#define HIT -2

#define MAX 26

int cpuBoard[MAX][MAX]; //initialises 2D array for the cpu board
int playerBoard[MAX][MAX]; //initialises 2D array for the player board

// Assigns exit codes to error codes
enum Errors {
    E_NOT_ENOUGH_PARAMETERS = 10,
    E_RULES_MISSING = 20,
    E_PLAYER_MAP_MISSING = 30,
    E_CPU_MAP_MISSING = 31,
    E_CPU_TURNS_MISSING = 40,
    E_RULES = 50,
    E_PLAYER_SHIP_OVERLAP = 60,
    E_CPU_SHIP_OVERLAP = 70,
    E_PLAYER_MAP_OOB = 80,
    E_CPU_MAP_OOB = 90,
    E_PLAYER_MAP = 100,
    E_CPU_MAP = 110,
    E_TURNS = 120,
    E_PLAYER_GIVES_UP = 130,
    E_CPU_GIVES_UP = 140
};

/* Prints the error message to stdout and exits with the exit code correspon
 * ding to the error code parameter.
 *
 * @param (int code) (enumerator value corresponding to the error code)
 */
void error_exit(int code)
{
    switch (code) {
        case E_NOT_ENOUGH_PARAMETERS:
            fprintf(stderr, "Usage: naval rules playermap cpumap turns\n");
            exit(10);
        case E_RULES_MISSING:
            fprintf(stderr, "Missing rules file\n");
            exit(20);
        case E_PLAYER_MAP_MISSING:
            fprintf(stderr, "Missing player map file\n");
            exit(30);
        case E_CPU_MAP_MISSING:
            fprintf(stderr, "Missing CPU map file\n");
            exit(31);
        case E_CPU_TURNS_MISSING:
            fprintf(stderr, "Missing CPU turns file\n");
            exit(40);
        case E_RULES:
            fprintf(stderr, "Error in rules file\n");
            exit(50);
        case E_PLAYER_SHIP_OVERLAP:
            fprintf(stderr, "Overlap in player map file\n");
            exit(60);
        case E_CPU_SHIP_OVERLAP:
            fprintf(stderr, "Overlap in CPU map file\n");
            exit(70);
        case E_PLAYER_MAP_OOB:
            fprintf(stderr, "Out of bounds in player map file\n");
            exit(80);
        case E_CPU_MAP_OOB:
            fprintf(stderr, "Out of bounds in CPU map file\n");
            exit(90);
        case E_PLAYER_MAP:
            fprintf(stderr, "Error in player map file\n");
            exit(100);
        case E_CPU_MAP:
            fprintf(stderr, "Error in CPU map file\n");
            exit(110);
        case E_TURNS:
            fprintf(stderr, "Error in turns file\n");
            exit(120);
        case E_PLAYER_GIVES_UP:
            fprintf(stderr, "Bad guess\n");
            exit(130);
        case E_CPU_GIVES_UP:
            fprintf(stderr, "CPU player gives up\n");
            exit(140);
        default:
            exit(0);
    }
}

/* Initialises an empty cpu board.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void initialise_cpu_board(int width, int height)
{
    for (int i = 1; i < (height + 1); i++) {
        for (int j = 1; j < (width + 1); j++) {
            cpuBoard[i][j] = NONE;
        }
    }
}

/* Initialises an empty player board.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void initialise_player_board(int width, int height)
{
    for (int i = 1; i < (height + 1); i++) {
        for (int j = 1; j < (width + 1); j++) {
            playerBoard[i][j] = NONE;
        }
    }
}

/* Reads one line of the provided file and returns it.
 *
 * @param (FILE* file) (filename of the file to be read)
 *
 * @return (char* result) (the line read as a string)
 */
char* read_line(FILE* file)
{
    char* result = malloc(sizeof(char) * 80);
    int position = 0;
    int next = 0;
    
    while (1) {
        next = fgetc(file);
        if (next == '#') {
            read_line(file);
        } else {
            if (next == '\n' || next == EOF) {
                result[position] = '\0';
                return result;
            } else {
                result[position++] = (char)next;
            }
        }
    }
}

/* Exits the game if the provided files are empty.
 *
 * @param (FILE* rules) (filename of the rules file)
 * @param (FILE* plrmap) (filename of the player map file)
 * @param (FILE* cpumap) (filename of the cpu map file)
 * @param (FILE* turns) (filename of the turns file)
 */
void check_null_files(FILE* rules, FILE* plrmap, FILE* cpumap, FILE* turns)
{
    if (rules == NULL) {
        error_exit(E_RULES_MISSING);
    }
    if (plrmap == NULL) {
        error_exit(E_PLAYER_MAP_MISSING);
    }
    if (cpumap == NULL) {
        error_exit(E_CPU_MAP_MISSING);
    }
    if (turns == NULL) {
        error_exit(E_CPU_TURNS_MISSING);
    }
}

/* Closes files at the end of the game.
 *
 * @param all the same as check_null_files function
 */
void close_files(FILE* rules, FILE* plrmap, FILE* cpumap, FILE* turns)
{
    fclose(rules);
    fclose(plrmap);
    fclose(cpumap);
    fclose(turns);
}

/* Removes leading and trailing whitespace from provided string.
 *
 * @param (char* string) (string to have whitespace removed from)
 *
 * @return (char* string) (string with whitespace removed)
 */
char* trim_whitespace(char* string)
{
    char* end;
    while (isspace((unsigned char)*string)) {
        string++;
    }
    end = string + strlen(string) - 1;
    while (end > string && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return string;
}

/* Returns a character based on the value. * for a hit, / for a miss and . 
 * otherwise. 
 *
 * @param (int value) (status of position on the board) 
 *
 * @return (char c) (character to be placed in that position of the board) 
 */
char cpu_chars(int value)
{
    char c;
    switch (value) {
        case HIT:
            c = '*';
            break;
        case MISS:
            c = '/';
            break;
        case NONE:
            c = '.';
            break;
        default:
            c = '.';
            break;
    }
    return c;
}

/* Returns a character based on the value. * for a hit, the ship number for 
 * a ship and . otherwise. 
 *
 * @param (int value) (status of position on the board) 
 *
 * @return (char c) (character to be placed in that position of the board) 
 */
char player_chars(int value)
{
    char c;
    switch (value) {
        case HIT:
            c = '*';
            break;
        case MISS:
            //remain as is 
        case NONE:
            c = '.';
            break;
        default:
            if (value > 9) {
                c = value + 55;
            } else {
                c = value + '0';
            }
            break;
    } 
    return c;
}

/* Checks if the input string is a valid move and returns 0 if so.
 * 
 * @param (char* move) (the ove just played as a string)
 *
 * @return (int) (0 if a bad move, 1 if not)
 */
int check_bad_move(char* move)
{ 
    if (strlen(move) == 3) {
        if (!(isupper(move[0])) || !(isdigit(move[1])) || 
                !(isdigit(move[2]))) {
            return 0;
        }
    } else if (strlen(move) == 2) {
        if (!(isupper(move[0])) || !(isdigit(move[1]))) {
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}

/* Checks if the x and y coordinates are valid and returns 0 if so.
 * 
 * @param (int x) (x coordinate of the boards position)
 * @param (int y) (y coordinate of the boards position)
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 *
 * @return (int) (0 if a bad guess, 1 if not)
 */
int check_bad_guess(int x, int y, int width, int height)
{
    if ((x > width) || (x < 1) || (y < 1) || (y > height)) { 
        return 0;
    }
    return 1;
}

/* Checks if the move just played has been played before.
 *
 * @param (int player) (PLAYER or CPU depending on which map file)
 * @param (int x) (x coordinate of the boards position)
 * @param (int y) (y coordinate of the boards position)
 *
 * @return (int) (0 if repeated guess, 1 if not)
 */
int check_repeat(int player, int x, int y)
{
    if (player == CPU) {
        if (playerBoard[y][x] == HIT || playerBoard[y][x] == MISS) {
            return 0;
        }
    } else if (player == PLAYER) {
        if (cpuBoard[y][x] == HIT || cpuBoard[y][x] == MISS) {
            return 0;
        }
    }
    return 1;
}

/* Checks if the hit just played sunk a ship and prints sunk ship if so..
 *
 * @param (int player) (PLAYER or CPU depending on which map file)
 * @param (int ship) (ship number that was hit)
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void check_sunk(int player, int ship, int width, int height)
{
    if (player == PLAYER) {
        for (int i = 1; i < (height + 1); i++) {
            for (int j = 1; j < (width + 1); j++) {
                if (cpuBoard[i][j] == ship) {
                    // another position exists with that ship in it
                    return;
                }
            }
        } 
        // no other positions exists with that ship on it
        printf("Ship sunk\n");
    } else if (player == CPU) { 
        for (int i = 1; i < (height + 1); i++) {
            for (int j = 1; j < (width + 1); j++) {
                if (playerBoard[i][j] == ship) {
                    return;
                }
            }
        }
        printf("Ship sunk\n");
    }
}

/* Checks if the move just played was a hit or miss and updates the board 
 * character accordingly.
 *
 * @param (int player) (PLAYER or CPU depending on which map file)
 * @param (int x) (x coordinate of the boards position)
 * @param (int y) (y coordinate of the boards position)
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 *
 * @return (int) (0 if miss, 1 if hit)
 */
int check_hit(int player, int x, int y, int width, int height) 
{
    if (player == CPU) {
        if (!(playerBoard[y][x] == HIT || playerBoard[y][x] == MISS || 
                playerBoard[y][x] == NONE)) {
            // position is a ship
            printf("Hit\n");
            playerBoard[y][x] = HIT;
            player_chars(playerBoard[y][x]);
            return 1;
        } else {
            //position is not a ship
            printf("Miss\n");
            playerBoard[y][x] = MISS;
            player_chars(playerBoard[y][x]);
        }
    } else if (player == PLAYER) {
        if (!(cpuBoard[y][x] == HIT || cpuBoard[y][x] == MISS || 
                cpuBoard[y][x] == NONE)) {
            printf("Hit\n");
            cpuBoard[y][x] = HIT;
            cpu_chars(cpuBoard[y][x]);
            return 1;
        } else {
            printf("Miss\n");
            cpuBoard[y][x] = MISS;
            cpu_chars(cpuBoard[y][x]);
        }
    }
    return 0;
}

/* Reads the players move from stdin and if it is not a bad guess 
 * will then check for hit or miss and check for sunk ship.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void get_player_move(int width, int height)
{
    char c;
    int y;
    char* move;
    printf("(Your move)>");
    move = read_line(stdin);
    if (strlen(move) == 0) {
        error_exit(E_PLAYER_GIVES_UP);
    }
    if (!(check_bad_move(trim_whitespace(move)))) {
        printf("Bad guess\n");
        get_player_move(width, height);
    } else {
        // is a valid input
        sscanf(trim_whitespace(move), "%c%i", &c, &y);
        int x = c - 64;

        if (!(check_bad_guess(x, y, width, height))) {
            printf("Bad guess\n");
            get_player_move(width, height);
        } else if (!check_repeat(PLAYER, x, y)) {
            printf("Repeated guess\n");
            get_player_move(width, height);
        } else {
            // not a bad or repeated guess
            int ship = cpuBoard[y][x];
            check_hit(PLAYER, x, y, width, height);
            check_sunk(PLAYER, ship, width, height);  
        } 
    }
}

/* Reads the cpus move from the turns file and if it is not a bad guess 
 * will then check for hit or miss and check for sunk ship.
 *
 * @param (FILE* turns) (filename of the cpu turns file)
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void get_cpu_move(FILE* turns, int width, int height)
{
    char c;
    int y;
    char* move;
    printf("(CPU move)>");
    move = read_line(turns);
    if (strlen(move) == 0) {
        error_exit(E_CPU_GIVES_UP);
    }
    printf("%s\n", move);
    if (!(check_bad_move(trim_whitespace(move)))) {
        printf("Bad guess\n");
        get_cpu_move(turns, width, height);
    } else {
        // is a valid input 
        sscanf(trim_whitespace(move), "%c%i", &c, &y);
        int x = c - 64;

        if (!(check_bad_guess(x, y, width, height))) {
            printf("Bad guess\n");
            get_cpu_move(turns, width, height);
        } else if (!check_repeat(CPU, x, y)) {
            printf("Repeated guess\n");
            get_cpu_move(turns, width, height);
        } else {
            // not a bad or repeated guess
            int ship = playerBoard[y][x];
            check_hit(CPU, x, y, width, height);
            check_sunk(CPU, ship, width, height);
        } 
    }
}

/* Checks if any of the ships overlap another by checking each position on 
 * the boards status.
 *
 * @param (int player) (PLAYER or CPU depending on which map file)
 * @param (int shipNum) (ship number)
 * @param (int x) (x coordinate of the boards position)
 * @param (int y) (y coordinate of the boards position)
 */
void check_overlap(int player, int shipNum, int x, int y){
    if (player == PLAYER) {
        if (playerBoard[y][x] == HIT || playerBoard[y][x] == MISS || 
                playerBoard[y][x] == NONE) {
            return;
        }
        error_exit(E_PLAYER_SHIP_OVERLAP);
    } else if (player == CPU) {
        if (cpuBoard[y][x] == HIT || cpuBoard[y][x] == MISS || 
                cpuBoard[y][x] == NONE) {
            return;
        }
        error_exit(E_CPU_SHIP_OVERLAP);
    }
}

/* Loops through each ship placing its corresponding number on the board.
 *
 * @param (int player) (PLAYER or CPU depending on which map file)
 * @param (int sizes[]) (array of ship sizes)
 * @param (int shipNum) (ship number)
 * @param (int x) (x coordinate of the boards position)
 * @param (int y) (y coordinate of the boards position)
 * @param (char dir) (direction ship is to be placed in)
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void ship_directions(int player, int sizes[], int shipNum, int x, int y, 
        char dir, int width, int height)
{
    for (int i = 0; i < sizes[shipNum - 1]; i++) {
        check_overlap(player, shipNum, x, y);

        (player == PLAYER) ? player_chars(playerBoard[y][x] = shipNum) 
            : cpu_chars(cpuBoard[y][x] = shipNum);
        switch (dir) {
            case 'N':
                y--;
                break;
            case 'E':
                x++;
                break;
            case 'S':
                y++;
                break;
            case 'W':
                x--;
                break;
            default:
                (player == PLAYER) ? error_exit(E_PLAYER_MAP) : 
                    error_exit(E_CPU_MAP);
                break;
        }
        if (x < 0 || x > width + 1 || y < 0 || y > height + 1) {
            (player == PLAYER) ? error_exit(E_PLAYER_MAP_OOB) : 
                error_exit(E_CPU_MAP_OOB);
        }
    }
}

/* Reads the map files line by line to get the ship location and direction, 
 * checks if the position is bad and checks if the number of ships aligns wi
 * th that in the rules file.
 *
 * @param (FILE* file) (filename of the map file)
 * @param (int player) (PLAYER or CPU depending on which map file)
 * @param (int sizes[]) (array of ship sizes)
 * @param (int num) (number of ships each player has)
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void read_map(FILE* file, int player, int sizes[], int num, int width, 
        int height)
{
    char* line;
    char c, dir;
    int x, y, shipNum = 0;
    char* buffer[10];
    while (line = trim_whitespace(read_line(file)), strlen(line) != 0) {
        int i = 0;
        char* token = strtok(line, " ");

        while (token != NULL) {
            buffer[i++] = token;
            token = strtok(NULL, " ");
        }
        if (i > 2) {
            (player == PLAYER) ? error_exit(E_PLAYER_MAP) : 
                error_exit(E_CPU_MAP);
        } else {
            char* move = trim_whitespace(buffer[0]);
            char* direction = trim_whitespace(buffer[1]);

            if (!(check_bad_move(move))) {
                (player = PLAYER) ? error_exit(E_PLAYER_MAP) : 
                    error_exit(E_CPU_MAP);
            } else {
                // is a valid move
                sscanf(move, "%c%i", &c, &y);
                x = c - 64;
                if (check_bad_guess(x, y, width, height)) {
                    // isnt out of bounds
                    if (strlen(trim_whitespace(direction)) == 1) {
                        // is a valid direction
                        sscanf(direction, "%c", &dir);
                        shipNum++;
                        ship_directions(player, sizes, shipNum, x, y, dir, 
                                width, height);
                    }
                }
            }
        }    
    }
    if (shipNum != num) {
        (player == PLAYER) ? error_exit(E_PLAYER_MAP) : 
            error_exit(E_CPU_MAP);
    }
}

/* Displays the cpu board after each player and cpu move with updated 
 * statues.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void display_cpu_board(int width, int height)
{
    printf("   ");
    for (char c = 65; c <= (width + 64); c++) {
        printf("%c", c);
    }
    printf("\n");

    for (int i = 1; i < (height + 1); i++) {
        if (i < 10) {
            printf(" %i ", i);
        } else {
            printf("%i ", i);
        }
        for (int j = 1; j < (width + 1); j++) {
            printf("%c", cpu_chars(cpuBoard[i][j]));
        }
        printf("\n");
    }
}

/* Displays the player board after each player and cpu move with updated 
 * statues.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 */
void display_player_board(int width, int height)
{
    printf("===\n   ");
    for (char c = 65; c <= (width + 64); c++) {
        printf("%c", c);
    }
    printf("\n");

    for (int i = 1; i < (height + 1); i++) {
        if (i < 10) {
            printf(" %i ", i);
        } else {
            printf("%i ", i);
        }
        for (int j = 1; j < (width + 1); j++) {
            printf("%c", player_chars(playerBoard[i][j]));
        }
        printf("\n");
    }
}

/* Checks if cpu has won the game by sinking all the cpu ships.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 *
 * @return (int) (1 if cpu has won, 0 otherwise)
 */
int check_cpu_win(int width, int height)
{
    for (int i = 1; i < (height + 1); i++) {
        for (int j = 1; j < (width + 1); j++) {
            if (!(playerBoard[i][j] == HIT || playerBoard[i][j] == MISS || 
                    playerBoard[i][j] == NONE)) {
                return 0;
            }
        }
    }
    return 1;
}

/* Checks if player has won the game by sinking all the cpu ships.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 *
 * @return (int) (1 if player has won, 0 otherwise)
 */
int check_player_win(int width, int height) 
{
    for (int i = 1; i < (height + 1); i++) {
        for (int j = 1; j < (width + 1); j++) {
            if (!(cpuBoard[i][j] == HIT || cpuBoard[i][j] == MISS || 
                    cpuBoard[i][j] == NONE)) {
                return 0;
            }
        }
    }
    return 1;
}

/* Checks if either player has won the game and returns 0 if not.
 *
 * @param (int width) (width of the board)
 * @param (int height) (height of the board)
 *
 * @return (int) (0 if game is still going, 1 if otherwise)
 */
int check_win(int width, int height)
{
    if (check_player_win(width, height)) {
        printf("Game over - you win\n");
        return 1;
    } else if (check_cpu_win(width, height)) {
        printf("Game over - you lose\n");
        return 1;
    }
    return 0;
}

/* Takes in filenames and opens them, initialises gameplay and loops until 
 * the game has been won or an error occurs.
 *
 * @param (int argc) (number of arguments)
 * @param (char* argv[]) (array of argument strings)
 *
 * @return (int) (0 if game exits normally)
 */
int main(int argc, char* argv[])
{
    if (argc < 5) {
        error_exit(E_NOT_ENOUGH_PARAMETERS);
    }
    int boardWidth, boardHeight, numShips;
    int shipSizes[numShips];

    FILE* rulesFile = fopen(argv[1], "r");
    FILE* playerMapFile = fopen(argv[2], "r");
    FILE* cpuMapFile = fopen(argv[3], "r");
    FILE* turnsFile = fopen(argv[4], "r"); 

    check_null_files(rulesFile, playerMapFile, cpuMapFile, turnsFile);
    sscanf(read_line(rulesFile), "%i %i", &boardWidth, &boardHeight);
    sscanf(read_line(rulesFile), "%i ", &numShips);
    
    for (int i = 0; i < numShips; i++) {
        sscanf(read_line(rulesFile), "%i", &shipSizes[i]);
    }
    if ((boardWidth < 0) || (boardHeight < 0) || (boardWidth > MAX) || 
            (boardHeight > MAX) || (numShips > 15)) {
        error_exit(E_RULES);
    } 

    initialise_cpu_board(boardWidth, boardHeight);
    initialise_player_board(boardWidth, boardHeight);

    read_map(playerMapFile, PLAYER, shipSizes, numShips, boardWidth, 
            boardHeight);
    read_map(cpuMapFile, CPU, shipSizes, numShips, boardWidth, 
            boardHeight);

    while (!check_win(boardWidth, boardHeight)) {
        display_cpu_board(boardWidth, boardHeight);
        display_player_board(boardWidth, boardHeight);
        get_player_move(boardWidth, boardHeight);
        if (check_win(boardWidth, boardHeight)) {
            break;
        }        
        get_cpu_move(turnsFile, boardWidth, boardHeight);
        if (check_win(boardWidth, boardHeight)) {
            return 0;
        }    
    }
    
    close_files(rulesFile, playerMapFile, cpuMapFile, turnsFile);
    return 0;
}

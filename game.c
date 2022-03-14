#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EVASION_NUM 101  //the max evasion rate of the monster

void start_game(int *r, int *c, int **map, int *p_blood, int *p_strength, int *player_r, int *player_c);
int **init_game(int *r, int *c, int **map);
bool player_action(int *r, int *c, int **map, int *player_r, int *player_c, int *p_blood, int *p_strength);
int **create_map(int *r, int *c, int **map);
void print_map(int *r, int *c, int **map);
void setup_village(int *r, int *c, int **map);
void setup_trap(int *r, int *c, int **map);
void setup_monster(int *r, int *c, int **map);
void setup_gun(int *r, int *c, int **map);
bool check_boundary(int inp_row, int inp_col, int *r, int *c);
bool check_availability(int inp_row, int inp_col, int **map);
void setup_player(int *r, int *c, int *player_r, int *player_c, int **map);
bool check_move(int x, int y, int *r, int *c, int *blood, int *strength, int **map);
bool player_move(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map);
bool go_up(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map);
bool go_down(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map);
bool go_left(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map);
bool go_right(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map);
void visit_village(int *blood, int *strength);
void encounter_gun(int *strength, int gun_r, int gun_c, int **map);
void encounter_trap(int *blood);
bool fight_monster(int *blood, int *strength);
bool battle_result(int *blood, int *strength);
bool evasion(int evasionRate, int *blood);
void renew_monster_location(int x, int y, int *r, int *c, int **map);

int main() {
    int row, col, *r = &row, *c = &col;   //map size
    int **map;    //2D array map
    int p_row, p_col, *player_r = &p_row, *player_c = &p_col;  //player position
    int blood = 10, strength = 2, *p_blood = &blood, *p_strength = &strength;  //player's blood & strength value
    start_game(r, c, map, p_blood, p_strength, player_r, player_c);
    return 0;
}
/**********************************************************
 * Function: start_game()
 * 
 *   where the game starts, includes:
 *   (1) initializes the game with init_game()
 *   (2) lets the player move anywhere with player_action()
 * ********************************************************/
void start_game(int *r, int *c, int **map, int *p_blood, int *p_strength, int *player_r, int *player_c) {
    while(true) {
        printf("Welcome, Adventurer!\n");
        map = init_game(r, c, map);
        print_map(r, c, map);
        printf("[1] Create a new map [2] Start travel the map: ");
        int action;
        scanf("%d", &action);
        if(action == 2) {
            //判斷遊戲是否終止
            if(!player_action(r, c, map, player_r, player_c, p_blood, p_strength)) {
                break;
            }
        }
        free(map);  //release the memory of the map
    }
}

/**********************************************************************
 * Function: init_game()
 * 
 *   initializes the game, includes:
 *   (1) sets up the size of the map with create_map()
 *   (2) sets up village location with setup_village()
 *   (3) sets up the number & locations of traps with setup_trap()
 *   (4) sets up the number & locations of monsters with setup_monster()
 *   (5) sets up the number & locations of guns with setup_gun()
 * 
 *   returns: the initialized 2D array map
 * ********************************************************************/
int **init_game(int *r, int *c, int **map) {
    map = create_map(r, c, map);
    setup_village(r, c, map);
    setup_trap(r, c, map);
    setup_monster(r, c, map);
    setup_gun(r, c, map);
    return map;
}

/*******************************************************************************************
 * Function: create_map()
 * 
 *   constructs the map, includes:
 *   (1) allocates the memory for 1000*1000 2D array map
 *   (2) initializes the element in 2D array according to the input of the map's row & column
 * 
 *   returns: the initialized 2D array
 * ****************************************************************************************/
int **create_map(int *r, int *c, int **map) {
    printf("Input the number of row and column for the map:\n");
    scanf("%d %d", r, c);
    map = (int **)malloc(1000*sizeof(int *));
    for(int i = 0; i < 1000; i++) {
        map[i] = (int *)malloc(1000*sizeof(int));
    }
    //初始化陣列，以免有些值跟village或monster代表數值搞混
    for(int i = 0; i < *r; i++) {
        for(int j = 0; j < *c; j++) {
            map[i][j] = 0;
        }
    }
    return map;
}

/************************************************
 * Funciton: setup_village()
 *   
 *   sets up the location for the village randomly
 *   "1" represents the village in 2D array map
 * **********************************************/
void setup_village(int *r, int *c, int **map) {
    int v_row, v_col;
    srand((unsigned)time(NULL));
    v_row = rand()%(*r);
    v_col = rand()%(*c);
    map[v_row][v_col] = 1; 
}

/**************************************************************************
 * Funciton: setup_monster()
 *   
 *   sets up the location for the monsters randomly
 *   "2" represents the monster in 2D array map
 *   
 *   variables: monster_num: the number of the monsters in map
 *              seed: to generate random numbers for the monsters' location
 *              m_row: the row of the monster location
 *              m_col: the column of the monster location
 * ************************************************************************/
void setup_monster(int *r, int *c, int **map) {
    int monster_num = (*r/10 + *r%10);
    int seed = (unsigned)time(NULL);
    for(int i = 0; i < monster_num;) {
        int m_row, m_col;
        seed *= 3;
        srand(seed);
        m_row = rand()%(*r);
        m_col = rand()%(*c);
        //判斷地圖上那個位置有無被佔 & monster位置有無超出地圖
        if(map[m_row][m_col] == 0) {
            map[m_row][m_col] = 2;
            i++;
        }
    }
}

/**********************************************************************
 * Funciton: setup_gun()
 *   
 *   sets up the location for the guns randomly
 *   "4" represents the guns in 2D array map
 * 
 *   variables: gun_num: the number of the guns in map
 *              seed: to generate random numbers for the guns' location
 * ********************************************************************/
void setup_gun(int *r, int *c, int **map) {
    //gun的數目為monster_num/3
    int gun_num = (*r/10 + *r%10)/3;
    //至少會有一把gun
    if(gun_num == 0) {
        gun_num = 1;
    } 
    int seed = (unsigned)time(NULL);
    for(int i = 0; i < gun_num;) {
        int g_row, g_col;
        seed *= 2;
        srand(seed);
        g_row = rand()%(*r);
        g_col = rand()%(*c);
        //判斷地圖上那個位置有無被佔
        if(map[g_row][g_col] == 0) {
            map[g_row][g_col] = 4;
            i++;
        }
    }
}

/***********************************************************************
 * Funciton: setup_trap()
 *   
 *   sets up the location for the traps randomly
 *   "5" represents the traps in 2D array map
 * 
 *   variables: trap_num: the number of the traps in map
 *              seed: to generate random numbers for the traps' location
 * *********************************************************************/
void setup_trap(int *r, int *c, int **map) {
    //trap的數目為monster_num/2
    int trap_num = (*r/10 + *r%10)/2;

    //trap的位置隨機
    int seed = (unsigned)time(NULL);
    int trap_r, trap_c;

    for(int i = 0; i < trap_num;) {
        seed *= 2;
        srand(seed);
        trap_r = rand()%(*r);
        trap_c = rand()%(*c);
        //判斷地圖上那個位置有無被佔
        if(map[trap_r][trap_c] == 0 ) {
            map[trap_r][trap_c] = 5;
            i++;
        }
    }
}

/********************************************************************************
 * Function: print_map()
 *   
 *   prints the display of the map,
 *   includes the positions of the village, monsters, guns, traps, and the player
 * ******************************************************************************/
void print_map(int *r, int *c, int **map) {
    printf("=== MAP ===\n");
    for(int i = 0; i < *r; i++) {
        for(int j = 0; j < *c; j++) {
            //畫village
            if(map[i][j] == 1) {
                printf("v");
            }
            //畫monster
            else if(map[i][j] == 2) {
                printf("m");
            }
            //畫player
            else if(map[i][j] == 3) {
                printf("p");
            }
            //畫gun
            else if(map[i][j] == 4) {
                printf("g");
            }
            //將trap畫為empty，這樣player掉下去才有驚喜感
            else if(map[i][j] == 5) {
                printf(".");
            }
            //player與village(6) or trap(7)位置重疊
            else if(map[i][j] == 6 || map[i][j] == 7) {
                printf("p");
            }
            //畫空的位置
            else if(map[i][j] == 0) {
                printf(".");
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("=== MAP ===\n");
}

/********************************************************************
 * Function: player_action()
 * 
 *   it's about the player's action, includes:
 *   (1) sets up for the player's location
 *   (2) asks player to visit the village before traveling on the map
 *   (3) player's moving
 * 
 *   returns: whether the game is exit (false) or not (true)
 * ******************************************************************/
bool player_action(int *r, int *c, int **map, int *player_r, int *player_c, int *p_blood, int *p_strength) {
    setup_player(r, c, player_r, player_c, map);
    print_map(r, c, map);
    visit_village(p_blood, p_strength);
    print_map(r, c, map);
    //如果血量 < 0 或 玩家要離開遊戲，則return false，讓遊戲終止
    if(player_move(r, c, player_r, player_c, p_blood, p_strength, map)) {
        return false;
    }
}

/**************************************************************************************************
 * Funciton: setup_player()
 *   
 *   lets the player set up the location for himself
 *   "3" represents the player in 2D array map
 * 
 *   variables: valid_player_position: check whether the input position is empty and inside the map
 * ************************************************************************************************/
void setup_player(int *r, int *c, int *player_r, int *player_c, int **map) {
    bool valid_player_position = true;
    while(valid_player_position) {
        printf("Input the row and column for player:\n");
        scanf("%d %d", player_r, player_c);
        if(!check_boundary(*player_r, *player_c, r, c) && check_availability(*player_r, *player_c, map)) {
            map[*player_r][*player_c] = 3;
            valid_player_position = false;
        }
    }
}

/*************************************************************************
 * Funciton: check_boundary()
 *   
 *   checks whether the input position is outside the map 
 * 
 *   variables: inp_row: input row
 *              inp_col: input column
 * 
 *   returns: whether the position is outside the map (true) or not (false)
 * ***********************************************************************/
bool check_boundary(int inp_row, int inp_col, int *r, int *c) {
    if(inp_row >= *r || inp_col >= *c || inp_row < 0 || inp_col < 0) {
        printf("the location is outside the map\n");
        return true;
    }
    else {
        return false;
    }
}

/*************************************************************
 * Funciton: check_availability()
 *   
 *   checks whether the input location is occupied by others 
 * 
 *   variables: inp_row: input row
 *              inp_col: input column
 * 
 *   returns: the location is occupied (false) / empty (true)
 * ***********************************************************/
bool check_availability(int inp_row, int inp_col, int **map) {
    if(map[inp_row][inp_col] == 0) {
        return true;
    }
    else {
        printf("the location is occupied\n");
        return false;
    }
}

/*********************************************************************************************
 * Function: player_move()
 *   
 *   manipulates the player's move, includes:
 *   (1) reads the direction from input and calls the corresponding function to move the player
 *   (2) prints the display of the map after each move
 *   (3) checks whether the game is over each round
 * 
 *   variables: flag: records whether the player wants to leave the game (0~1)
 *              go_direction: stores the direction from input that player wants to go (1~5)
 * 
 *   returns: whether the game is over (true)
 * *******************************************************************************************/
bool player_move(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map) {
    int flag = 0;
    while(true) {
        int go_direction;
        printf("You can choose anywhere to go:\n[1] go up [2] go down [3] go right [4] go left [5] exit: ");
        scanf("%d", &go_direction);
        switch(go_direction) {
            case 1:
                if(go_up(r, c, player_r, player_c, blood, strength, map)) {
                    print_map(r, c, map);
                }
                break;
            case 2:
                if(go_down(r, c, player_r, player_c, blood, strength, map)) {
                    print_map(r, c, map);
                }
                break;
            case 3:
                if(go_right(r, c, player_r, player_c, blood, strength, map)) {
                    print_map(r, c, map);
                }
                break;
            case 4:
                if(go_left(r, c, player_r, player_c, blood, strength, map)) {
                    print_map(r, c, map);
                }
                break;
            default:
                //player選擇離開遊戲
                flag = 1;
                break;
        }
        //若player主動離開遊戲or血量低於0，則遊戲結束
        if(flag || *blood <= 0) {
            if(*blood <= 0) {
                printf("You dead. Bye!\n");
            }
            return true;
        }
    }
}

/*********************************************************************************************************
 * Function: go_up()
 * 
 *   (1) moves the player one step upward
 *   (2) checks whether the moved position triggers any events with check_move() 
 *   (3) checks whether the previous position is right on the trap/village, if so, restores thier location
 * 
 *   variables: player_r: the pointer that points to player's current row
 *              player_c: the pointer that points to player's current column
 *   
 *   returns: whether the player can move to the next step (true) or not (false)
 * *******************************************************************************************************/
bool go_up(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map) {
    *player_r -= 1;
    if(!check_move(*player_r, *player_c, r, c, blood, strength, map)) {
        *player_r += 1;
        return false;
    }
    else {
        //player移動後的位置並未與trap/village重疊，則將player移動後位置更新在地圖上
        if(map[*player_r][*player_c] != 6 && map[*player_r][*player_c] != 7) {
            map[*player_r][*player_c] = 3;
        }
        //若原位置player與village重疊，則還原village
        if(map[*player_r+1][*player_c] == 6) {
            map[*player_r+1][*player_c] = 1;
        }
        //若原位置player與trap重疊，則還原trap
        else if(map[*player_r+1][*player_c] == 7) {
            map[*player_r+1][*player_c] = 5;
        }
        //否則，清空原位置
        else {
            map[*player_r+1][*player_c] = 0;
            
        }
        return true;
    }
}

/*********************************************************************************************************
 * Function: go_down()
 * 
 *   (1) moves the player one step downward
 *   (2) checks whether the moved position triggers any events with check_move() 
 *   (3) checks whether the previous position is right on the trap/village, if so, restores thier location
 * 
 *   variables: player_r: the pointer that points to player's current row
 *              player_c: the pointer that points to player's current column
 *   
 *   returns: whether the player can move to the next step (true) or not (false)
 * *******************************************************************************************************/
bool go_down(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map) {
    *player_r += 1;
    if(!check_move(*player_r, *player_c, r, c, blood, strength, map)) {
        *player_r -= 1;
        return false;
    }
    else {
        //player移動後的位置並未與trap/village重疊，則將player移動後位置更新在地圖上
        if(map[*player_r][*player_c] != 6 && map[*player_r][*player_c] != 7) {
            map[*player_r][*player_c] = 3;
        }
        //若原位置player與village重疊，則還原village
        if(map[*player_r-1][*player_c] == 6) {
            map[*player_r-1][*player_c] = 1;
        }
        //若原位置player與trap重疊，則還原trap
        else if(map[*player_r-1][*player_c] == 7) {
            map[*player_r-1][*player_c] = 5;
        }
        //否則，清空原位置
        else {
            map[*player_r-1][*player_c] = 0;
            
        }
        return true;
    }
}

/*********************************************************************************************************
 * Function: go_left()
 * 
 *   (1) moves the player one step leftward
 *   (2) checks whether the moved position triggers any events with check_move() 
 *   (3) checks whether the previous position is right on the trap/village, if so, restores thier location
 * 
 *   variables: player_r: the pointer that points to player's current row
 *              player_c: the pointer that points to player's current column
 *   
 *   returns: whether the player can move to the next step (true) or not (false)
 * *******************************************************************************************************/
bool go_left(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map) {
    *player_c -= 1;
    if(!check_move(*player_r, *player_c, r, c, blood, strength, map)) {
        *player_c += 1;
        return false;
    }
    else {
        //player移動後的位置並未與trap/village重疊，則將player移動後位置更新在地圖上
        if(map[*player_r][*player_c] != 6 && map[*player_r][*player_c] != 7) {
            map[*player_r][*player_c] = 3;
        }
        //若原位置player與village重疊，則還原village
        if(map[*player_r][*player_c+1] == 6) {
            map[*player_r][*player_c+1] = 1;
        }
        //若原位置player與trap重疊，則還原trap
        else if(map[*player_r][*player_c+1] == 7) {
            map[*player_r][*player_c+1] = 5;
        }
        //否則，清空原位置
        else {
            map[*player_r][*player_c+1] = 0;
            
        }
        return true;
    }
}

/*********************************************************************************************************
 * Function: go_right()
 * 
 *   (1) moves the player one step rightward
 *   (2) checks whether the moved position triggers any events with check_move() 
 *   (3) checks whether the previous position is right on the trap/village, if so, restores thier location
 *   
 *   variables: player_r: the pointer that points to player's current row
 *              player_c: the pointer that points to player's current column
 *   
 *   returns: whether the player can move to the next step (true) or not (false)
 * *******************************************************************************************************/
bool go_right(int *r, int *c, int *player_r, int *player_c, int *blood, int *strength, int **map) {
    *player_c += 1;
    if(!check_move(*player_r, *player_c, r, c, blood, strength, map)) {
        *player_c -= 1;
        return false;
    }
    else {
        //player移動後的位置並未與trap/village重疊，則將player移動後位置更新在地圖上
        if(map[*player_r][*player_c] != 6 && map[*player_r][*player_c] != 7) {
            map[*player_r][*player_c] = 3;
        }
        //若原位置player與village重疊，則還原village
        if(map[*player_r][*player_c-1] == 6) {
            map[*player_r][*player_c-1] = 1;
        }
        //若原位置player與trap重疊，則還原trap
        else if(map[*player_r][*player_c-1] == 7) {
            map[*player_r][*player_c-1] = 5;
        }
        //否則，清空原位置
        else {
            map[*player_r][*player_c-1] = 0;
            
        }
        return true;
    }
}

/**********************************************************************************************************
 * Function: check_move()
 * 
 *   (1) checks the player's moved position whether triggers any events => calls the corresponding function
 *       events include: visiting village, encountering monster, picking up gun, falling into a trap 
 *   (2) checks whether the moved position beyond boudaries
 *   
 *   returns: whether the player can move to the new position (true) or not (false)
 * *******************************************************************************************************/
bool check_move(int x, int y, int *r, int *c, int *blood, int *strength, int **map) {
    //遇到monster
    if(map[x][y] == 2) {
        //打輸，不改變monster&player位置
        if(fight_monster(blood, strength)){
            print_map(r, c, map);
            return false;
        }
        //打贏，player佔據monster位置，monster重新指派新位址
        else {
            renew_monster_location(x, y, r, c, map);
            return true;
        }
    }
    //造訪village
    else if(map[x][y] == 1) {
        visit_village(blood, strength);
        //表village跟player位置重疊(6)
        map[x][y] = 6;
        return true;
    }
    //遇到gun可撿
    else if(map[x][y] == 4) {
        encounter_gun(strength, x, y, map);
        return true;
    }
    //掉入陷阱
    else if(map[x][y] == 5) {
        encounter_trap(blood);
        //表trap跟player位置重疊(7)
        map[x][y] = 7;
        return true;
    }
    else if(x >= *r || y >= *c || x < 0 || y < 0) {
        printf("you can't go there\n");
        return false;
    }
    else {
        return true;
    }
}

/**********************************************************************************
 * Function: visit_village()
 *  
 *   actions that player does in the village, includes:
 *   (1) buys a magic sword (strength++)
 *   (2) go to the bar (blood++)
 *   (3) leave the village
 * 
 *   variables: visit: whether the player wants to visit the village
 *              action_in_village: the action player chooses to do
 *              buy_sword: record whether player has bought the sword in this round
 * ********************************************************************************/
void visit_village(int *blood, int *strength) {
    int visit, action_in_village;
    // haven't bought sword yet
    int buy_sword = 0;
    printf("Do you want to go to our village first (1: Yes; 0: No)? ");
    scanf("%d", &visit);
    if(visit != 1) {
        return;
    }
    do {
        printf("What do you want to do in our village (1: buy a magical sword; 2: go to the bar; 3: leave the village)? ");
        scanf("%d", &action_in_village);
        switch(action_in_village) { 
            // buy sword get one strength in first round
            case 1:
                if(!buy_sword) {
                    (*strength)++;
                    buy_sword = 1;
                    printf("Nice sword! Now you have %d strength!\n", *strength);
                }
                else {
                    printf("You already bought a sword.\n");
                }
                break;
            // drink beer get one blood if blood isn't full
            case 2:
                if(*blood < 10) {
                    (*blood)++;
                    printf("Nice beer! Now you have %d blood!\n", *blood); 
                }
                else {
                    printf("Your blood is full. You don't need a beer.\n");
                }
                break;
            // leave the village
            case 3:
                printf("You are welcome to come back anytime!\n");
                break;
            // wrong input
            default:
                printf("Invalid action! 1: buy a magical sword; 2: go to the bar; 3: leave the village\n");
                break;
        }  
    } while(action_in_village != 3);
}

/******************************************************
 * Function: encounter_gun()
 * 
 *   the player gets a gun, and gains one strength
 * 
 *   variables: gun_r: the row of the gun's location
 *              gun_c: the column of the gun's location
 * ****************************************************/
void encounter_gun(int *strength, int gun_r, int gun_c, int **map) {
    (*strength)++;
    //取完gun則將gun從地圖中刪除
    map[gun_r][gun_c] = 0;
    printf("You get a gun! Now you have %d strength.\n", *strength);
}

/******************************************************
 * Function: encounter_trap()
 * 
 *   the player falls into a trap, and losses one blood
 * ***************************************************/
void encounter_trap(int *blood) {
    (*blood)--;
    printf("Oh no! You fall into a trap :( (Now you have %d blood)\n", *blood);
}

/**********************************************************************************
 * Function: fight_monster()
 * 
 *   (1) lets the player choose to fight which level's monster
 *   (2) calculates the strength & blood of the player 
 *   
 *   variables: monster_level: the level of the monster player wants to fight (1~4)
 * 
 *   returns: whether the player wins (false) / loses (true) the fight
 * ********************************************************************************/
bool fight_monster(int *blood, int *strength) {
    int monster_level;
    printf("You have %d blood and %d strength.\n", *blood, *strength);
    printf("You meet monster!! You have to choose one monster to fight(1 to 4): ");
    scanf("%d", &monster_level);
    //怪物等級輸入有誤
    if(monster_level > 4 || monster_level < 1) {
        printf("No such monster, please enter 1 to 4 to choose the monster.\n");
        return true;
    }
    //打怪，並計算剩餘血量&力量
    else {
        printf("You are fighting Monster %d!\n", monster_level);
        if(*strength > monster_level) {
            //玩家有足夠多力量打怪物，判斷怪物是否被打死
            if(battle_result(blood, strength)) {
                printf("You Win!\n");
                (*strength)++;
                return false;
            }
        }
        else {
            *blood -= monster_level;
            if(*blood > 0) {
                printf("You Lose!\n");
            }
        }
        //表示打輸，或player血量被扣到0
        return true;
    }
}

/********************************************************************************
 * Function: battle_result()
 *   
 *   gets the battle result of the fight
 * 
 *   variables: evasionRate: monster's probability of skipping the attack (0~100)
 *              seed: generate the random number for evasionRate
 *   
 *   returns: whether the player wins (true) or not (false)
 * *****************************************************************************/
bool battle_result(int *blood, int *strength) {
    int evasionRate;
    int seed = (unsigned) time(NULL);
    do {
        seed *= 2;
        srand(seed);
        evasionRate = rand() % MAX_EVASION_NUM;
        printf("Monster's evasion rate = %d\n", evasionRate);
    } while(evasion(evasionRate, blood) && (*blood > 0));
    //怪物持續閃避成功，玩家被怪物閃避後的攻擊打死
    if(*blood == 0) {
        return false;
    }
    //怪物閃避失敗，玩家贏
    return true;
}

/*********************************************************************************
 * Function: evasion()
 * 
 *   checks whether the monster dodges the attack successfully => player's blood--
 *   the probability is 50%
 * 
 *   returns: whether the monster dodges successfully (true) / or not (false)
 * ******************************************************************************/
bool evasion(int evasionRate, int *blood) {
    //怪物閃避成功，則扣玩家一血量
    if(evasionRate > 50) {
        (*blood)--;
        printf("Monster dodge successfully, you loss 1 blood(you have %d blood)\n", *blood);
        return true;
    }
    //怪物閃避失敗
    printf("You attack the monster!\n");
    return false;
}

/*****************************************************************
 * Function: renew_monster_location()
 * 
 *   resets the monster's location if the player wins the fight
 * 
 *   variables: seed: generates the location for monster randomly
 *              monster_r: the row of the monster's location
 *              monster_c: the column of the monster's location
 * **************************************************************/
void renew_monster_location(int x, int y, int *r, int *c, int **map) {
    int seed = (unsigned)time(NULL);
    int monster_r, monster_c;
    do {
        seed *= 2;
        srand(seed);
        monster_r = rand()%(*r);
        monster_c = rand()%(*c);
        //monster位址不變，重新指派
        if(monster_r == x && monster_c == y) {
            continue;
        }
    } while(map[monster_r][monster_c] != 0);
    map[monster_r][monster_c] = 2;
}
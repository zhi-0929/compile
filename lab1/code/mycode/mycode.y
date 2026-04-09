%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>

    int curr_x, curr_y, treasure_x, treasure_y; //机器人位置和宝藏位置
    int treasure_value; //宝藏价值
    int sum; //总价值
    int direction; //当前方向
    char *direction_c[] = {"北", "东", "南", "西"};
    int move_x[4] = {0, 1, 0, -1};
    int move_y[4] = {1, 0, -1, 0};

    void yyerror(const char *s);
    int yylex();
    int randint(int , int );
%}
%token LEFT RIGHT UP DOWN FORWARD BACK QUERY NUMBER OVER
%start robot_program

%%
robot_program :
    instruction_list
    ;

instruction_list :
    instruction
    |  instruction_list instruction
    ;

instruction :
    move OVER
    | turn OVER
    | take OVER
    | QUERY OVER {printf("当前位置:(%d,%d)，当前朝向: %s， 宝藏位置:(%d,%d)\n",
            curr_x, curr_y, direction_c[direction], treasure_x,treasure_y);}
    | OVER
    ;

move:
    FORWARD {
            curr_x += move_x[direction]; curr_y += move_y[direction];
            printf("前进1步，当前位置:(%d, %d)\n", curr_x, curr_y);
        }
    | FORWARD NUMBER {
            curr_x += $2 * move_x[direction]; curr_y += $2 * move_y[direction];
            printf("前进%d步，当前位置:(%d, %d)\n", $2, curr_x, curr_y);
        }
    | BACK {
            curr_x -= move_x[direction]; curr_y -= move_y[direction];
            printf("后退1步，当前位置:(%d, %d)\n", curr_x, curr_y);
        }
    | BACK NUMBER {
            curr_x -= $2 * move_x[direction]; curr_y -= $2 * move_y[direction];
            printf("后退%d步，当前位置:(%d, %d)\n", $2, curr_x, curr_y);
        }
    ;

turn :
    LEFT {
            direction -= 1; direction += 4; direction %= 4;
            printf("左转1次，当前朝向:%s\n", direction_c[direction]);
        }
    | LEFT NUMBER {
            direction -= $2; direction %= 4; direction += 4; direction %= 4;
            printf("左转%d次，当前朝向:%s\n", $2, direction_c[direction]);
        }
    | RIGHT {
            direction += 1; direction %= 4;
            printf("右转1次，当前朝向:%s\n", direction_c[direction]);
        }
    | RIGHT NUMBER {
            direction += $2; direction %= 4;
            printf("右转%d次，当前朝向:%s\n", $2, direction_c[direction]);
        }
    ;

take :
    DOWN UP {
            if (curr_x == treasure_x && curr_y == treasure_y) {
                sum += treasure_value;
                printf("获得价值为%d的宝藏，总共获得价值为%d的宝藏\n", treasure_value, sum);
                treasure_x = randint(-10, 10), treasure_y = randint(-10, 10);
                treasure_value = randint(1, 100);
                printf("宝藏位置:(%d,%d)\n", treasure_x, treasure_y);
            } else {
                printf("此地无宝藏\n");
            }
        }
    ;

%%
void yyerror(const char *s) {
    fprintf(stderr,"错误:%s\n",s);
}

int randint(int l, int r) {//生成[l, r]的随机数
    int t = rand() % (r - l + 1);
    return l + t;
}

int main() {
    srand(time(NULL));

    curr_x = randint(-10, 10), curr_y = randint(-10, 10);
    treasure_x = randint(-10, 10), treasure_y = randint(-10, 10);
    treasure_value = randint(1, 100);
    sum = direction = 0;

    printf("机器人系统启动\n");
    printf("当前位置:(%d,%d)，当前朝向: %s， 宝藏位置:(%d,%d)\n",
            curr_x, curr_y, direction_c[direction], treasure_x,treasure_y);

    yyparse();
    return 0;
}
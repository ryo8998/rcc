#include "rcc.h"


/* ---------------- グローバル変数  -------------------*/
Token *token;

/* ---------------- 関数  -------------------*/

static bool is_ident1(char c){
    //最初の文字がアルファベットか_なら識別子
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_ident2(char c){
    //識別子の2文字目以降はident1か数字
    return is_ident1(c) || ('0' <= c && c <= '9');
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        //空白文字をスキップ
        if(isspace(*p)){
            // printf("space, %d\n",counter);
            p++;
            continue;
        }

        if(!strncmp(p, ">=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, "==", 2) || !strncmp(p, "!=", 2)){
            // printf("two %d\n",counter);
            cur = new_token(TK_RESERVED, cur, p,2);
            p+=2;
            continue;
        }

        if(strncmp(p, "return", 6) == 0 && !is_ident2(p[6])) {
            cur = new_token(TK_RET, cur, p, 6);
            p+=6;
            continue;
        }

        if(strncmp(p,"if", 2) == 0){
            cur = new_token(TK_IF, cur, p, 2);
            p+=2;
            continue;
        }

        if(is_ident1(*p)){
            char *start = p;

            do{
                p++;
            }while(is_ident2(*p));
            //何文字進んだかを把握する
            int len = p - start;
            cur = new_token(TK_IDENT, cur, start, len);
            continue;
        }

        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';'){
            // printf("one %d\n",counter);
            cur = new_token(TK_RESERVED, cur, p++,1);
            continue;
        }

        if(isdigit(*p)){
            // printf("num %d\n",counter);
            cur = new_token(TK_NUM, cur, p,0);
            char *q = p;
            cur->val = strtol(p,&p, 10);
            cur->len = p - q; //進んだポインタの差がlenになる
            continue;
        }
        error_at(p,"トークナイズできません");      
    }
    new_token(TK_EOF, cur, p,0);
    return head.next;
}

void print_token(){
    Token *iterator = token;
    while(iterator){
        printf("%d -> ",iterator->kind);
        iterator=iterator->next;
    }
}

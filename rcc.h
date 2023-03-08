#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- Tyepdef宣言  -------------------*/

// トークンの種類
typedef enum{
    TK_RESERVED,    //記号
    TK_NUM,         //整数トークン
    TK_EOF,         //入力の終わりを表すトークン
} TokenKind;

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_EQUAL, // ==
    ND_NOT_EQUAL, // !=
    ND_SMALLER, // <
    ND_SMALLER_OR_EQUAL, // <=
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    int val;   // kindがND_NUMの場合のみ使う
};


typedef struct Token Token;

// トークン型
struct Token{
    TokenKind kind;     //トークンの型
    Token *next;        //次の入力トークン
    int val;            //kindがTK_NUMの場合、その数値
    char *str;          //トークン文字列
    int len;            //トークンの長さ
};

/* ----------------グローバル変数-----------------*/
extern Token *token;
extern char* user_input;

/* ----------------関数プロトタイプ宣言-----------------*/
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *primary();
Node *expr();
Node *mul();
Node *unary();
Node *relational();
Node *add();
Node *equality();
Token *tokenize(char *p);

void gen(Node *node);
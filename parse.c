#include "rcc.h"

// ローカル変数を保存する連結リストの先頭
LVar *locals = NULL;

// エラーを報告するための関数
// printfと同じ引数を取る
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s",pos, " "); //pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す

bool consume(char *op){
    // printf("token->len = %d, %d\n",(int)strlen(op),token->len);
    if(token->kind != TK_RESERVED || (int)strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    // printf("true %s\n", op);
    token = token->next;
    return true;
}

Token *consume_ident(){ //正直この関数の必要性がわかっていない・・・
    if(token->kind == TK_IDENT){
        Token *ret = token;
        token = token->next;
        return ret;
    }
    return NULL;
}

bool consume_return(){
    if(token->kind != TK_RET){
        return false;
    }
    token = token->next;
    return true;
}

bool consume_if(){
    if(token->kind != TK_IF){
        return false;
    }
    token = token->next;
    return true;
}

// 次のトークガン期待している記号のときには、トークンを1つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        error_at(token->str, "'%s’ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
// それ以外の場合にはエラーを報告する
int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind =kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1,sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *code[100]; //複数のノードを保存するための配列（ノードのポインタを保存）

void program(){
    int i = 0;
    while(!at_eof()){
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *stmt(){
    Node *node;

    if(consume_return()){
       node = calloc(1,sizeof(Node));
       node->kind = ND_RET;
       node->lhs = expr();
       expect(";");
    }else if(consume_if()){
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
    }else{
        node = expr();
        expect(";");
    }
    

    return node;
}


Node *primary(){
    
    // 次のトークンが”("なら、 "(" expr ")"のはず
    if(consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();

    if(tok){
        Node *node = calloc(1,sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if(lvar){
            node->offset = lvar->offset;
        }else{
            lvar = calloc(1,sizeof(LVar));
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->next = locals;
            lvar->offset = locals->offset + 8;
            // lvar->offset = 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    // ()でもidentでもなければ数値のはず
    return new_node_num(expect_number());
}

Node *mul(){
    
    Node *node = unary();
    
    for(;;){
        if(consume("*")){
            node = new_node(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_node(ND_DIV, node, unary());
        }else {
            return node;
        }
    }
}

Node *add(){
    
    Node *node = mul();

    for(;;){
        if(consume("+")){
            node = new_node(ND_ADD,node, mul());
        }else if(consume("-")){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }

}

Node *unary(){
    if(consume("+")){
        return primary();
    }
    if(consume("-")){
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node *equality(){
    Node *node = relational();
    for(;;){
        if(consume("==")){
            node = new_node(ND_EQUAL, node, relational());
        }else if(consume("!=")){
            node = new_node(ND_NOT_EQUAL, node, relational());
        }else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();

    for(;;){
        if(consume("<")){
            node = new_node(ND_SMALLER, node, add());
        }else if(consume("<=")){
            node = new_node(ND_SMALLER_OR_EQUAL, node, add());
        }else if(consume(">")){
            node = new_node(ND_SMALLER, add(), node);
        }else if(consume(">=")){
            node = new_node(ND_SMALLER_OR_EQUAL, add(), node);
        }else{
            return node;
        }
    }
}

Node *expr(){
    return assign();
}

Node *assign(){
    Node *node = equality();
    if(consume("=")){
        node = new_node(ND_ASSIGN, node , assign());
    }
    return node;
}

LVar *dummy_lvar(){
    LVar *var = calloc(1,sizeof(LVar));
    var->next = NULL;
    var->offset = 0;
}

LVar *find_lvar(Token *tok){
    //これまで保存されているローカル変数を走査する
    for(LVar *var = locals; var; var = var->next){
        //もし同じものがあればそのノードを返す
        if(var->len == token->len && memcmp(tok->str, var->name, var->len) == 0){
            return var;
        }
    }
    return NULL;
}

bool is_ident1(char c){
    //最初の文字がアルファベットか_なら識別子
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_ident2(char c){
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

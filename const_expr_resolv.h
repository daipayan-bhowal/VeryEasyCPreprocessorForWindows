
/******************************************************************************
      Expression Resolver For C programming language v1.6
        Designed by Daipayan Bhowal
       Using Priority linked list to solve expression evaluation
*******************************************************************************/
#pragma once
#include "defines.h"
#include "debug.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int size = 0;
int search_hash_defination_var(string_t Def);

void erasell(struct ListNode** head) /* erase the last 1 or 2 elements left in the priority linked list after calculation or travesal */
{
    struct ListNode* ptr = *head;
    struct ListNode* temp = NULL;
    while (ptr != NULL)
    {
        temp = ptr;
        ptr = ptr->next;
        free(temp);
        printf("memory is freed!\n");
        temp = NULL;
    }
    *head = NULL;
    return;
}

void printll(struct ListNode* head) /* used for debugging purpose not a functionality */
{
    struct ListNode* ptr = head;

    while (ptr != NULL)
    {
        if (ptr->isOperatorOrOperand == OPERAND)
        {
            printf("Symbol is %s priority is %d\n", ptr->str->value, ptr->priority);
        }
        if (ptr->isOperatorOrOperand == OPERATOR)
        {
            printf("Symbol is %c priority is %d\n", ptr->op, ptr->priority);
        }
        ptr = ptr->next;
    }

    printf("\n");
}

struct ListNode* maximum(struct ListNode* new_ptr)
{
    int i = size - 1;
    int max = -1;
    struct ListNode* ptr = new_ptr;
    struct ListNode* max_ptr = NULL;
    //printll();
    while (ptr != NULL)
    {
        //DecodeNodeOp(ptr);
        if (max < ptr->priority && ptr->isOperatorOrOperand == OPERATOR)
        {
            max = ptr->priority;
            max_ptr = ptr;
            printf("maximum value hit ! %c %d\n", ptr->op, ptr->priority);
        }

        ptr = ptr->next;
    }
    // printf("\noperator is %c\n", max_ptr->op);
    return max_ptr;
}

void fill(int isType, int pr, struct Symbol* s, int op, enum Operation opType, int derefLevel, struct ListNode** head)
{
    struct ListNode* ptr = (struct ListNode*)malloc(sizeof(struct ListNode));
    static struct ListNode* tail;
    ptr->isOperatorOrOperand = isType;
    ptr->priority = pr;
    ptr->next = NULL;
    ptr->prev = NULL;
    if (isType == OPERAND)
    {
        ptr->str = s;
        ptr->type = INT;
        ptr->isOperatorOrOperand = OPERAND;
        ptr->derefLevel = derefLevel;
    }
    else if (isType == OPERATOR)
    {
        ptr->op = op;
        ptr->opType = opType;
        ptr->isOperatorOrOperand = OPERATOR;
    }
    else if (isType == CONST)
    {
        ptr->str = s;
        ptr->type = CONST;
        ptr->isOperatorOrOperand = CONST;
    }

    if (*head == NULL)
    {
        *head = ptr;
        tail = ptr;
    }
    else
    {
        tail = *head;

        while (!tail && tail->next != NULL)
        {
            tail = tail->next;
        }

        tail->next = ptr;
        ptr->prev = tail;
    }

    size++;
    // (*head)->sz = size;
}

int ConstExprResolv(struct ListNode** new_ptr)
{
    int temp = 0;
    char buffer[64] = { 0 };


    // char *reg[] = { "r0" ,"r1" ,"r2" , "r3" , "r4"}; // for register/temp variable insertion
    //    static int rcount =0;
    do
    {

        struct ListNode* p = maximum(*new_ptr);
        if (p == NULL)
        {
            //   printf("operation completed\n"); 
            return temp;
        }
        struct ListNode* left_op = p->prev;
        struct ListNode* right_op = p->next;

        printf("DecodeNode Op is \n");
        DecodeNodeOp(p);
        //	printf("keft and right is: %s %s\n", left_op->str->value, right_op->str->value);
               // start the reduce functionality
        if (left_op == NULL && right_op == NULL)
            return 0;
        if ((left_op == NULL && right_op != NULL) || (left_op != NULL && right_op == NULL))
        {
            printf("Expression string is for pure unary op 1 !\n");
            if (left_op == NULL && right_op != NULL && p->opType == UNARY_OP)
            {
                switch (p->op)
                {
                case ADD_OP:
                    // do nothing
                    break;
                case SUB_OP:
                    // do nothing, legal statement but no opeartion
                    break;
                case NOT_OP:
                    temp = !atoi(right_op->str->value);
                    break;

                case BNOT_OP:
                    temp = ~atoi(right_op->str->value);
                    break;

                }
                free(right_op);
                right_op = NULL;
            }
            else if (left_op != NULL && right_op == NULL && p->opType == UNARY_OP)
            {
                printf("Expression string is for pure unary op 2 !\n");
                switch (p->op)
                {
                case ADD_OP:
                case DIV_OP:
                case MUL_OP:
                case SUB_OP:
                case NOT_OP:
                case AND_OP:
                {
                    printf("Compilation error!");
                    exit(0);
                }
                break;

                }
                /*  free(left_op); free is not required as left_op is being deleted by erase_ll, left_op is considered to be residual
                  left_op = NULL; */
            }
            printf("Expression string is for pure unary op 3, return temp !\n");
            return temp; // terminate as only 2 element one operator & one operand present
        }
        else if (left_op == NULL && right_op == NULL)
        {
            printf("Compilation error!");
            exit(0);
        }
        printf("p->op is %c %s %s!\n", p->op, left_op->str->value, right_op->str->value);
        if (p->opType == UNARY_OP)
        {


            if (right_op->isOperatorOrOperand == OPERAND) // pre increment
            {
                printf("right op is Operand\n");
                switch (p->op)
                {
                case ADD_OP:
                    temp = atoi(right_op->str->value);
                    break;
                case SUB_OP:
                    temp = -atoi(right_op->str->value);
                    break;
                case BNOT_OP:  // Binary not
                    temp = ~atoi(right_op->str->value);
                    break;
                case NOT_OP:
                    temp = !atoi(right_op->str->value);
                    break;
                }
                //  printf("left_op %s right_op %s\n", left_op->str->symbol,right_op->str->symbol);
                right_op->prev = p->prev;
                if (p->prev != NULL)
                    p->prev->next = right_op;

                if (right_op->next != NULL && p->prev != NULL)
                {
                    if (p->prev->priority > right_op->next->priority)
                    {
                        right_op->priority = p->prev->priority;
                    }
                    else
                    {
                        right_op->priority = right_op->next->priority;
                    }
                }
                else if (right_op->next != NULL)
                {
                    right_op->priority = right_op->next->priority;
                }
                else if (p->prev != NULL)
                {
                    right_op->priority = p->prev->priority;
                }
                free(p);
                p = NULL;
                continue;
            }
            else
            {
                printf("lvalue is required for post/pre-suffix operation\n");
            }
        }
        else if (p->opType == BINARY_OP)
        {
            switch (p->op)
            {
            case LSR_OP:
            {
                temp = atoi(left_op->str->value) < atoi(right_op->str->value);
            }
            break;
            case LSR_EQ_OP:
            {
                temp = atoi(left_op->str->value) <= atoi(right_op->str->value);
            }
            break;
            case GTR_EQ_OP:
            {
                temp = atoi(left_op->str->value) >= atoi(right_op->str->value);
            }
            break;
            case GTR_OP:
            {
                temp = atoi(left_op->str->value) > atoi(right_op->str->value);
            }
            break;
            case EQ_EQ_OP:
            {
                temp = atoi(left_op->str->value) == atoi(right_op->str->value);
            }
            break;
            case LSHR_OP:
                temp = atoi(left_op->str->value) << atoi(right_op->str->value);
                break;
            case RSHR_OP:
                temp = atoi(left_op->str->value) >> atoi(right_op->str->value);
                break;
            case SUB_OP:
                temp = atoi(left_op->str->value) - atoi(right_op->str->value);
                break;
            case ADD_OP:
                temp = atoi(left_op->str->value) + atoi(right_op->str->value);
                break;
            case XOR_OP:
                temp = atoi(left_op->str->value) ^ atoi(right_op->str->value);
                break;;
            case OR_OP:
                temp = atoi(left_op->str->value) | atoi(right_op->str->value);
                break;
            case OR_OR_OP:
                temp = atoi(left_op->str->value) || atoi(right_op->str->value);
                break;
            case AND_AND_OP:
                temp = atoi(left_op->str->value) && atoi(right_op->str->value);
                break;
            case DIV_OP:
                temp = atoi(left_op->str->value) / atoi(right_op->str->value);
                break;
            case MUL_OP:
                temp = atoi(left_op->str->value) * atoi(right_op->str->value);
                break;
            case MOV_OP:
                temp = atoi(right_op->str->value);
                break;
            case AND_OP:
                temp = atoi(left_op->str->value) & atoi(right_op->str->value);
                break;

            case MOD_OP:
                temp = atoi(left_op->str->value) % atoi(right_op->str->value);
                break;

            }

            if (left_op->prev == NULL && right_op->next == NULL)
            {
                // printf("left_op prev is NULL and right_op next is NULL");
                return temp;
            }
            printf("temp is %d!\n", temp);
#ifdef _MSC_VER
            snprintf(buffer, sizeof(buffer), "%d", temp);
#else
            sprintf(buffer, "%d", temp);
#endif
            //   Symbol_insert(reg[rcount++],INT ,buffer); // for register/temp variable insertion
               // printll();
               // make left_op as the temporary variable which will store the result of left_op + right_op
            left_op->next = right_op->next;
            if (right_op->next != NULL)
                right_op->next->prev = left_op;
#ifdef _MSC_VER
            strcpy_s(left_op->str->value, strlen(buffer), buffer);
#else
            strcpy(left_op->str->value, buffer);
#endif

            // Assign the neighbour node priority to out result node left_op
            if (left_op->prev != NULL && right_op->next != NULL)
            {
                if (left_op->prev->priority > right_op->next->priority)
                {
                    left_op->priority = left_op->prev->priority;
                }
                else
                {
                    left_op->priority = right_op->next->priority;
                }
            }
            else if (left_op->prev != NULL)
            {
                left_op->priority = left_op->prev->priority;
            }
            else if (right_op->next != NULL)
            {
                left_op->priority = right_op->next->priority;
            }
            free(right_op);
            right_op = NULL;
            free(p);
            p = NULL;
        }
        //   printll();
          //printf("maximum() pr is %d!\n", maximum());


    } while (1);

    return temp;
}

int getNextPriority(char* str) /* need this function to assign operands or variables with priority value */
{
    int pr = 0;
    char* s = str;
    while (*s != '+' && *s != '-' && *s != '/' && *s != '*' && *s != '%' && *s != '=' && *s != '>' && *s != '<' && *s != '!' && *s != '&' && *s != '^' && *s != '|' && *s != '?' && *s != ':')
    {
        s++;
    }
    if (strncmp(str, "&&", 2) == 0 || strncmp(str, "||", 2) == 0)
    {
        if (strncmp(str, "&&", 2) == 0)
        {
            pr = AND_AND_OP;
        }
        else if (strncmp(str, "||", 2) == 0)
        {
            pr = OR_OR_OP;
        }
    }
    else if (strncmp(str, "==", 2) == 0 || strncmp(str, ">=", 2) == 0 || strncmp(str, "<=", 2) == 0 || strncmp(str, "!=", 2) == 0)
    {
        switch (*s)
        {
        case '=':
            pr = EQ_EQ_OP;
            break;
        case '>':
            pr = GTR_EQ_OP;
            break;
        case '<':
            pr = LSR_EQ_OP;
            break;
        case '!':
            pr = NOT_EQ_OP;
            break;
        }
    }
    else if (strncmp(str, ">>", 2) == 0 || strncmp(str, "<<", 2) == 0)
    {
        if (strncmp(str, "<<", 2) == 0)
        {
            pr = LSHR_OP;
        }
        else if (strncmp(str, ">>", 2) == 0)
        {
            pr = RSHR_OP;
        }
    }
    else
    {

        switch (*s)
        {
        case '+':
            pr = ADD_OP;
            break;
        case '-':
            pr = SUB_OP;
            break;
        case '/':
            pr = DIV_OP;
            break;
        case '*':
            pr = MUL_OP;
            break;
        case '<':
            pr = LSR_OP;
            break;
        case '>':
            pr = GTR_OP;
            break;
        case '&':
            pr = AND_OP;
            break;
        case '|':
            pr = OR_OP;
            break;
        case '!':
            pr = NOT_OP;
            break;
        case '=':
            pr = MOV_OP;
            break;
        case '%':
            pr = MOD_OP;
            break;
        }
    }
    return pr;
}
int mapBlocklevel(int blocklevel)
{
    return (LAST_OP - ZERO_OP) * blocklevel;
}
int insert(char* str, int* k, struct ListNode** head) /* inserts operators and operands into the priority linked list */
{
    static int blocklevel = 0;
    static int priority = INIT_PRIORITY;
    char* s;
    char string_c[100];
    struct Symbol* sym_ptr;
    char* id=(char*)NULL;
    printf("process started for :%c\n", *str);
    if (*str == ' ' || *str == '\t' || *str == '\v' || *str == '\n')
    {
        // do nothing, just ignore whitespaces
        printf("do nothing, just ignore whitespaces\n");
        return 1;
    }
    else if (*str == '<')
    {
        int count = 1;
        int i = 1;
        while (*(str + i) == '<')
        {
            count++;
            i++;
        }
        if (count == 2)
        {
            priority = LSHR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, LSHR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, LSHR_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else if (count == 1 && *(str + 1) == '=')
        {
            printf("error: wrong operator !\n");
            exit(0);
        }
        else if (count == 1)
        {
            priority = LSR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, LSR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, LSR_OP, BINARY_OP, 0, head);
            }
        }
        else if (count > 1)
        {
            printf("error: wrong operator !");
            exit(0);
        }
        return 1;
    }
    else if (*str == '>')
    {
        int count = 1;
        int i = 1;

        while (*(str + i) == '>')
        {
            count++;
            i++;
        }

        if (count == 2)
        {
            priority = RSHR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, RSHR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, RSHR_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else if (count == 2 && *(str + 2) == '=')
        {
            priority = RSHR_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, RSHR_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, RSHR_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 2;
        }
        else  if (count == 1 && *(str + 1) == '=')
        {
            priority = GTR_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, GTR_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, GTR_EQ_OP, BINARY_OP, 0, head);
            }

            *k = *k + 1;
        }
        else if (count == 1)
        {
            priority = GTR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, GTR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, GTR_OP, BINARY_OP, 0, head);
            }
        }
        return 1;
    }
    else if (*str == '*')
    {
        int count = 1;
        int i = 1;
        int l = 1;
        int j = 1;
        while (*(str + i) == '*')
        {
            count++;
            i++;
        }
        if (count > 1)
            j = count;

        while (*(str - l) == ' ' || *(str - l) == '\t' || *(str - l) == '\v' || *(str - l) == '\n')
        {
            l++;
        }
        while (*(str + j) == ' ' || *(str + j) == '\t' || *(str + j) == '\v' || *(str + j) == '\n')
        {
            j++;
        }
        if (count == 1 && *(str + 1) == '=')
        {
            printf("error: wrong operator !");
            exit(0);
        }
        else if (count == 1)
        {
            priority = MUL_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, MUL_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, MUL_OP, BINARY_OP, 0, head);
            }
        }
        else if (count > 1)
        {
            printf("error: wrong operator !");
            exit(0);
        }
        return 1;
    }
    else if (*str == '/')
    {
        int count = 1;
        int i = 1;

        while (*(str + i) == '/')
        {
            count++;
            i++;
        }

        if (count == 1 & *(str + 1) == '=')
        {
            priority = DIV_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, DIV_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, DIV_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else if (count == 1)
        {
            priority = DIV_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, DIV_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, DIV_OP, BINARY_OP, 0, head);
            }
        }
        return 1;
    }
    else if (*str == '+')
    {
        int count = 1;
        int i = 1;

        while (*(str + i) == '+')
        {
            count++;
            i++;
        }

        if (count == 1 && *(str + 1) == '=')
        {

            priority = ADD_EQ_OP;

            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, ADD_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
                fill(OPERATOR, priority, NULL, ADD_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else if (count == 1)
        {

            priority = ADD_OP;

            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, ADD_OP, BINARY_OP, 0, head);
            }
            else
            {
                //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
                fill(OPERATOR, priority, NULL, ADD_OP, BINARY_OP, 0, head);
            }

        }
        return 1;
    }
    else if (*str == '-')
    {
        int count = 1;
        int i = 1;

        while (*(str + i) == '-')
        {
            count++;
            i++;
        }
        if (count > 1)
        {
            printf("%s:%d error: Invalid operator", __func__, __LINE__);
            exit(0);

        }
        if (count == 1 && *(str + 1) == '=')
        {
            printf("%s:%d error: Invalid operator", __func__, __LINE__);
            exit(0);
        }
        else if (count == 1)
        {
            priority = SUB_OP;
            if (blocklevel > 0)
            {//  printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, SUB_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, SUB_OP, BINARY_OP, 0, head);
            }
        }

        return 1;
    }
    else if (*str == '=')
    {
        if (*(str + 1) == '=')
        {
            priority = EQ_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, EQ_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, EQ_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else
        {

            priority = MOV_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, MOV_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, MOV_OP, BINARY_OP, 0, head);
            }
        }
        return 1;
    }
    else if (*str == '!')
    {
        if (*(str + 1) == '=')
        {
            printf("error: Not a valid operator");
        }
        else
        {
            priority = NOT_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, NOT_OP, UNARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, NOT_OP, UNARY_OP, 0, head);
            }
        }
        return 1;
    }
    else if (*str == '~')
    {
        priority = BNOT_OP;
        if (blocklevel > 0)
        { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
            fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, BNOT_OP, UNARY_OP, 0, head);
        }
        else
        {
            fill(OPERATOR, priority, NULL, BNOT_OP, UNARY_OP, 0, head);
        }
        return 1;
    }
    else if (*str == '^')
    {
        if (*(str + 1) == '=')
        {
            priority = XOR_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, XOR_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, XOR_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else
        {
            priority = XOR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, XOR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, XOR_OP, BINARY_OP, 0, head);
            }
        }
        return 1;
    }
    else if (*str == '&')
    {
        int count = 1;
        int i = 1;
        int l = 1;
        int j = 1;
        while (*(str + i) == '&')
        {
            count++;
            i++;
        }
        if (count == 2)  // for differentiating between '&' and '&&'
            j = 2;

        while (*(str - l) == ' ' || *(str - l) == '\t' || *(str - l) == '\v' || *(str - l) == '\n')
        {
            l++;
        }
        while (*(str + j) == ' ' || *(str + j) == '\t' || *(str + j) == '\v' || *(str + j) == '\n')
        {
            j++;
        }
        printf("count:%d %d %d\n ", count, isalnum(*(str - l)), isalnum(*(str + j)));
        if (count == 1 && (isalnum(*(str - l)) || *(str - l) == '_') && (isalnum(*(str + j)) || *(str - l) == '_') )  // for example i & j
        {
            priority = AND_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, AND_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, AND_OP, BINARY_OP, 0, head);
            }
        }
        else if (count == 1 && (isalnum(*(str + j)) || *(str + j) == '_')) // for address of( &k ) example addr = & k
        {
            priority = ADDR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, ADDR_OP, UNARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, ADDR_OP, UNARY_OP, 0, head);
            }
        }
        else if (count == 2 && (*(str + j) == '(') || ((isalnum(*(str - l)) || *(str - l) == '_') && (isalnum(*(str + j)) || *(str + j) == '_')))  // for logical or, ex:- i && j
        {
            priority = AND_AND_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, AND_AND_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, AND_AND_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else if (count == 1 && *(str + 1) == '=')
        {
            priority = AMP_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, AMP_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, AMP_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;

        }
        else if (count == 2)  // for logical or with unary operator, ex:- i && !j
        {
            priority = AND_AND_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, AND_AND_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, AND_AND_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else {
            printf("Compilation error ! line:%d", __LINE__);
            exit(0);
        }
        return 1;
    }
    else if (*str == '|')
    {
        printf("Line no. 903\n");
        int count = 1;
        int i = 1;
        int l = 1;
        int j = 1;
        while (*(str + i) == '|')
        {
            count++;
            i++;
        }
        if (count == 2)  // for differentiating between '|' and '||'
            j = 2;

        while (*(str - l) == ' ' || *(str - l) == '\t' || *(str - l) == '\v' || *(str - l) == '\n')
        {
            l++;
        }
        while (*(str + j) == ' ' || *(str + j) == '\t' || *(str + j) == '\v' || *(str + j) == '\n')
        {
            j++;
        }
        printf("count:%d %d %d\n ", count, isalnum(*(str - l)), isalnum(*(str + j)));
        if (count == 1 && strncmp(str+j, "defined", 7) == 0)
        {
            // check whether macro is present or not 
            // if present then fill the linked list with CONST value 1
            // if not present then fill the linked list with CONST value 0
            priority = OR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, OR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, OR_OP, BINARY_OP, 0, head);
            }
            str = str + j;
            goto DEF;
        }
        else if (count == 1 && (isalnum(*(str - l)) || *(str - l) == '_') && (isalnum(*(str + j)) || *(str + j) == '_'))  // for example i | j
        {
            priority = OR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, OR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, OR_OP, BINARY_OP, 0, head);
            }
        }
        else if (count == 1 && *(str + 1) == '=') // for address of( |=k ) example addr = |= k
        {
            priority = OR_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, OR_EQ_OP, UNARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, OR_EQ_OP, UNARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        if (count == 2 && strncmp(str+j, "defined", 7) == 0)
        {
            // check whether macro is present or not 
            // if present then fill the linked list with CONST value 1
            // if not present then fill the linked list with CONST value 0

            priority = OR_OR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, OR_OR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, OR_OR_OP, BINARY_OP, 0, head);
            }
            *k = *k + j;
            str = str + j;
            goto DEF;
        }
        else if (count == 2 && ((*(str + j) == '(') || (isalnum(*(str - l)) || *(str - l) == '_') && (isalnum(*(str + j)) || *(str + j) == '_')))  // for logical or, ex:- i || j
        {
            priority = OR_OR_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, OR_OR_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, OR_OR_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else {
            printf("Compilation error ! line:%d", __LINE__);
            exit(0);
        }
        return 1;
    }
    else if (*str == '%')
    {
        if (*(str + 1) == '=')
        {
            priority = MOD_EQ_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, MOD_EQ_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, MOD_EQ_OP, BINARY_OP, 0, head);
            }
            *k = *k + 1;
        }
        else
        {
            priority = MOD_OP;
            if (blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
                fill(OPERATOR, priority + mapBlocklevel(blocklevel), NULL, MOD_OP, BINARY_OP, 0, head);
            }
            else
            {
                fill(OPERATOR, priority, NULL, MOD_OP, BINARY_OP, 0, head);
            }
        }
        return 1;
    }
    else if (!isalnum(*(str - 1)) && strncmp(str, "sizeof", 6) == 0) // sizeof will be replaced by an integer value similar to a macro
    {
        char type_size = 1;
        char string_type_size[3];
        int j = 0, index = -1;
        char* s = str + 6;
        char* id;
        while (*(s + j) == ' ' || *(s + j) == '\n' || *(s + j) == '\t' || *(s + j) == '\v') // ignore white-spacing after 'sizeof'
            j++;
        while (*(s + j) != ')' && *(s + j) != ' ')
        {
            j++;
        }
        id = (char*)calloc(j, sizeof(char)); // -1 added to j for opening bracket '('
#ifdef _MSC_VER
        strncpy_s(id, strlen(id), s + 1, j - 1);
#else
        strncpy(id, s + 1, j - 1);
#endif
        //printf("id is %s s is %s j is %d\n", id,s,j ); 
        index = Symbol_search(id);
        if (strncmp(s, "(int)", 5) == 0)
        {
            type_size = 4;
        }
        else if (strncmp(s, "(char)", 6) == 0)
        {
            type_size = 1;
        }
        else if (strncmp(s, "(long long)", 11) == 0)
        {
            type_size = 8;
        }
        else if (strncmp(s, "(unsigned char)", 14) == 0)
        {
            type_size = 1;
        }
        else if (strncmp(s, "(unsigned int)", 13) == 0)
        {
            type_size = 4;
        }
        else if (strncmp(s, "(double)", 8) == 0)
        {
            type_size = 8;
        }
        else if (strncmp(s, "(float)", 7) == 0)
        {
            type_size = 4;
        }
        else if (strncmp(s, "(long)", 6) == 0)
        {
            type_size = 8;
        }
        else if (strncmp(s, "(short)", 7) == 0)
        {
            type_size = 2;
        }
        else if (strncmp(s, "('", 2) == 0)
        {
            if ((*(s + 3) == '\'' && *(s + 4) == ')') || (*(s + 4) == '\'' && *(s + 5) == ')'))
            {
                type_size = 1;
            }
            else
            {
                printf("error: Compilation error!");
                exit(0);
            }
        }
        else if (index != -1)
        {
            if (symtbl[index].type == INT)
                type_size = 4;
            else if (symtbl[index].type == FLOAT)
                type_size = 8;
            else if (symtbl[index].type == CONST)
                type_size = 4;

        }
        free(id);
        /********* similar to inserting an integer constant, integer constant defines the size of an object  *********/
#ifdef _MSC_VER
        snprintf(string_type_size, strlen(string_type_size), "%d", type_size);
#else
        sprintf(string_type_size, "%d", type_size); // converting integer to string 
#endif
        printf("mov %s,%s\n", register_str[reg_count], string_type_size);

        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*)malloc(strlen(string_type_size));

#ifdef _MSC_VER
        strcpy_s(sym->value, strlen(string_type_size), string_type_size);
#else
        strcpy(sym->value, string_type_size);
#endif
        sym->r = (Registers)reg_count++;
        if (blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority+mapBlocklevel(blocklevel));

            fill(CONST, priority + mapBlocklevel(blocklevel), sym, '\0', NULL_OP, 0, head);
        }
        else
        {
            //    printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority);
            fill(CONST, priority, sym, '\0', NULL_OP, 0, head);
        }
        *k = *k + 5 + j;
        // printf("next str is %s\n", str+*k ); 
    }
    else if (!isalnum(*(str - 1)) && strncmp(str, "defined", 7) == 0) // sizeof will be replaced by an integer value similar to a macro
    {
    DEF:   
        char macro_defined_value = 1;
        char string_macro_eval_value[3];
        int j = 0, index = -1, useless_space=0;
        char* s = str + 7;
        while (*(s + j) == ' ' || *(s + j) == '\n' || *(s + j) == '\t' || *(s + j) == '\v') // ignore white-spacing after 'defined'
        {
            j++;
            useless_space++;
        }
        if (*(s + j) == '(')  // ignore ( braces
        {
            useless_space++;
            j++;
        }
        while (*(s + j) != '\0' && *(s + j) != ')' && *(s + j) != ' ' && *(s + j) != '\n' && *(s + j) != '\t' && *(s + j) != '\v')
        {
            j++;
        }
        id = (char*)calloc(j - useless_space+1, sizeof(char)); // -1 added to j for opening bracket '('
#ifdef _MSC_VER
        strncpy_s(id, j - useless_space+1, s + useless_space, j - useless_space);
#else
        strncpy(id, s + useless_space, j - useless_space);
#endif
     
        //printf("id is %s s is %s j is %d\n", id,s,j ); 
        index = Symbol_search(id);

        if (index != -1)
        {
            macro_defined_value = 1;  // macro was not able to find as entry is not present
        }
        else
        {
            macro_defined_value = 0;
        }
       
        /********* similar to inserting an integer constant, integer constant defines the size of an object  *********/
#ifdef _MSC_VER
        snprintf(string_macro_eval_value, strlen(string_macro_eval_value), "%d", macro_defined_value);
#else
        sprintf(string_macro_eval_value, "%d", macro_defined_value); // converting integer to string 
#endif
       // printf("mov %s,%s\n", register_str[reg_count], macro_eval_value);
       // free(id);
        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*)calloc(strlen(string_macro_eval_value), sizeof(char));
#ifdef _MSC_VER
        strcpy_s(sym->value, strlen(string_macro_eval_value)+1, string_macro_eval_value);
#else
        strcpy(sym->value, macro_eval_value);
#endif
        sym->r = (Registers)reg_count++;
        if (blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority+mapBlocklevel(blocklevel));

            fill(CONST, priority + mapBlocklevel(blocklevel), sym, '\0', NULL_OP, 0, head);
        }
        else
        {
            //    printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority);
            fill(CONST, priority, sym, '\0', NULL_OP, 0, head);
        }
        *k = *k  + 5 + j + useless_space;
        // printf("next str is %s\n", str+*k ); 
       // free(id);
        return 1;
    }
    else if (*str == '(')
    {
        blocklevel++;
        priority = getNextPriority(str + 1);
        return 1;
    }
    else if (*str == ')')
    {
        int continous_brack = 1;
        blocklevel--;

        if (*(str + 1) == '\0')
            return 1;
        while (*(str + continous_brack) == ')')
        {
            blocklevel--;
            continous_brack++;
            if (*(str + continous_brack) == '\0')
                return 1;
        }
        priority = getNextPriority(str + 1);
        return 1;
    }
    else if (*str == '\'')
    {
        int i = 1;
        int character, char_in_digit_form;
        if ((*(str + 2) == '\'') || (*(str + 3) == '\''))
        {
            if (*(str + 2) == '\'')
                i = 2;
            else if (*(str + 3) == '\'')
                i = 3;
        }
        else
        {
            printf("error: Compilation error!");
            exit(0);
        }
        *k += i - 1;
        if (i == 3)
        {
            character = *(str + 1);
            char_in_digit_form = character - '0';
        }
        /************** adding the character as ascii integer constant ********/
#ifdef _MSC_VER
        strncpy_s(string_c, strlen(string_c), str + 1, 1);
#else
        strncpy(string_c, str + 1, 1);
#endif
        string_c[i] = '\0';
        printf("mov %s,%s\n", register_str[reg_count], string_c);

        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*)malloc(strlen(string_c));
#ifdef _MSC_VER
        strcpy_s(sym->value, strlen(string_c), string_c);
#else
        strcpy(sym->value, string_c);
#endif
        sym->r = (Registers)reg_count++;
        if (blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

            fill(CONST, priority + mapBlocklevel(blocklevel), sym, '\0', NULL_OP, 0, head);
        }
        else
        {
            //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
            fill(CONST, priority, sym, '\0', NULL_OP, 0, head);
        }



    }
    else if (isdigit(*str))
    {
        int i = 1;
        int int_literal_suffix_count = 0;
        printf("is_digit section!");
        s = str;

        while (isdigit(*(str + i)))
        {
            str++;
            i++;
        }
        while (*(str + i) == 'u' || *(str + i) == 'U' || *(str + i) == 'L' || *(str + i) == 'l')
        {
            str++;
            i++;
            int_literal_suffix_count++;
        }
        if (int_literal_suffix_count > 2)
        {
            printf("error: integer suffix is wrong !");
            exit(0);
        }

        if (*(s + i - 1) == '.')
        {
            goto FP;
        }
        //printf("is_digit section line no. 1366\n!");
        id = (char*)calloc(i+2, sizeof(char));
#ifdef _MSC_VER
        strncpy_s(id, i+2, s, i);
#else
        strncpy(id, s, i + 1);
#endif
        id[i + 1] = '\0';
       // string_c[i + 1] = '\0';
        //printf("mov %s,%s\n", register_str[reg_count], id);
        printf("is_digit section line no. 1377\n!");
        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*)calloc(i+2,sizeof(char));
#ifdef _MSC_VER
        strcpy_s(sym->value, i+2, id);
#else
        strcpy(sym->value, id);
#endif
       // free(id);
        sym->value[i + 1] = '\0';
        sym->r = (Registers)reg_count++;
        if (blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

            fill(CONST, priority + mapBlocklevel(blocklevel), sym, '\0', NULL_OP, 0, head);
        }
        else
        {
            //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
            fill(CONST, priority, sym, '\0', NULL_OP, 0, head);
        }
        *k += i; // for updating the reading pointer in main() for removing multi-integer constants
        printf("is_digit section end!");
        return i;

    }
    else if (*str == '_' || isalpha(*str))
    {
        int i = 0, index = -1;
        s = str;

        while (*str == '_' || isalpha(*str) || isdigit(*str))
        {
            str++;
            i++;
        }
        *k += i - 1; // for moving the reading pointer in main() for removing multi-charachter variable ex:-abc not just ex:-a
        char* temp = (char*)calloc(i+1, sizeof(char));
#ifdef _MSC_VER
        strncpy_s(temp, i+1, s, i);
#else
        strncpy(temp, s, i);
#endif
       // string_c[i] = '\0';
        //  printf("Symbol inserted is %s\n !",string_c );
       // string_c[i + 1] = '\0';
        index = Symbol_search(temp);
        if (index != -1)
        {
            if (priority == INIT_PRIORITY)
                priority = getNextPriority(str);
            else if (*(str + 1) != '\0' && priority < getNextPriority(str))
                priority = getNextPriority(str);

            // if symbol search is successful then move it into a register and assign a register
        //    static int reg_count = 0;

          //  if(reg_count >= 6)
        //      reg_count = reg_count%5;

            printf("mov %s,%s\n", register_str[symtbl[index].r], symtbl[index].value);


            /*   sym_ptr = (struct Symbol*) malloc (sizeof(struct Symbol));
               sym_ptr->symbol = (char*) malloc (strlen()) */
            if (blocklevel > 0)
            { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

                fill(OPERAND, priority + mapBlocklevel(blocklevel), &symtbl[index], '\0', NULL_OP, 0, head);
            }
            else
            {
                //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
                fill(OPERAND, priority, &symtbl[index], '\0', NULL_OP, 0, head);
            }

        }
        else
        {
            // printf("[ERROR] Unknown Symbol %s not found !",string_c );
            struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
            sym->value = (char*)malloc(sizeof(char)* strlen(temp));
#ifdef _MSC_VER
            strcpy_s(sym->value, strlen(temp), "0");
#else
            strcpy(sym->value, "0");
#endif
            sym->r = (Registers)reg_count++;
            if (blocklevel > 0)
            { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

                fill(CONST, priority + mapBlocklevel(blocklevel), sym, '\0', NULL_OP, 0, head);
            }
            else
            {
                //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
                fill(CONST, priority, sym, '\0', NULL_OP, 0, head);
            }
        }
        //free(temp);

        return i;
    }
    else if (*str == '.')
    {
    FP:     
       printf("error: floating point is not a valid token for preprocessor directives");
       return 0;

    }
    else if (*str == '\"')
    {
        printf("error: string is not a valid token for preprocessor directives");
        return 0;
    }
   /* if (id != NULL)
        free(id); */

    return 0;
}


struct ListNode** expression(char* arr) // we have kept it for parsing only for now, eval we will do afterwards
{
    int i;
    int expr_ret=0;
    //char arr[] = "a=b+++--c*c%99";
    struct ListNode** single_ptr = (struct ListNode**)malloc(sizeof(struct ListNode**));
    *single_ptr = NULL;
    for (i = 0; i < strlen(arr); )
    {
      //  printf("%s:%d %c is making the expression crash at pos:%d !", __func__, __LINE__, arr[i], i);
        expr_ret = insert(arr + i, &i, single_ptr); // we are passing i for jumping for double ot more digit constant or multi-chaacter variables
        if (!expr_ret)
        {
            single_ptr = NULL;
            break;
        }
        printf("processs completed !");
        i++;
    }
    //  printf("\nmaximum value is %d\n", maximum()->priority);
    // printll(*single_ptr);
    // expr_ret = ConstExprResolv(single_ptr);
     // erasell(single_ptr);
    return single_ptr;
}

int* searchCString(char* arr, char* find, int* numberOfOccurrance) /* searches multiple instances of find in str and returs the array with locations/indexes*/
{
    int k = strlen(arr);
    int l = strlen(find);
    int i;
    int* z = (int*)calloc(k / l, sizeof(int));
    int x = 0;
    for (i = 0;i < k;i++)
    {
        if (strncmp(arr + i, find, l) == 0)
        {
            z[x] = i;
            x++;
            *numberOfOccurrance = x;
        }
    }
    return z;
}

/*int main()
{
    symbols_entry();
    char arr[] = "a || b && c";
    // char arr[] = "e[d]";
    printf("expr val is %d\n", expression(arr));


    return 0;

} */


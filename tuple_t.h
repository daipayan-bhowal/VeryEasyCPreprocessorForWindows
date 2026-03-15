#pragma once

/******************************************************************************

  New Era Datastructure

 Designed by Daipayan Bhowal

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
typedef enum Type
{
    TUP_CHAR,
    TUP_INT,
    TUP_FLOAT,
    TUP_DOUBLE,
    TUP_SHORT,
    TUP_LONG,
    TUP_LONG_LONG,
    TUP_STRING
}type_t;

typedef struct Tables
{
    int index;
    int type[64];
    void* args[64];
    char max_index;
} tables;

typedef tables* table_t;


table_t itable(int num, ...) // init_table
{
    va_list valist;
    va_start(valist, num);
    static int counter = 0;
    table_t t = (table_t)malloc(sizeof(tables));
    t->index = ++counter;
    for (int j = 0; j < num; j++)
    {
        t->type[j] = va_arg(valist, type_t);
    }
    t->max_index = num;
    va_end(valist);
    return t;
}

void itable_set(table_t t, int num, ...)
{
    va_list valist;
    va_start(valist, num);

    for (int j = 0; j < num; j++)
    {
        switch (t->type[j])
        {
        case TUP_CHAR:
        {
            char* c = (char*)malloc(sizeof(char));
            char c2 = (char)va_arg(valist, int);
            memcpy(c, &c2, sizeof(char));
            t->args[j] = (void*)c; // need heap memory as stack memory is only visible to the function scope
            break;
        }


        case TUP_INT:
        {
            int* i = (int*)malloc(sizeof(int));
            int i2 = va_arg(valist, int);
            memcpy(i, &i2, sizeof(int));
            t->args[j] = (void*)i; // need heap memory as stack memory is only visible to the function scope
            break;
        }


        case TUP_FLOAT:
        {
            float* f = (float*)malloc(sizeof(float));
            float f2 = (float)va_arg(valist, double);
            memcpy(f, &f2, sizeof(float));
            t->args[j] = (void*)f; // need heap memory as stack memory is only visible to the function scope
            break;
        }


        case TUP_DOUBLE:
        {
            double* d = (double*)malloc(sizeof(double));
            double d2 = va_arg(valist, double);
            memcpy(d, &d2, sizeof(double));
            t->args[j] = (void*)d;
            break;
        }


        case TUP_SHORT:
        {
            short* s = (short*)malloc(sizeof(short));
            short s2 = (short)va_arg(valist, int);
            memcpy(s, &s2, sizeof(short));

            t->args[j] = (void*)s;
            break;
        }


        case TUP_LONG:
        {
            long* l = (long*)malloc(sizeof(long));
            long l2 = (long)va_arg(valist, long);
            memcpy(l, &l2, sizeof(long));
            t->args[j] = (void*)l;
            break;
        }


        case TUP_LONG_LONG:
        {
            long long* ll = (long long*)malloc(sizeof(long long));
            long long ll2 = (long long)va_arg(valist, long long);
            memcpy(ll, &ll2, sizeof(long long));
            t->args[j] = (void*)ll;
            break;
        }

        case TUP_STRING:
        {

            char* str2 = (char*)va_arg(valist, char*);
            char* str = (char*)malloc(strlen(str2));
            memcpy(str, str2, strlen(str2));
            t->args[j] = (void*)str;

            break;
        }

        }
    }
    va_end(valist);
}

void itable_printall(table_t t)
{
    int num = t->max_index;

    for (int j = 0; j < num; j++)
    {
        switch (t->type[j])
        {
        case TUP_CHAR:
            printf("%c ", *(char*)t->args[j]);
            break;

        case TUP_INT:
            printf("%d ", *(int*)t->args[j]);
            break;

        case TUP_FLOAT:
            printf("%f ", *(float*)t->args[j]);
            break;

        case TUP_DOUBLE:
            printf("%lf ", *(double*)t->args[j]);
            break;

        case TUP_SHORT:
            printf("%d ", *(short*)t->args[j]);
            break;

        case TUP_LONG:
            printf("%l ", *(long*)t->args[j]);
            break;

        case TUP_LONG_LONG:
            printf("%ll ", *(long long*)t->args[j]);
            break;

        case TUP_STRING:
            printf("%s ", (char*)t->args[j]);
            break;
        }
    }
    printf("\n");
}
void itable_get(void* ptr, type_t ty)
{

    switch (ty)
    {
    case TUP_CHAR:
        printf("%c ", *(char*)ptr);
        break;

    case TUP_INT:
        printf("%d ", *(int*)ptr);
        break;

    case TUP_FLOAT:
        printf("%f ", *(float*)ptr);
        break;

    case TUP_DOUBLE:
        printf("%lf ", *(double*)ptr);
        break;

    case TUP_SHORT:
        printf("%d ", *(short*)ptr);
        break;

    case TUP_LONG:
        printf("%l ", *(long*)ptr);
        break;

    case TUP_LONG_LONG:
        printf("%ll ", *(long long*)ptr);
        break;

    case TUP_STRING:
        printf("%s ", (char*)ptr);
        break;
    }

    printf("\n");
}
void* itable_getElem(table_t t, int column, type_t* type)
{
    void* ptr = t->args[column];
    *type = (type_t)t->type[column];

    return ptr;
}

void itable_destroy(table_t t)
{
    int num = t->max_index;

    for (int j = 0; j < num; j++)
    {
        free(t->args[j]);
    }
    free(t);
    t = NULL;
}
table_t itable_concat(table_t a, table_t b)
{
    int j, k, i;
    table_t temp = (table_t)malloc(sizeof(tables));
    temp->max_index = a->max_index + b->max_index;
    temp->index = a->index;
    for (j = 0; j < a->max_index; j++)
    {
        temp->type[j] = a->type[j];
        temp->args[j] = a->args[j];
    }
    for (k = j, i = 0; k < (j + b->max_index); k++, i++)
    {
        temp->type[k] = b->type[i];
        temp->args[k] = b->args[i];
    }
    return temp;
}

table_t itable_dup(table_t a)
{
    int j;
    table_t temp = (table_t)malloc(sizeof(tables));
    temp->max_index = a->max_index;
    temp->index = a->index;
    for (j = 0; j < a->max_index; j++)
    {
        temp->type[j] = a->type[j];
        temp->args[j] = a->args[j];
    }
    return temp;
}

void itable_swap(table_t a, table_t b)
{
    table_t temp = (table_t)malloc(sizeof(tables));
    int j = 0;
    temp->max_index = a->max_index;
    temp->index = a->index;
    for (j = 0; j < a->max_index; j++)
    {
        temp->type[j] = a->type[j];
        temp->args[j] = a->args[j];
    }
    a->max_index = b->max_index;
    a->index = b->index;
    for (j = 0; j < b->max_index; j++)
    {
        a->type[j] = b->type[j];
        a->args[j] = b->args[j];
    }

    b->max_index = temp->max_index;
    b->index = temp->index;
    for (j = 0; j < temp->max_index; j++)
    {
        b->type[j] = temp->type[j];
        b->args[j] = temp->args[j];
    }

    free(temp);
}


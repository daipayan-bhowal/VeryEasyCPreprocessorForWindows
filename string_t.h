#pragma once

/*
*    Very Easy C Preprocessor
*        by Daipayan Bhowal
*       Built in India (2025)
*
*
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef empty
#define STR_LEN_CHECK(str,pos)  str->len <= pos
#define IS_SPACE(str,pos) at(str,pos) == ' ' ||  \
	at(str, pos) == '\n' || \
	at(str, pos) == '\v' || \
	at(str, pos) == '\t'


struct string_struct
{
	int len;
	char* str;
	char* back;
	char empty;
};

typedef struct string_struct struct_var;
typedef struct_var* string_t;

typedef struct mem
{
	string_t str;
	int string_size;
	int index;
} memrecord;


string_t string_file(char* s, int* len);
string_t string(char* s);

void push(string_t st, char s);
char pop(string_t st);
char* con(string_t a);
int length(string_t a);
char at(string_t a, int i);
string_t copy(string_t a);
string_t getlines(string_t a);
string_t overwrite(string_t a, string_t b);
string_t getlineByPos(string_t a, int start_pos);

int locate(string_t s, string_t find_str);
int findString(string_t s, string_t find_str);
int findStringPos(string_t s, string_t find_str, int pos);
string_t partOfstring(string_t s, int start, int length);
char* c_string(string_t s);
void manipulate(string_t s, const char* str, int start);

string_t removeExtraSpacesInFront(string_t s);
string_t removeExtraSpacesInEnd(string_t s);
string_t removeFromBothEnds(string_t str, char a, char b, int *isRemoved);
string_t truncateFromEnd(string_t s, int trucationEnd);

string_t tokenizeFullWord(string_t str, int* offset);
string_t tokenizeUsingOffset(string_t str, int* offset);

string_t truncate(string_t s, int trucationStart);
int compare_cstr(string_t a, const char* b);
int findCharNext(string_t a, char c, int *pos);
string_t string_const(const char* s);
void setCommonChars(string_t s, int start, int end, char c);
void setCharsByString(string_t s1, string_t s2, int pos);

bool is_first(string_t str_line, int t_pos, char c);
void delete_str(string_t s, int start, int end);
void shift_str(string_t s, int start, int end);
void insert_str(string_t src, string_t ins_str, int pos);
void swap(string_t a, string_t b);
int compare(string_t a, string_t b);
string_t concat(string_t a, string_t b);
string_t concat_seperator(string_t a, string_t b, char sep);
string_t concat_copy(string_t a, string_t b);
string_t* concat_alloc(string_t* a, string_t b);
string_t tokenize(string_t a, char delim);
char pos(string_t a, int p);
void string_destroy(string_t st);
void string_destroy_unalloc(string_t st);
#define append_str(a,b) concat(a,b)
#define string_instances(a,b,arr)  searchstr(c_string(a), c_string(b), arr)
int* searchstr(char* arr, char* find, int* numberOfOccurrance);
/*int main()
{
	string_t c, d;
	string_t a = string("abc-bcd-");
	string_t b = string("abc");
	c = string("abc\nhelo\nbye");
	printf("%s\n", con(tokenize(a, '-')));
	printf("%s\n", con(tokenize(a, '-')));
	concat(a, b);
	printf("concat is:%s\n", con(a));
	push(a, '/');
	printf("%s\n", con(a));
	pop(a);
	printf("%s\n", con(a));
	swap(a, b);
	printf("swapped are:%s %s\n", con(a), con(b));
	printf("parsed lines are:%s %s\n", con(getlines(c)), con(getlines(c)));
	printf("%d\n", locate(a, b));

	printf("at string is:%c\n", at(b, 1));
	string_destroy(a);
	string_destroy(b);
}*/



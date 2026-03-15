//#include <stdafx.h>
/*
*    Very Easy C Preprocessor
*        by Daipayan Bhowal
*       Built in India (2025)
*
*
  */
#include "string_t.h"

int string_file_len(char* s)
{
	int i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return i;
}
string_t string_file(char* s, int* file_length)
{
	int len = string_file_len(s);
	printf("length of the file is:%d %d\n", len, s[len]);
	char* content = (char*)malloc(len + 1);
	s[len]='\0';
	//s[len+1]='\0';
	memcpy(content, s, len);
	content[len] = '\0';
	string_t st = (string_t)malloc(sizeof(struct_var));
	st->len = len + 1;
	st->str = content;
	st->back = content + len - 1;
	if (strcmp(s, "\0"))
		st->empty = 1;
	*file_length = len;
	return st;
}

string_t string(char* s)
{
	int len = strlen(s);

	char* content = (char*)calloc(len + 1,sizeof(char));
	memcpy(content, s, len);
	content[len] = '\0';
	string_t st = (string_t)malloc(sizeof(struct_var));
	st->len = len + 1;
	st->str = content;
	st->back = content + len - 1;
	if (strcmp(s, "\0"))
		st->empty = 1;

	return st;
}

string_t string_unalloc(char* dynamic_mem_str)
{
	int len = strlen(dynamic_mem_str);

	dynamic_mem_str[len] = '\0';
	string_t st = (string_t)malloc(sizeof(struct_var));
	st->len = len + 1;
	st->str = dynamic_mem_str;
	st->back = dynamic_mem_str + len - 1;
	if (strcmp(dynamic_mem_str, "\0"))
		st->empty = 1;

	return st;
}

string_t string_const(const char* s)
{
	int len = strlen(s);

	char* content = (char*)malloc(len + 1);
	memcpy(content, s, len);
	content[len] = '\0';
	string_t st = (string_t)malloc(sizeof(struct_var));
	st->len = len + 1;
	st->str = content;
	st->back = content + len - 1;
	if (strcmp(s, "\0"))
		st->empty = 1;

	return st;
}
#define empty_str(st)   if(st->len == 0)                                                 \
{ printf("Underflow ! func:%s line:%d\n", __FUNCDNAME__, __LINE__);      \
	return '\0';       }

void push(string_t st, char s)
{
	st->str = (char*)realloc(st->str, st->len + 1);
	st->str[st->len - 1] = s;
	st->len++;
	st->str[st->len - 1] = '\0';
}
char pop(string_t st)
{
	empty_str(st);

	char temp = st->str[st->len - 1];
	st->str = (char*)realloc(st->str, st->len - 1);
	st->len--;
	st->str[st->len - 1] = '\0';
	return temp;
}
char* con(string_t a) // content of the string
{
	return a->str;
}
int length(string_t a)
{
	return a->len;
}
char at(string_t a, int i)
{
	if (i >= a->len)
	{
		printf("length limit crossed\n");
		return 0;
	}

	return a->str[i];
}
int find_char(string_t a, char x)
{
	int i = 0;
	while (a->str[i] != x && a->str[i] != EOF)
	{
		i++;
	}
	if (a->str[i] == x)
		return i;

	return -1;
}

int findCharNext(string_t a,char c, int *pos)
{
	for (int i = *pos; i < a->len; i++)
	{
		if (a->str[i] == c)
		{
			*pos = i;
			return i;
		}

	}
	return -1;

}

string_t copy(string_t a)
{
	string_t ret = string(a->str);	
	return ret;
}

/*void update(string_t st, char* temp)
{
	int len = strlen(temp);
	st->str = (char*)realloc(st->str, len + 1);
	strncpy(temp, st->str, len);
}*/

string_t overwrite(string_t a, string_t b)
{
	int length_b = b->len;
	if (a->len >= b->len)
	{
		memcpy(a->str, b->str, length_b);
	}
	else
	{

		a->str = (char*)realloc(a->str, length_b);
		memcpy(a->str, b->str, length_b);
		a->str[length_b] = '\0';
		a->len = length_b;
	}
	return a;
}
/*string_t concat_overwrite(string_t a, string_t b)
{
	int length_b = b->len;
	if (a->len >= b->len)
	{
		memcpy(a->str, b->str, length_b);
	}
	else
	{

		a->str = (char*)realloc(a->str, a->len + length_b);
		memcpy(a->str+a->len-1, b->str, length_b);
		a->str[a->len+length_b-1] = '\0';
		a->len = a->len+length_b;
	}
	return a;
}*/

/*string_t getline(string_t a, int start_pos)
{
	int k = 0;
	string_t ret = NULL;
	char* s= NULL;
	int max_length = a->len - start_pos;
	while (k < max_length && a->str[k + start_pos] != '\n' && a->str[k+ start_pos] != '\0' && a->str[k + start_pos] != EOF && a->str[k + start_pos] != '\\')
	{
		k++;
	}
	s = (char*)calloc(k+1, sizeof(char));
	if (a->str[k+ start_pos] == '\n' || a->str[k+ start_pos] == '\0' || a->str[k+ start_pos] == EOF || a->str[k + start_pos] == '\\')
	{
		ret = (string_t)malloc(sizeof(string_t));		
		//memcpy(s, a->str + start_pos, k-1 );
		for (size_t i = 0; i < k; i++) {
			s[i] = a->str[i+start_pos];
		}
		//s[k] = '\0';
		ret->len = k;
		ret->empty = 0;
		ret->str = s;
		printf("getline str is: %s k+start_pods char:%d k:%d len:%d\n", ret->str, a->str[k + start_pos], k, ret->len);
		return ret;
		
	}
	return NULL;
}*/

string_t getlineByPos(string_t a, int start_pos)
{
	int i,j,k = 0;
	//string_t ret = NULL;
	char* s = NULL;

	while (a->str[k + start_pos] != '\n' && a->str[k + start_pos] != '\0' && a->str[k + start_pos] != EOF)
	{
		  k++;
	}
	if (k == 1 && (a->str[k + start_pos- 1] == '\t' || a->str[k + start_pos- 1] == '\v' || a->str[k + start_pos-1] == '\r'))
	{
		return NULL;
	}
	s = (char*)calloc(k + 1, sizeof(char));
	if (a->str[k + start_pos] == '\n' || a->str[k + start_pos] == '\0' || a->str[k + start_pos] == EOF)
	{
		for (i = 0, j = start_pos; i < k; i++, j++)
		{
			s[i] = a->str[j];
		}
		s[k] = '\0';
		//printf("The string is %s and len is %d\n", s, k);
		string_t ret = string_unalloc(s);
		return ret;
	}
	return NULL;
}


string_t getlines(string_t a)
{
	int prev = 0;
	static int k = 0;
	prev = k;
	string_t tk = NULL;
	if (a->str[k] == '\0')
		return NULL;
	while (a->str[k] != '\n' && a->str[k] != '\0')
	{
		k++;
	}
	if (a->str[k] == '\n' || a->str[k] == '\0')
	{
		tk = (string_t)malloc(sizeof(string_t));
		tk->str = (char*)malloc(a->len);
		memcpy(tk->str, a->str + prev, k - prev + 1);
		tk->len = k - prev + 1;
		tk->empty = 0;

		k++; // \n character to traverse
	}
	return tk;
}

int locate(string_t s, string_t find_str)
{
	int ln = find_str->len - 1;
	char* arr = (char*)calloc(ln+1, sizeof(char));
	int i, j;
	for (i = 0; i < s->len - find_str->len - 1; i++)
	{
		memcpy(arr, s->str + i, find_str->len - 1);
		for (j = find_str->len - 1; j > 0; j--)
		{
			
			if (strncmp(arr, find_str->str, j) == 0)
			{
				printf("arr is: %s, find_str is: %s  \n", arr, find_str->str);
				return i;
			}
		}
	}
	return -1;
}

int findString(string_t s, string_t find_str)
{
	int ln = find_str->len - 1;
	char* arr = (char*)calloc(ln + 1, sizeof(char));
	int i, j;
	for (i = 0; i < s->len - find_str->len - 1; i++)
	{
		memcpy(arr, s->str + i, find_str->len - 1);

			if (strncmp(arr, find_str->str, find_str->len) == 0)
			{
				printf("%s: %d, i is:%d, arr is: %s, find_str is: %s  \n", __func__, __LINE__,i, arr, find_str->str);
				return i;
			}
	}
	return -1;
}

int findStringPos(string_t s, string_t find_str,int pos)
{
	int ln = find_str->len - 1;
	int i, j;
	char* arr = (char*)calloc((ln+1),sizeof(char));
	for (i = pos; i < s->len - find_str->len-1; i++)
	{
		/*if ((i + ln) >= strlen(s->str))
		{
			free(arr);
			return -1;
		} */
		memcpy(arr, s->str + i, (size_t)(find_str->len)-1);
		//arr[ln] = '\0';
		//printf("arr is: %s , strlen(arr) is:%d , strlen(s) is %d,pos is:%d arr length is:%d, find->str->len is:%d\n", arr, strlen(arr), strlen(s->str), i, ln, find_str->len - 1);
		if (strncmp(arr, find_str->str, find_str->len-1) == 0)
		{
			printf("%s: %d, i is:%d, arr is: %s, find_str is: %s  \n", __func__, __LINE__, i, arr, find_str->str);
			free(arr);
			return i;
		}
	}
	//printf("arr string is: %s\n", arr);
	free(arr);
	return -1;
}
string_t removeExtraSpacesInFront(string_t s)
{
	int start_index = 0;
	int len = s->len;
	int i;
	for (int i = 0; i < len; i++)
	{
		if (s->str[i] == ' ' || s->str[i] == '\v' || s->str[i] == '\t' || s->str[i] == '\r')
			start_index++;
		else
			break;
	}
	if (start_index < len - 1)
	{
		char* new_str = (char*)calloc(len + 1,sizeof(char));
		for (i = 0; i < len - start_index; i++)
		{
			new_str[i] = s->str[i + start_index];
		}
		new_str[i] = '\0';
		string_t ret = string(new_str);
		ret->len = strlen(new_str);
		return ret;
	}

	s->len = len - start_index - 1;

	return s;
}

string_t removeExtraSpacesInEnd(string_t s)
{
	
	int len = s->len;
	int end_index = len;
	int i;
	for (int i = len-1; i >=0; i--)
	{
		if (s->str[i] == ' ' || s->str[i] == '\v' || s->str[i] == '\t' || s->str[i] == '\r')
			end_index--;
		else
			break;
	}
	if (end_index < len)
	{
		char* new_str = (char*)calloc(end_index, sizeof(char));
		for (i = 0; i < end_index; i++)
		{
			new_str[i] = s->str[i];
		}
		new_str[i] = '\0';
		string_t ret = string(new_str);
		ret->len = strlen(new_str);
		return ret;
	}

	s->len = end_index;

	return s;
}

string_t removeFromBothEnds(string_t str, char a,char b, int *isRemoved)
{
	if (str->str[0] == a)
	{
		*isRemoved = 1;
		str->str[0] = ' ';
	}

	if (str->str[str->len-1] == b)
	{
		str->str[str->len - 1] = ' ';
	}

	return str;
}
string_t truncate(string_t s, int trucationStart)
{
	char* new_str = (char*)calloc(s->len-trucationStart+1,sizeof(char));
	int i;
	int k = 0;
	for (i = trucationStart; i < s->len; i++)
	{
			new_str[k] = s->str[i];
			k++;
	}
	new_str[k] = '\0';
	string_t ret = string(new_str);
	ret->len = strlen(new_str);
	return ret;
}
string_t truncateFromEnd(string_t s, int trucationEnd)
{
	char* new_str = (char*)malloc(sizeof(char) * s->len + 1);
	int i;
	for (i = 0; i < trucationEnd; i++)
	{
		new_str[i] = s->str[i];
	}
	new_str[i] = '\0';
	string_t ret = string(new_str);
	return ret;
}
string_t partOfstring(string_t s, int start, int length)
{
	char* new_str = (char*)calloc(length+1, sizeof(char));
	int i;
	if (length > s->len)
	{
		printf("[ERROR] $s:%s String length is Out Of Bounds ! \n", __FUNCDNAME__, __LINE__);
		return NULL;
	}

	for (i = 0; i < length; i++)
	{
		new_str[i] = s->str[i + start];
	}
	new_str[i] = '\0';
	string_t ret = string_unalloc(new_str);
	//free(new_str);
	return ret;
}
char* c_string(string_t s)
{
	char* new_str = (char*)malloc(sizeof(char) * s->len + 1);
	int i;
	for (i = 0; i < s->len; i++)
	{
		new_str[i] = s->str[i];
	}
	new_str[i] = '\0';
	return new_str;
}
void insert_str(string_t src, string_t ins_str, int pos)
{
	int i, k, l, m, n;
	int diff_len = ins_str->len - 1;
	src->str = (char*)realloc(src->str, src->len + diff_len);
	src->len = src->len + diff_len;
	char* temp = (char*)calloc(src->len - pos + 1, sizeof(char));
	for ( i = pos, k = 0; i < src->len-1; i++, k++)
	{
		temp[k] = src->str[i];
	}
	temp[k] = '\0';
	k = strlen(temp);
	for (i = pos, l = 0; i < pos + diff_len; i++, l++)
	{
		src->str[i] = ins_str->str[l];
	}
	for (m = pos + diff_len, n = 0; n < k; m++, n++)
	{
		
		src->str[m] = temp[n];
	}
	src->str[m] = '\0';
	free(temp);
}

void shift_str(string_t s, int start, int end)
{
	int actual_length = 0;
	if (end < start)
	{
		printf("%s:%d error: end position(%d) is smaller than start position(%d)! ", __func__,__LINE__, end, start);
		return;
	}
	if (end >= s->len)
	{
		printf("%s:%d error: end character(%d) is greater than length of the string(%d)! ", __func__, __LINE__, end, s->len);
		return;
	}

	for (int i = start, j = end + 1; j < end+ 1 +(end-start) && j < s->len; j++, i++)
	{
		printf("shifted chars are: %c %c\n", s->str[i], s->str[j]);
		s->str[i] = s->str[j];
		actual_length++;
	}
	s->str = (char *)realloc(s->str, s->len - (actual_length));
	s->len = s->len - (actual_length);
	s->str[s->len - (actual_length)] = ' ';
}

void setCommonChars(string_t s, int start, int end, char c)
{
	if (end < start)
	{
		printf("%s:%d error: end position(%d) is smaller than start position(%d)! ", __func__,__LINE__, end, start);
		return;
	}
	if (end >= s->len)
	{
		printf("%s:%d error: end character(%d) is greater than length of the string(%d)! ", __func__, __LINE__, end, s->len);
		return;
	}
	for (int i = start; i < end; i++)
	{
		//printf("set chars are: %c\n", s->str[i]);
		s->str[i] = c;
	}
}

void setCharsByString(string_t s1, string_t s2, int pos)
{
	int i = pos;
	int k = 0;
	if (s2->len > s1->len)
	{
		printf("error: 2nd string length(%d) is greater than length of the 1st string(%d)! ", s2->len, s1->len);
		return;
	}
	for (; i < s2->len; i++)
	{
		s1->str[i] = s2->str[k++];
	}

}

void delete_str(string_t s, int start, int end)
{
	int i,j,diff = 0;
	if (end < start)
	{
		printf("%s:%d error: end position(%d) is smaller than start position(%d)! ", __func__, __LINE__, end, start);
		return;
	}
	if (end >= s->len)
	{
		printf("%s:%d error: end character(%d) is greater than length of the string(%d)! ", __func__, __LINE__, end, s->len);
		return;
	}
	if (end - start < s->len - end)
		diff = end - start;
	else
		diff = s->len - end;
	char* temp = (char*)calloc(s->len - diff, sizeof(char));
	if (end - start > s->len - end)
	{
		for (i = start, j = end + 1; j < s->len; i++, j++)
		{
			s->str[i] = s->str[j];
		}
	}
	else
	{
		for (i = start, j = end; i < end; i++, j++)
		{
			s->str[i] = s->str[j];
			
		}
		for (; j < s->len; j++,i++)
		{
			s->str[i] = s->str[j];
		}

	}
	memcpy(temp, s->str, s->len - (end - start) -1 );
	temp[s->len - (end - start) -1] = '\0';
	free(s->str);
	s->str = temp;
	s->len = s->len - (end - start);
}

void manipulate(string_t s, const char* str, int start)
{
	int length = 0, length2 = 0, i;
	while (str[length] != '\0')
		length++;

	if (s->len < length + start)
	{
		printf("[ERROR] $s:%s String length is Out Of Bounds ! \n", __FUNCDNAME__, __LINE__);
		return;
	}
	length2 = s->len;

	char* temp = (char*)malloc(sizeof(s->len));
	memcpy(temp, s->str, s->len);

	if (length > length2)
	{
		s->str = (char*)realloc(s->str, length);
		memcpy(s->str, temp, length);
	}
	else if (length < length2)
	{
		s->str = (char*)realloc(s->str, length2);
		memcpy(s->str, temp, length2);
	}
	for (i = 0; i < length; i++)
	{
		s->str[i + start] = str[i];
	}
	free(temp);
	return;
}

bool is_first(string_t str_line, int t_pos, char c)
{
	while (at(str_line, t_pos) == ' ' || at(str_line, t_pos) == '\v' || at(str_line, t_pos) == '\t')
		t_pos++;
	if (at(str_line, t_pos) == c)
		return true;
	else
		return false;
}


void swap(string_t a, string_t b)
{
	int length_a = a->len;
	int length_b = b->len;
	char* temp = (char*)malloc(sizeof(a->len));
	memcpy(temp, a->str, a->len);

	a->str = (char*)realloc(a->str, length_b);
	memcpy(a->str, b->str, length_b);
	b->str = (char*)realloc(b->str, length_a);
	memcpy(b->str, temp, length_a);

	free(temp);
}
int compare(string_t a, string_t b)
{
	char c1, c2;
	int i;
	if (a->len == b->len)
	{
		for (i = 0; i < a->len; i++)
		{
			c1 = b->str[i];
			c2 = a->str[i];
			if (c1 != c2)
			{
				return c2 - c1;
			}
		}
	}
	else
	{
		return b->len - a->len;
	}

	return 0;
}

int compare_cstr(string_t a, const char* b)
{
	if (a->len-1 != strlen(b))
	{
		return -1;
	}
	else
	{
		for (int i = 0; i < a->len; i++)
		{
			if (a->str[i] != b[i])
				return -1;

		}

	}
	return 0;

}

string_t concat_copy(string_t a, string_t b)
{
	char* new_str = (char*)calloc(a->len+b->len, sizeof(char));
	int i=0,k=0;
	if (a == NULL || a->str == NULL)
	{
		printf("Error in concat as first string is empty !\n");
		return NULL;
	}
	else if (b == NULL || b->str == NULL)
	{
		return a;
	}

	for (; i < a->len; i++)
	{
		new_str[i] = a->str[i];
	}

	for (k=0; k < b->len; k++,i++)
	{
		new_str[i] = b->str[k];
	}
	new_str[i] = '\0';
	string_t ret = string(new_str);

	return ret;

}
string_t concat(string_t a, string_t b)
{

	char* ptr;
	//string_t temp;
	if (a == NULL || a->str == NULL)
	{
		printf("Error in concat as first string is empty !\n");
		return NULL;
	}
	else if (b == NULL || b->str == NULL)
	{
		return a;
	}
	//printf("File content length inside concat():%d\n", a->len );
	
    //printf("%s: %d the entire 1st block inside concat():%s\n", __func__, __LINE__, a->str);
	//printf("%s: %d the 1st and 2nd block length inside concat(): len calculated:%d actual len:%d len calculated:%d actual len:%d\n", __func__, __LINE__, a->len, string_file_len(a->str), b->len , string_file_len(b->str));
	//printf("%s: %d the entire 2nd block inside concat():%s\n", __func__, __LINE__, b->str);
	a->str[string_file_len(a->str)] = ' ';
	a->str = (char*)realloc(a->str, (size_t)(a->len + b->len));
	for (int i = a->len; i < a->len + b->len-1; i++)
	{
		a->str[i] = ' ';
	}
	a->str[a->len + b->len - 1] = '\0';
	ptr = a->str + a->len-1;
	memcpy(ptr, b->str, b->len);

	//printf("%s: %d a->len is: %d b->len is: %d the entire block inside concat() ends is:\n",__func__,__LINE__, a->len,b->len);
	
	a->len = a->len + b->len;

	//printf("File content inside concat():%s\n", a->str);
	return a;
}

string_t concat_seperator(string_t a, string_t b, char sep)
{
	char* ptr;
	int k = 0;
	//string_t temp;
	if (a == NULL || a->str == NULL)
	{
		printf("Error in concat as first string is empty !\n");
		return NULL;
	}
	else if (b == NULL || b->str == NULL)
	{
		return a;
	}
	//printf("a->str is: %s len:%d\n", a->str, a->len);

	a->str = (char*)realloc(a->str, (size_t)(a->len + b->len));
	
	for (int i = a->len; i < a->len + b->len - 1; i++)
	{
		a->str[i] = b->str[k++];
	}
	a->str[a->len - 1] = sep;
	a->str[a->len + b->len - 1] = '\0';

	//printf("%s: %d a->len is: %d b->len is: %d the entire block inside concat() ends is:\n",__func__,__LINE__, a->len,b->len);

	a->len = a->len + b->len;

	//printf("File content inside concat():%s\n", a->str);
	return a;

}

string_t *concat_alloc(string_t *a, string_t b)
{
	if (!(*a) || !(*a)->str) {
		printf("First string is null\n");
		return NULL;
	}
	if (!b || !b->str) {
		return a;
	}

	size_t new_len = (*a)->len + b->len;
	char* new_str = (char*)calloc(new_len + 1, sizeof(char)); // +1 for '\0'
	if (!new_str) {
		printf("Memory allocation failed\n");
		return NULL;
	}

	memcpy(new_str, (*a)->str, (*a)->len);
	memcpy(new_str + (*a)->len, b->str, b->len);
	new_str[new_len] = '\0'; // explicit null terminator

	free((*a)->str); // Only after copying!
	(*a)->str = new_str;
	(*a)->len = new_len;

	return a;
}
void splitString(string_t wholestr, string_t a, string_t b, int pos)
{
	int i, k = 0;
	char* temp1 = (char*)calloc(pos, sizeof(char));
	for (i = 0; i < pos; i++)
	{
		temp1[k] = wholestr->str[i];
		k++;
	}
	a = string(temp1);
	free(temp1);

	char* temp2 = (char*)calloc(wholestr->len - pos, sizeof(char));
	for (i = pos; i < wholestr->len; i++)
	{
		temp2[k] = wholestr->str[i];
		k++;
	}
	a = string(temp2);
	free(temp2);
}

string_t tokenizeFullWord(string_t str, int* offset)
{
	//string_t str_withoutSpaces;
	int k, j, i = *offset, start;
	int unknown_char = true;
	if (*offset >= str->len - 1)
	{
		printf("offset is greater than  string length!");
		return NULL;
	}
	//printf("token input is: %s", str->str + i);
	while (str->str[i] == ' ' || str->str[i] == '\t' || str->str[i] == '\v' || str->str[i] == '\n')
	{
		i++;
	}
	start = i;
	int len = 0;
	if (
		str->str[i] == '{' ||
		str->str[i] == '}' ||
		str->str[i] == '[' ||
		str->str[i] == ']' ||
		str->str[i] == '&' ||
		str->str[i] == '^' ||
		str->str[i] == '%' ||
		str->str[i] == '*' ||
		str->str[i] == '+' ||
		str->str[i] == '-' ||
		str->str[i] == '|' ||
		str->str[i] == ';' ||
		str->str[i] == ':' ||
		str->str[i] == '?' ||
		str->str[i] == '=' ||
		str->str[i] == '~'
		)
	{
		i++;
		len++;
		unknown_char = false;
	}
	else if (str->str[i] == '\"') // string condition
	{
		i++;
		len++;
		unknown_char = false;
		while (str->str[i] != '\"' && i < str->len - 1) // everycharacter is accepted inside a string
		{
			i++;
			len++;
			unknown_char = false;
		}
		if (str->str[i] == '\"')
		{
			i++;
			len++;
			unknown_char = false;

		}
		else
		{
			printf("error: incomplete string, expected a closed \" ");
			exit(0);
		}

	}
	else
	{
		unknown_char = true;

		if (str->str[i] == '\"')
		{
			i++;
			len++;
			unknown_char = false;
			while ((str->str[i] >= 'a' && str->str[i] <= 'z') ||
				(str->str[i] >= 'A' && str->str[i] <= 'Z') ||
				(str->str[i] >= '0' && str->str[i] <= '9') ||
				(str->str[i] == '_'))
				{
				   i++;
				   len++;
				   unknown_char = false;
				}
			if (str->str[i] == '\"')
			{
				i++;
				len++;
				unknown_char = false;

			}

		}


		while (str->str[i] == '>' ||
			str->str[i] == '.' ||
			str->str[i] == ',' ||
			str->str[i] == '<' ||
			str->str[i] == '(' ||
			str->str[i] == ')' ||
			str->str[i] == '!' ||
			str->str[i] == '#' ||
			(str->str[i] >= 'a' && str->str[i] <= 'z') ||
			(str->str[i] >= 'A' && str->str[i] <= 'Z') ||
			(str->str[i] >= '0' && str->str[i] <= '9') ||
			(str->str[i] == '_'))
		{
			i++;
			len++;
			unknown_char = false;
		}
	}
	if (unknown_char == true)
	{
		if (str->str[i] == '\0')
			return NULL;
		printf("%s:%d error: unknown character(ASCII) is:%d", __func__, __LINE__, str->str[i]);
		exit(0);
	}
	char* s = (char*)calloc(len + 1, sizeof(char));
	for (k = 0, j = start; k < len; k++, j++)
	{
		s[k] = str->str[j];
	}
	s[len] = '\0';
	*offset = i;
	//printf("The string is %s and len is %d\n", s, len);
	string_t ret = string_unalloc(s);
	//free(s);
	return ret;
}

string_t tokenizeUsingOffset(string_t str, int* offset)
{
	//string_t str_withoutSpaces;
	int k,j,i= *offset,start;
	int unknown_char = true;
	if (*offset >= str->len)
	{
		printf("offset is greater than  string length!");
	}
	//printf("token input is: %s", str->str + i);
	while (str->str[i] == ' ' || str->str[i] == '\t' || str->str[i] == '\v' || str->str[i] == '\n')
	{
		i++;
	}
	start = i;
	int len = 0;
	if (str->str[i] == '(' ||
		str->str[i] == ')' ||
		str->str[i] == '[' ||
		str->str[i] == ']' ||
		str->str[i] == ',' ||
		str->str[i] == ';' ||
		str->str[i] == '!' ||
		str->str[i] == '=' ||
		str->str[i] == '{' ||
		str->str[i] == '}'
		)
	{
		i++;
		len++;
		unknown_char = false;
	}
	else
	{
		unknown_char = true;
		while (str->str[i] == '>' ||
			   str->str[i] == '.' ||
			   str->str[i] == '<' ||
			   str->str[i] == '&' ||
			   str->str[i] == '^' ||
			   str->str[i] == '%' ||
			   str->str[i] == '*' ||
			   str->str[i] == '+' ||
			   str->str[i] == '-' ||
			   str->str[i] == '|' ||
			   str->str[i] == '~' ||
			   str->str[i] == '\"' || 
			   str->str[i] == '#' ||
			   (str->str[i] >= 'a' && str->str[i] <= 'z') ||
			   (str->str[i] >= 'A' && str->str[i] <= 'Z') || 
			   (str->str[i] >= '0' && str->str[i] <= '9') ||
			   (str->str[i] == '_'))
		{
			i++;
			len++;
			unknown_char = false;
		}
	}
	if (unknown_char == true && str->str[i] != '\0')
	{

		printf("%s:%d error: unknown character is:%c ASCII format of unknown char:%d", __func__, __LINE__, str->str[i], str->str[i]);
		exit(0);
	}
	char* s = (char*)calloc(len+1,sizeof(char));
	for (k = 0, j = start; k < len; k++, j++)
	{
		s[k] = str->str[j];
	}
	s[len] = '\0';
	*offset = i;
	//printf("The string is %s and len is %d\n", s, len);
	string_t ret = string_unalloc(s);	
	//free(s);
	return ret;
}

string_t tokenize(string_t a, char delim)
{
	int len_counter = 0;
	static int k = 0;
	int prev = k;
	while (a->str[k] != delim && len_counter < a->len)
	{
		k++;
		len_counter++;
	}
	if (len_counter >= a->len)
		return NULL;
	char* content = (char*)malloc(len_counter + 1);
	memcpy(content, a->str + prev, len_counter); // prev needs to added for next string totokenize
	content[len_counter] = '\0';
	string_t b = (string_t)malloc(sizeof(struct_var));
	b->str = content;
	b->len = len_counter;
	b->back = content + len_counter - 1;
	b->empty = 0;
	k++;
	return b;
}
char pos(string_t a, int p)
{
	return a->str[p];
}
void string_destroy(string_t st)
{
	free(st->str);
	free(st);
}

void string_destroy_unalloc(string_t st)
{
	free(st);
}
#define append_str(a,b) concat(a,b)
#define string_instances(a,b,arr)  searchstr(c_string(a), c_string(b), arr)
int* searchstr(char* arr, char* find, int* numberOfOccurrance)
{
	int k = strlen(arr);
	int l = strlen(find);
	int i;
	int* z = (int*)malloc((k / l) * sizeof(int));
	int x = 0;
	for (i = 0; i < k; k++)
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


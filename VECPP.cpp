/*
*    Very Easy C Preprocessor
*        by Daipayan Bhowal
*       Built in India (2025) 
* 
* 
  */

#include "tape_t.h"
#include "string_t.h"
#include "tuple_t.h"
#include "bool_t.h"
#include <direct.h>
#include <time.h>
#include "const_expr_resolv.h"
#if _MSC_VER
#include "io.h"
#else
#include <cunistd>
#endif

typedef bool_t _bool;

/**** first design #include, #define VAR, #define FUNC() and #ifdef, #elif and #endif *****/
enum MacroType {
	define_t,
	include_t,
	line_t,
	undef_t,
	error_tt,
	pragma_t,
	defined_t,
	if_t,
	ifdef_t,
	ifndef_t,
	elif_t,
	else_t,
	endif_t,
	COMPILER_DATA,
	UNDEFINED,
	MAX_TYPE
};
/*MacroType getIndexFromStr(string_t s)
{
	string_t tmp;
	char* arr[13] = { "define","include","line","undef","error","pragma","defined","if","ifdef","ifndef","elif","else","endif" };
	for (int i = 0; i < 13; i++)
	{
		tmp = string(arr[i]);
		if ((compare(tmp, s)) == 0)
			return (MacroType)i;
	}
	return UNDEFINED;
} */

struct Define_var_map
{
	int lineno;
	int def_pos;
	int end_scope_lineno;
	string_t Def_str;
	string_t Rep_str;
} Defvar[2048];

int SetGetIndex_hash_def_var(int index, bool_t SetOrGet)
{

	static int max_index;
	if (SetOrGet == True)
	{
		max_index = index;
		return max_index;
	}
	else if (SetOrGet == False)
	{
		return max_index;
	}

	return -1;

}
int insert_hash_defination_var(int lineno, int pos, string_t Def, string_t Rep)
{
	int current_index = SetGetIndex_hash_def_var(0, False);
	if (current_index >= 2048)
	{
		printf("erro: no space left in #define var table!");
		return -1;
	}

	Defvar[current_index].lineno = lineno;
	Defvar[current_index].def_pos = pos;
	Defvar[current_index].Def_str = Def;
	Defvar[current_index].Rep_str = Rep;
	current_index++;
	SetGetIndex_hash_def_var(current_index, True);
	return current_index;

}

int search_hash_defination_var(string_t Def)
{
	for (int i = 0; i < SetGetIndex_hash_def_var(0, False); i++)
	{
		if (compare(Defvar[i].Def_str, Def) == 0)
		{
			return i;
		}

	}
	return -1;
}
void delete_hash_defination_var_by_index(int index)
{
	struct Define_var_map* Defptr = &Defvar[index];
	memset(Defptr, 0, sizeof(struct Define_var_map));
	int max_index = SetGetIndex_hash_def_var(0, False);
	for (int i = index+1; i < max_index; i++)
	{
		Defptr = &Defvar[i];
		memcpy(&Defvar[i-1], Defptr, sizeof(struct Define_var_map));

	}
	SetGetIndex_hash_def_var(max_index - 1, True);
	//memcpy(&Defvar[max_index - 1], 0, sizeof(struct Define_var_map));
}

bool_t update_hash_defination_end_scope(string_t Def, int end_scope_lineno)   /* #undef */
{
	for (int i = 0; i < 2048; i++)
	{
		if (compare(Defvar[i].Def_str, Def) == 0)
		{
			Defvar[i].end_scope_lineno = end_scope_lineno;
			return True;
		}

	}
	return False;

}

struct Define_fun_map
{
	int Def_fun_pos;
	int end_scope_lineno;
	string_t Def_fun_str;
	int args_len;
	string_t args_def[64];
	string_t args_caller[64];
	int def_args_end_pos;
	string_t body;
} Deffunc[1024];


int SetGetIndex_hash_def_func(int index, _bool SetOrGet)
{

	static int max_index;
	if (SetOrGet == True)
	{
		max_index = index;
		return max_index;
	}
	else if (SetOrGet == False)
	{
		return max_index;
	}

	return -1;

}

int insert_hash_defination_func_data(int def_func_pos, string_t DefFun, int args_len, string_t* args, int args_end_pos, string_t func_body)
{
	static int current_index;

	Deffunc[current_index].Def_fun_pos = def_func_pos;
	Deffunc[current_index].Def_fun_str = DefFun;
	Deffunc[current_index].def_args_end_pos = args_end_pos;
	Deffunc[current_index].args_len = args_len;
	for (int i = 0; i < args_len; i++)
	{
		Deffunc[current_index].args_def[i] = copy(args[i]);
	}
	Deffunc[current_index].body = copy(func_body);
	SetGetIndex_hash_def_func(current_index, True);
	current_index++;

	return current_index;
}

int search_hash_defination_func(string_t func_name)
{
	for (int i = 0; i < SetGetIndex_hash_def_func(0, False); i++)
	{
		if (compare(Deffunc[i].Def_fun_str, func_name) == 0)
		{
			return i;
		}

	}
	return -1;
}

void fetchFunctionCallerbyOffset(string_t strline, int* offset, string_t fetch, bool_t *isFunc)
{
	int i = *offset;
	*isFunc = False;
	int start_offset = -1;
	int ln = strline->len - 1;
	if (i >= ln)
	{
		fetch = NULL;
		return;
	}
	while (i <= ln && ((strline->str[i] >= 'a' && strline->str[i] <= 'z') ||
		(strline->str[i] >= 'A' && strline->str[i] <= 'Z') ||
		(strline->str[i] >= '0' && strline->str[i] <= '9') ||
		(strline->str[i] == '_'))
		)
	{
		if (start_offset == -1)
		{
			if ((strline->str[i] >= '0' && strline->str[i] <= '9'))
			{
				*isFunc = False;
				fetch = NULL;
				return;
			}
			start_offset = i;
		}
		i++;
	}
	*offset = i;
	while ((strline->str[i] == ' ' || strline->str[i] == '\t' || strline->str[i] == '\v' || strline->str[i] == '\r') && i <= ln)
	{
		i++;
	}
	*offset = i;
	if (strline->str[i] == '(')
	{
		*isFunc = True;
		while (strline->str[i] != ')' && i <= ln)
		{
			if (strline->str[i] == '\n')
			{
				*isFunc = False;
				fetch = NULL;
				return;
			}
			i++;
		}
		if (strline->str[i] == ')')
		{
			*isFunc = True;
			fetch = partOfstring(strline, start_offset, i);
			*offset = ++i;
			return;
		}
		*offset = i;
	}
}

void parseHashFuncDef(string_t strline, int Define_func_pos)
{
	int offset = 0, args_counter = 0, ln = length(strline);
	string_t Define_func_name = tokenizeUsingOffset(strline, &offset);
	string_t Define_bracket = tokenizeUsingOffset(strline, &offset);
	string_t new_tok = tokenizeUsingOffset(strline, &offset);
	string_t args[64];
	while (compare_cstr(new_tok, ")") != 0 && offset < ln)
	{
		if (compare_cstr(new_tok, ",") == 0)
			;
		else
		{
			args[args_counter] = copy(new_tok);
			args_counter++;
		}

		string_destroy(new_tok);
		new_tok = tokenizeUsingOffset(strline, &offset);
	}
	if (compare_cstr(new_tok, ")") != 0)
	{
		printf("error: expected ) !");
		exit(0);
	}
	else
	{   // fetch the function body
		string_t line;
		string_t func_body = string_const("");
		func_body = getlineByPos(strline, offset);


		/*while (line = getlines(strline))
		{
			if (at(line, line->len - 1) != '\\')
			{
				break;
			}
			else
			{
				concat(func_body, line);
			}
		}*/
		insert_hash_defination_func_data(Define_func_pos,Define_func_name, args_counter, args, offset,func_body);
		string_destroy(func_body);
	}


}
struct Include_map
{
	string_t filename;
	int line_number;
	//  string_t filecontent;
}IncMap[1024];

int SetGetIndex_include_content(int index, _bool SetOrGet)
{

	static int max_index;
	if (SetOrGet == True)
	{
		max_index = index;
		return max_index;
	}
	else if (SetOrGet == False)
	{
		return max_index;
	}

	return -1;

}

void insert_hash_include_content(string_t filename, int line_number)
{
	static int current_index;
	IncMap[current_index].filename = filename;
	IncMap[current_index].line_number = line_number;

	SetGetIndex_include_content(current_index, True);
	current_index++;


}

string_t HeaderTraversed[500];
static int curr = 0;
void markHeaderAlreadyIncluded(string_t header)
{
	HeaderTraversed[curr] = copy(header);
	curr++;
}

_bool checkHeaderAlreadyIncluded(string_t header)
{
	int i;
	for (i = 0; i < curr; i++)
	{
		if (strncmp(HeaderTraversed[i]->str, header->str, header->len) == 0)
		{
			return True;
		}

	}
	return False;

}

struct single_if_def_map
{
	bool_t isFilled;
	MacroType macro_type;
	int scope_no;  // x coordinate of hash if/ifdef
	int branch_no; // y coordinate of hash if/ifdef
	int start_char_pos;
	int end_char_pos;
	string_t body;
	//int if_line_no;
    //int endif_line_no;
	int calculated_expr; // stores the macro true false evaluated by expr or if already defined, should be updated by lexer
	//string_t hash_elifs_var[64];
};

struct Entire_If_def_block_map
{	 
	struct single_if_def_map single_block[256][256];
	int max_scope_no;
	int max_branch_no;
	string_t OnlyLeftOverbodyForTrueCond; // should be used after evaluation only
	int Ifblock_start_pos; // parser will store the entire #if... #endif block inside tape
	int Ifblock_end_pos;
} IfBlockAnalyzer[512];

int SetGetIndex_ifdef_map(int index, _bool SetOrGet)
{

	static int max_index;
	if (SetOrGet == True)
	{
		max_index = index;
		return max_index;
	}
	else if (SetOrGet == False)
	{
		return max_index;
	}

	return -1;

}

int get_start_pos_by_coordinates(int index, int x, int y)
{
	struct single_if_def_map* singleBlkPtr = &IfBlockAnalyzer[index].single_block[x][y];
	return singleBlkPtr->start_char_pos;
}

void set_body_pos_by_index(int index, int start_pos,int end_pos)
{
	IfBlockAnalyzer[index].Ifblock_start_pos = start_pos;
	IfBlockAnalyzer[index].Ifblock_end_pos = end_pos;
}

void get_body_pos_by_index(int index, int *start, int *end)
{
	*start = IfBlockAnalyzer[index].Ifblock_start_pos;
	*end = IfBlockAnalyzer[index].Ifblock_end_pos;
}

void fill_calculated_expr(int index, int x,int y,int expr)
{
	struct single_if_def_map* singleBlkPtr = &IfBlockAnalyzer[index].single_block[x][y];
    singleBlkPtr->calculated_expr = expr;
}

void fill_ifblock_map_by_coordinates(int index,int x,int y, MacroType mType, int start_pos,int end_pos)
{


	struct single_if_def_map *singleBlkPtr = &IfBlockAnalyzer[index].single_block[x][y];

	if (x >= 256 || y >= 256)
	{
		printf("error: at max 128 branches can be supported at a time!");
		exit(0);
	}

	//IfBlockAnalyzer[index].max_scope_no = max_scope;
	printf("\n index:%d (%d,%d) coordinates is filled True!\n",index, x, y);
	singleBlkPtr->isFilled = True;
	singleBlkPtr->macro_type = mType;
	singleBlkPtr->scope_no = x;
	singleBlkPtr->branch_no = y;
	if (start_pos != -1)
	{
		singleBlkPtr->start_char_pos = start_pos;
	}
	if (end_pos != -1)
	{
		singleBlkPtr->end_char_pos = end_pos;
	}

	if (IfBlockAnalyzer[index].max_scope_no < x)
	{
		IfBlockAnalyzer[index].max_scope_no = x;
	}

	if (IfBlockAnalyzer[index].max_branch_no < y)
	{
		IfBlockAnalyzer[index].max_branch_no = y;
	}
  /*	IfBlockAnalyzer[index].no_of_elif = no_of_elif;
	for (int i = 0; i < no_of_elif; i++)
	{
		IfBlockAnalyzer[index].elif_linenos[i] = elif_linenos[i];
		IfBlockAnalyzer[index].hash_elifs_var[i] = copy(hash_elifs[i]);
	} */
	//IfBlockAnalyzer[index].entireIfendIfBlock = copy(entireIfendIfBlock);

}

void get_coordinates_by_char_pos(int *fill_x, int *fill_y, int *fill_index,MacroType mType, int char_pos)
{
	int i = 0;
	int end_pos=-1,start_pos= -1;
	int max_index = SetGetIndex_ifdef_map(-1, False);
	for (i = 0; i < max_index; i++)
	{
		if (mType == endif_t)
		{
			end_pos = char_pos;
			for (int x = 0; x < 256; x++)
			{
				for (int y = 0; y < 256;y++)
				{
					if (IfBlockAnalyzer[i].single_block[x][y].isFilled == True &&
						IfBlockAnalyzer[i].single_block[x][y].end_char_pos == end_pos)
					{
						*fill_x = x;
						*fill_y = y;
						*fill_index = i;
						return;
					}
				}
			}
		}
		else
		{
			start_pos = char_pos;
			for (int x = 0; x < 256; x++)
			{
				for (int y = 0; y < 256;y++)
				{
					if (IfBlockAnalyzer[i].single_block[x][y].isFilled == True &&
						IfBlockAnalyzer[i].single_block[x][y].start_char_pos == start_pos)
					{
						*fill_x = x;
						*fill_y = y;
						*fill_index = i;
						return;
					}
				}
			}
		}

	}

}

void update_expr_by_coordinates(int index,int x, int y, int calculated_expr)
{
    /*
	* #ifdef MYWORLD -->(1,1)
	    #ifdef HELLO --> (2,1)

	    #endif

	    #ifdef BYE  --> (2,2)

	    #endif

	    #if WIN32  --> (2,3)

		#elif WIN64 --> (2, 4)

		#endif 
	* #endif 	
	*/

	struct single_if_def_map* singleBlkPtr = &IfBlockAnalyzer[index].single_block[x][y];
	singleBlkPtr->calculated_expr = calculated_expr;

}
 
/*void search_if_or_ifdef_or_elif(int endif_lineno, int block_no, bool_t* if_or_elif, int* ifline_index, int* elif_index) // search is required in #elif or #endif
{
	int search_lineno = endif_lineno - 1;
	*ifline_index = -1;
	*elif_index = -1;
	for (int i = 0; i < SetGetIndex_ifdef_map(0, False); i++)
	{
		if (*if_or_elif == True)
		{
			if (search_lineno >= IfBlockAnalyzer[i].if_line_no && block_no == IfBlockAnalyzer[i].block_no)
			{
				*ifline_index = i;
				break;
			}
		}
		else if(*if_or_elif == False)
		{
			for (int k = 0; k < IfBlockAnalyzer[i].no_of_elif; k++)
			{
				if (search_lineno >= IfBlockAnalyzer[i].elif_linenos[k] && block_no == IfBlockAnalyzer[i].block_no)
				{
					*ifline_index = i;
					*elif_index = k;
					break;
				}
			}

		}
	}


}*/


struct Lex_Macro_data
{
	int line_no;
	MacroType type;
	string_t macro_str_data;
	int def_var_map_index;
	int def_func_map_index;
	int def_include_map_index;
	int if_map_index;
}MacroLexAnalyzer[2048];

int SetGetIndex_macrolexdata(int index, _bool SetOrGet)
{

	static int max_index;
	if (SetOrGet == True)
	{
		max_index = index;
		return max_index;
	}
	else if (SetOrGet == False)
	{
		return max_index;
	}

	return -1;

}

int insert_macro_data(int line, MacroType ty, string_t str, int macro_index)
{
	static int current_index;
	if (current_index >= 2048)
	{
		printf("error: cannot open anymore !");
		return -1;
	}
	MacroLexAnalyzer[current_index].line_no = line;
	MacroLexAnalyzer[current_index].type = ty;
	MacroLexAnalyzer[current_index].macro_str_data = str;
	MacroLexAnalyzer[current_index].def_var_map_index = -1;
	MacroLexAnalyzer[current_index].def_func_map_index = -1;
	MacroLexAnalyzer[current_index].def_include_map_index = -1;
	MacroLexAnalyzer[current_index].if_map_index = -1;
	switch (ty)
	{
	case define_t:
		MacroLexAnalyzer[current_index].def_var_map_index = macro_index;
		break;

	case include_t:
		MacroLexAnalyzer[current_index].def_include_map_index = macro_index;
		break;

	case if_t:
		MacroLexAnalyzer[current_index].if_map_index = macro_index;
		break;

	case ifdef_t:
		MacroLexAnalyzer[current_index].if_map_index = macro_index;
		break;

	case ifndef_t:
		MacroLexAnalyzer[current_index].if_map_index = macro_index;
		break;

	}
	SetGetIndex_macrolexdata(current_index, True);
	current_index++;
	return current_index;

}

struct Lex_Non_macro_data
{

	int line_no;
	string_t non_macro_dat;
}CompilerData[10192];
int SetGetIndex_compilerdata(int index, _bool SetOrGet)
{

	static int max_index;
	if (SetOrGet == True)
	{
		max_index = index;
		return max_index;
	}
	else if (SetOrGet == False)
	{
		return max_index;
	}

	return -1;

}

int insert_non_macro_data(int lineno, string_t normal_clang_str)
{
	int current_index = SetGetIndex_compilerdata(-1, False);
	CompilerData[current_index].non_macro_dat = normal_clang_str;
	CompilerData[current_index].line_no = lineno;
	SetGetIndex_compilerdata(current_index, True); // setter for setting the index
	current_index++;

	return current_index;
}

int getlineNumberbyPos(string_t src, int offset)
{
	string_t line = NULL;
	int line_no = 0;
	int char_pos = 0;
	while ((line = getlines(src)) != NULL)
	{		 
		line_no++;
		char_pos += line->len;
		if (char_pos >= offset)
		{
			return line_no;
		}
	}
	return -1;
}

string_t custom_hashinclude(string_t file)
{
	int len = file->len;
	static string_t slash = string_const("\\");
	string_t cwd = string_const(""), full_path = string_const("");
	if (at(file, len - 2) != 'h' || at(file, len - 3) != '.')
	{
		printf("error: %s: No such file or directory", file);
		exit(0);
	}
	char* retCwd = 0;
#ifdef _MSC_VER
	retCwd = _getcwd((char*)cwd->str, (size_t)cwd->len);
#else
	retCwd = getcwd((char*)cwd->str, (size_t)cwd.length());
#endif
	if (retCwd != NULL)
	{
		concat(full_path, cwd);
		concat(full_path, slash);
		concat(full_path, file);
		//   fetch_str_fromfile_singleline(full_path,content);
		return full_path;
	}
	else
	{
		printf("getcwd() error\n");
	}
	return string_const("");
}

int search_file(const char* file_name)
{
	//  printf("Filename is %d\n", access(file_name,F_OK)); 
	int ret = -1;
#if _MSC_VER
	ret = _access(file_name, 0);
#else
	ret = access(file_name, F_OK);
#endif

	if (ret == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}

	return 0;
}

void system_hashinclude(string_t file, string_t path)
{
	int i, k = 0, g, incl_dir_size = 3;


#if defined(_WIN32) || defined(_WIN64)
	char slash[2] = "\\";
	char incl_dir[3][128] = { "C:\\MinGW\\include",
		"C:\\MinGW\\lib\\gcc\\mingw32\\6.3.0\\include",
		"C:\\MinGW\\mingw32\\lib\\gcc\\mingw32\\6.3.0\\include",
	};
#elif __linux 
	char slash[2] = "/";
	char incl_dir[3][128] = { "/usr/local/include",
		"/usr/target/include",
		"/usr/include" };
#endif
	char temp[128];
	for (i = 0; i < incl_dir_size; i++)
	{
		memset(temp, 0, sizeof(temp));
#ifdef _MSC_VER
		strcpy_s(temp, incl_dir[i]);
#else
		strcpy(temp, incl_dir[i]);
#endif
		k += sizeof(incl_dir[i]);
#ifdef _MSC_VER
		strcat_s(temp, slash);
#else
		strcat(temp, slash);
#endif
		k += sizeof(slash);
#ifdef _MSC_VER
		strcat_s(temp, file->str);
#else
		strcat(temp, file->str);
#endif
		k += sizeof(file->str);
		//temp[k] = '\0';
		if (search_file(temp) == 1) //search file at system level directory
		{
			printf("System directory found %s", temp);
			string_t temp2 = string(temp);
			path = overwrite(path, temp2);
			// fetch_str_fromfile_singleline(file,content);
			break;

		}
	}
}

bool_t is_id(string_t symbol)
{
	if (symbol->len < 1)
	{
		printf("empty string\n");
		return False;
	}
	int i = 0;

	while (at(symbol, i) == ' ' || at(symbol, i) == '\t' || at(symbol, i) == '\v' || at(symbol, i) == '\r')
	{
		i++;
	}
	//printf("id string is:%s\n", symbol->str);
	if (at(symbol, i) >= '0' && at(symbol, i) <= '9')
		return False;

	if (at(symbol, i) == '_' || (at(symbol, i) >= 'a' && at(symbol, i) <= 'z') || (at(symbol, i) >= 'A' && at(symbol, i) <= 'Z'))
	{
		if (symbol->len == 1)
		{
			return True;
		}
		i++;
		while (i < symbol->len - 1)
		{
			if (at(symbol, i) == '_' || (at(symbol, i) >= '0' && at(symbol, i) <= '9') || (at(symbol, i) >= 'a' && at(symbol, i)) <= 'z' || (at(symbol, i) >= 'A' && at(symbol, i) <= 'Z'))
				;
			else
				return False;
			i++;
		}
		return True;
	}
	return False;

}

bool_t is_num(string_t str)
{
	int pos = 0;
	int start_pos;

	if (compare_cstr(str, "0") == 0 ||
		compare_cstr(str, "1") == 0 ||
		compare_cstr(str, "2") == 0 ||
		compare_cstr(str, "3") == 0 ||
		compare_cstr(str, "4") == 0 ||
		compare_cstr(str, "5") == 0 ||
		compare_cstr(str, "6") == 0 ||
		compare_cstr(str, "7") == 0 ||
		compare_cstr(str, "8") == 0 ||
		compare_cstr(str, "9") == 0

		) // single digit case
	{
		return True;
	}
	while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
		pos++;
	    if (at(str, pos) == '-' || at(str, pos) == '~')
	    {
			pos++;
		}
		switch(at(str,pos))
		{ 
	        case '0':
		     if (at(str, pos + 1) == '0' && at(str, pos + 1) == '9')
		     {
			   pos++;
			   while ('0' <= at(str, pos) && at(str, pos) <= '9')
				pos++;
			   while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
				   pos++;
			  return True;

		     }
		     else if (at(str, pos + 1) == 'x' || at(str, pos + 1) == 'X')
		     {
			   pos += 2;
			while ('0' <= at(str, pos) && at(str, pos) <= '9')
				pos++;
			while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
				pos++;
			   return True;
		     }
		     else if (at(str, pos + 1) >= '0' && at(str, pos + 1) <= '9')
		     {
			    pos += 2;
			  while ('0' <= at(str, pos) && at(str, pos) <= '9')
				pos++;
			  while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
				  pos++;
			  return True;
		     }
	         case '1':
	         case '2':
	         case '3':
	         case '4':
	         case '5':
	         case '6':
	         case '7':
	         case '8':
	         case '9':
	         {

		     int dec_pos = 0;
		     start_pos = pos;
		     while ('0' <= at(str, pos) && at(str, pos) <= '9')
			    pos++;

		       if (at(str, pos) == '.')
		       {
			      dec_pos = pos;
			      pos++;
		       }
		        else if (at(str, pos) == EOF || IS_SPACE(str, pos))
		       {
			      return True;
		       }

		       while ('0' <= at(str, pos) && at(str, pos) <= '9')
			     pos++;
		       if (dec_pos == 0 && (at(str, pos) == 'u' || at(str, pos) == 'U'))
		       {
			      pos++;
				  while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
					  pos++;
			      return True;
		       }
		       else  if (dec_pos == 0 && (at(str, pos) == 'l' || at(str, pos) == 'L'))
		       {
			      pos++;
				  while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
					  pos++;
			      return True;
		       }
		       else if (dec_pos > 0 && (at(str, pos) == 'f' || at(str, pos) == 'F'))
		       {
			      pos++;
				  while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
					  pos++;
		          return True;
		       }
		       else  if (dec_pos > 0 && (at(str, pos) == 'l' || at(str, pos) == 'L'))
		       {
			       pos++;
				   while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
					   pos++;
			      return True;
		       }
		       else  if (dec_pos > 0 && (at(str, pos) == 'e' || at(str, pos) == 'E'))
		       {
			       pos++;
			       if (at(str, pos) == '+' || at(str, pos) == '-' || ('0' <= at(str, pos) && at(str, pos) <= '9'))
			       {
				      while ('0' <= at(str, pos) && at(str, pos) <= '9')
					    pos++;
			       }
				   while (at(str, pos) == '\t' || at(str, pos) == '\v' || at(str, pos) == ' ')
					   pos++;
			     return True;
		       }
		       if (dec_pos > 0)
			     return True;
		       else
			     return True;

		        break;
	         }
			 default:
				 return False;
			 

	    }
	return False;
}

bool_t is_func_caller(string_t strline)
{
	int offset = 0, args_counter = 0, ln = length(strline);
	string_t Define_func_name = tokenizeUsingOffset(strline, &offset);
	if (!is_id(Define_func_name))
		return False;
	if (offset >= ln - 1 )
		return False;
	string_t Define_bracket = tokenizeUsingOffset(strline, &offset);
	if (compare_cstr(Define_bracket, "(") != 0)
		return False;
	string_t new_tok = tokenizeUsingOffset(strline, &offset);
	while (compare_cstr(new_tok, ")") != 0 && offset < ln - 1)
	{
		if (compare_cstr(new_tok, ",") == 0)
			;

		string_destroy(new_tok);
		new_tok = tokenizeUsingOffset(strline, &offset);
	}
	if (compare_cstr(new_tok, ")") != 0)
	{
		return False;
	}
	else
	{
		return True;
	}

	return False;
}

/*bool_t is_sys_header_str(string_t headerfile)
{
	if (at(headerfile, 0) == '<' && at(headerfile, headerfile->len - 1) == '>')
		return True;
	else
		return False;
}
bool_t is_custom_header_str(string_t headerfile)
{
	if (at(headerfile, 0) == '\"' && at(headerfile, headerfile->len - 1) == '\"')
		return True;
	else
		return False;
}*/
int parseHeaderFile(string_t withoutHashInc, bool_t* system_or_custom)
{
	int end_pos = 0;
	for (int i = 0; i < withoutHashInc->len; i++)
	{
		if ((withoutHashInc->str[i] == '>' && withoutHashInc->str[0] == '<') || (withoutHashInc->str[i] == '\"' && withoutHashInc->str[0] == '\"'))
			break;
		else
			end_pos++;

	}
	char* c = withoutHashInc->str;
	if (withoutHashInc->str[0] != '\"' && withoutHashInc->str[0] != '<')
	{
		printf("error: Not a header file, expected \" or > but got: %c\n", withoutHashInc->str[0]);
		exit(0);
	}

	if (withoutHashInc->str[end_pos] != '\"' && withoutHashInc->str[end_pos] != '>')
	{
		printf("error: Not a header file, expected \" or > but got: %c\n", withoutHashInc->str[end_pos]);
		exit(0);
	}
	else
	{
		if (withoutHashInc->str[0] == '\"')
		{
			*system_or_custom = False;
		}
		else if (withoutHashInc->str[0] == '<')
		{
			*system_or_custom = True;
		}
	}
	withoutHashInc->str[0] = ' ';
	withoutHashInc->str[end_pos] = ' ';
	return end_pos;

}



void removeAtMiddle(string_t headercontent, string_t header, int include_loc, bool_t* system_or_custom)
{
	shift_str(headercontent, include_loc, include_loc + header->len);
}
void eraseAtTheMiddle(string_t headercontent, string_t header, int include_loc, char c)
{
	setCommonChars(headercontent, include_loc, include_loc + header->len,c);
}
string_t fetchHeaderString(string_t HeaderFilecontent, int include_loc)
{
	int length = 0;
	int start_filename = 0;
	int index = include_loc;
	if (HeaderFilecontent->str[index] == '#')
	{
		index = index + 8;
		while (HeaderFilecontent->str[index] == ' ' || HeaderFilecontent->str[index] == '\t' || HeaderFilecontent->str[index] == '\v')
			index++;
		start_filename = index;
		for (int i = start_filename + 1; i < HeaderFilecontent->len; i++)
		{
			if ((HeaderFilecontent->str[i] == '>' && HeaderFilecontent->str[start_filename] == '<') || (HeaderFilecontent->str[i] == '\"' && HeaderFilecontent->str[start_filename] == '\"'))
			{
				index++;
				break;
			}
			else
				index++;

		}
	}
	else
	{
		printf("error: fetchHeaderString unable to fetch header due to missing '#' \n");
		exit(0);
	}
	length = index - include_loc + 1;
	return partOfstring(HeaderFilecontent, include_loc, length);
}

string_t fetchHeaderfile(string_t onlyHeaderFilecontent, int include_loc, bool_t* system_or_custom) // fetches stdio.h from #include <stdio.h>
{
	string_t withoutHashInc = truncate(onlyHeaderFilecontent, include_loc + 8);
	string_t HashIncwithoutSpace = removeExtraSpacesInFront(withoutHashInc);
	int parse_len = parseHeaderFile(HashIncwithoutSpace, system_or_custom);
	string_t HeaderFilenameWithSpaceInFront = truncateFromEnd(HashIncwithoutSpace, parse_len);
	string_t HeaderFilename = removeExtraSpacesInFront(HeaderFilenameWithSpaceInFront);
	string_destroy(withoutHashInc);
	string_destroy(HashIncwithoutSpace);
	string_destroy(HeaderFilenameWithSpaceInFront);
	return HeaderFilename;
}

void getIncludeHeaderFullPath(string_t file, string_t path, bool_t system_or_custom_header_type)
{
	if (system_or_custom_header_type == True)
	{
		system_hashinclude(file, path);

	}
	else if (system_or_custom_header_type == False)
	{
		custom_hashinclude(file);
	}

	//   Tokens *curr_t;
	//  lex(file,curr_t);   // should be parser
	//  file_lines=append_file(curr_t,prev_t);
}

string_t removeComments2(string_t Filecontent)
{
	static string_t StartComment = string_const("/*");
	static string_t EndComment = string_const("*/");
	int start_loc = 0, end_loc = -1;
	int actual_end_loc = -1;
	int comment_pos[1000] = {0};
	int comm = 0;
	int offset = 0;
	int total_size = Filecontent->len;
	while ((start_loc = findStringPos(Filecontent, StartComment, start_loc)) != -1)
	{
		end_loc = findStringPos(Filecontent, EndComment, start_loc);
		actual_end_loc = end_loc + 2;
		printf("%s: %d, start_comment pos is:%d  end_comment pos is: %d\n", __func__, __LINE__, start_loc, end_loc);
		if (actual_end_loc >= Filecontent->len)
		{
			printf("%s: %d error: terminated as end_loc crossed the file length(%d) !", __func__, __LINE__, end_loc);
			break;
		}

		if (end_loc > start_loc && end_loc != -1)
		{
			//shift_str(Filecontent, start_loc, end_loc + 2);
			//printf("%s: %d, start_comment pos is:%d  end_comment pos is: %d\n", __func__, __LINE__, start_loc, end_loc);
			//printf("%s: %d, start_comment pos is:%d  end_comment pos is: %d\n", __func__, __LINE__, start_loc, end_loc);
			//Filecontent->str[Filecontent->len] = '\0'; 
			comment_pos[comm + 1] = start_loc;
			comment_pos[comm + 2] = actual_end_loc;
			comm += 2;
		}
		else
		{
			printf("%s: %d error: due to wrong end comment position(%d) !", __func__, __LINE__, end_loc);
			break;
		}
		offset = actual_end_loc - start_loc;
		start_loc = actual_end_loc;
		total_size = total_size - offset;
	}
	char* s = (char *)malloc(total_size*sizeof(char)+1);
	int length_traverse = 0;
	if (comm < 1)
		return copy(Filecontent);
	if (length_traverse < comment_pos[1])
	{
		for (int i = length_traverse; i < comment_pos[1]; i++)
		{
			s[i] = Filecontent->str[i];
			length_traverse++;
		}
	}
	for (int p=1; p <= comm ; p += 2)
	{

		if (p + 2 <= comm)
		{
			for (int i = comment_pos[p+1]; i < comment_pos[p+2]; i++)
			{
				s[length_traverse] = Filecontent->str[i];
				length_traverse++;
			}

		}
		else
		{
			for (int i = comment_pos[p + 1]; i < Filecontent->len; i++)
			{
				s[length_traverse] = Filecontent->str[i];
				length_traverse++;
			}

		}

	}
	s[length_traverse] = '\0';
	string_destroy(Filecontent);
	
	
	string_t ret = string(s);
	printf("%s: %d, file length after deletion is: %d %d\n", __func__, __LINE__, total_size, ret->len);
	return ret;
}


void setCommentChars(string_t Filecontent, char replaceChar)
{
	static string_t StartComment = string_const("/*");
	static string_t EndComment = string_const("*/");
	int start_loc = 0, end_loc = -1;
	while ((start_loc = findStringPos(Filecontent, StartComment, start_loc)) != -1)
	{
		end_loc = findStringPos(Filecontent, EndComment, start_loc + 2);


		if (end_loc >= Filecontent->len)
		{
			printf("\n%s: line no:%d error: terminated as end_loc crossed the file length(%d) !", __func__, __LINE__, end_loc);
			break;
		}

		if (end_loc > start_loc && end_loc != -1)
		{
			//printf("\n%s: line no:%d, start_comment pos is:%d  end_comment pos is: %d\n", __func__, __LINE__, start_loc, end_loc);
			for (int i = start_loc; i < end_loc + 2 && i < Filecontent->len; i++)
			{
				Filecontent->str[i] = replaceChar;
			}

		}
		else
		{
			printf("\n%s: line no:%d error: due to wrong end comment position(%d) and start pos(%d) !", __func__, __LINE__, end_loc, start_loc);
			break;
		}
		start_loc = end_loc + 2;
	}
	//Filecontent->str[end_loc + 2] = '\0';

	//printf("%s: %d, delete file content is: %s", __func__, __LINE__, Filecontent->str);

}

void removeMultilines(string_t Filecontent)
{
	static string_t StartSlash = string_const("\\\n");
	int start_loc = 0;
	while ((start_loc = findStringPos(Filecontent, StartSlash, start_loc)) != -1)
	{


		if (start_loc != -1)
		{
			//printf("\n%s: line no:%d, start_comment pos is:%d  end_comment pos is: %d\n", __func__, __LINE__, start_loc, end_loc);
			Filecontent->str[start_loc] = ' ';
			Filecontent->str[start_loc+1] = ' ';

		}
		else if (start_loc < Filecontent->len)
		{
			printf("\n%s: line no:%d error: due to wrong end comment position(%d) !", __func__, __LINE__, start_loc);
			break;
		}
		else
			break;
		start_loc = start_loc + 2;
	}

	//printf("%s: %d, delete file content is: %s", __func__, __LINE__, Filecontent->str);

}

void inliner(string_t onlyHeaderFilecontent) // call this one second
{
	static string_t inc = string_const("#include");
	bool_t system_or_custom_header_type = False;
	int include_loc = 0;
	char* tape = {};
	int file_length = 0;
    if ((include_loc = findStringPos(onlyHeaderFilecontent, inc, include_loc)) == -1 || onlyHeaderFilecontent->len < inc->len) // terminating condition
	{
		return;
	}

	string_t HeaderString = fetchHeaderString(onlyHeaderFilecontent, include_loc);  // entire #include <abc.h>
	string_t header = fetchHeaderfile(onlyHeaderFilecontent, include_loc, &system_or_custom_header_type);
	string_t path = string_const("");
	string_t RemovedCommentFilecontent = string_const("");
	if (checkHeaderAlreadyIncluded(header)) // Ignore the header content and delete the entire #include
	{
		printf("checkHeaderAlreadyIncluded()\n");
		//(onlyHeaderFilecontent, header, include_loc, &system_or_custom_header_type); // remove #include <abc..>
		eraseAtTheMiddle(onlyHeaderFilecontent, HeaderString, include_loc, ' ');  // remove #include <abc..>
		include_loc = include_loc + HeaderString->len;
	}
	else
	{
		markHeaderAlreadyIncluded(header);

		getIncludeHeaderFullPath(header, path, system_or_custom_header_type);
		tape = (char*)calloc(1, sizeof(char));
		//printf("\nHeader file name:%s header file len : %d, HeaderString:%s \n", header->str, header->len, HeaderString->str);

		/* error handling if header file not present */
		/*if (compare_cstr(path, "") == 0 && system_or_custom_header_type == true && header->len > 0)
		{
			include_loc = include_loc + HeaderString->len;
		} */

		if (system_or_custom_header_type == true)
			tape_read(path->str, &tape);
		else
			tape_read(header->str, &tape); // header contains only stdio.h
		string_t openFileContent = string_file(tape, &file_length);
		
		//printf("last file chars are : %c %c %c", openFileContent->str[file_length - 2], openFileContent->str[file_length-1], openFileContent->str[file_length]);
		//printline(openFileContent, file_length - 2);
		printf("\nFile length is:%d\n", file_length);
		//printf("\nFile context is: %s\n", openFileContent->str);
		//printf("\nFile context End\n");
		//printline(openFileContent, file_length-2);
		//eraseAtTheMiddle(onlyHeaderFilecontent, HeaderString, include_loc, ' '); // remove #include <abc..> by setting all header chars to ' '
		RemovedCommentFilecontent = removeComments2(openFileContent);

		
		//setCommentChars(openFileContent, ' ');
		
		//printf("Remove File content is:%s", RemovedCommentFilecontent->str);
		//removeAtMiddle(onlyHeaderFilecontent, HeaderString, include_loc, &system_or_custom_header_type);
		onlyHeaderFilecontent = concat(onlyHeaderFilecontent, RemovedCommentFilecontent);
	   // printf("\n Headerfile content after concat is: %s\n", onlyHeaderFilecontent->str);
		
	   // removeAtMiddle(onlyHeaderFilecontent, HeaderString, include_loc, &system_or_custom_header_type); // remove #include <abc..>
		eraseAtTheMiddle(onlyHeaderFilecontent, HeaderString, include_loc, ' '); // remove #include <abc..> by setting all header chars to ' '
		//printf("\n Headerfile content after header string removal is: %s\n", onlyHeaderFilecontent->str);

	}
	inliner(onlyHeaderFilecontent);
}

void StringReplacer(string_t expandedFileContent, string_t OriginalFilecontent) // call this one at last
{
	// #undef , before the line of #undef the string replacer will replace but after the line of #undef , stringreplacer won't work for the undef variable
	int loc = 0;
	int tokenOffset = 0;
	int index = -1;
	int trav_index = 0;
	int rep_str_pos = 0;
	int calculated_expr = 0;
	bool_t found = False;
	char buff[100] = { 0 };
	int isRemoved = 0;
	int offset = 0, ln=0, tok_pos =0, word_offset = 0;
	string_t bracket_fetch= NULL, args_fetch = NULL, word = NULL;
	bool_t func_found = False;

	while ((loc = findCharNext(expandedFileContent, '#', &loc)) != -1)
	{
		tokenOffset = loc + 1;
		string_t new_tok = tokenizeUsingOffset(expandedFileContent, &tokenOffset);
		//	printf("\nthe IfBlock loc_singleblock is:%d %s\n", loc_singleblock, new_tok->str);
		if (compare_cstr(new_tok, "define") == 0)
		{
			string_t def_str = tokenizeUsingOffset(expandedFileContent, &tokenOffset);
			rep_str_pos = tokenOffset;
			if (expandedFileContent->str[rep_str_pos] == '\n')
			{
				loc++;
				continue;
			}
			else if (expandedFileContent->str[rep_str_pos] == '(')
			{
				tokenOffset++;
				offset = 0;
				string_t arg_line_str = getlineByPos(expandedFileContent, tokenOffset);
				ln = arg_line_str->len- 1;
				new_tok = tokenizeUsingOffset(arg_line_str, &offset);
				while (compare_cstr(new_tok, ")") != 0 && offset < ln)
				{
					if (compare_cstr(new_tok, ",") == 0)
					{
						tokenOffset++;
						offset++;
					}

					string_destroy(new_tok);
					new_tok = tokenizeUsingOffset(arg_line_str, &offset);
					offset += new_tok->len - 1;
				}
				tokenOffset += offset;
				offset = 0;
				//continue;

			}
			string_t body_line_str = getlineByPos(expandedFileContent, tokenOffset);
			if (body_line_str == NULL)
			{
				loc++;
				continue;
			}

			string_t temp_str = removeExtraSpacesInFront(body_line_str);
			string_t temp_str2 = removeExtraSpacesInEnd(temp_str);
			string_t rep_str = temp_str2;
			word_offset = 0;
			if(!is_num(rep_str))
			{ 
				while ((word = tokenizeFullWord(rep_str, &word_offset)) != NULL && word_offset <rep_str->len)
				{
					printf("rep_str: %s word: %s\n", rep_str->str, word->str);
					if ((func_found=is_func_caller(word)) == True)
					{
						new_tok = tokenizeUsingOffset(word, &offset); // function name
						//	offset++;
						index = search_hash_defination_func(new_tok);
						bracket_fetch = tokenizeUsingOffset(word, &offset);


						if (index != -1)
						{
							for (int q = 0; q < Deffunc[index].args_len; q++)
							{
								args_fetch = tokenizeUsingOffset(word, &offset);
								bracket_fetch = tokenizeUsingOffset(word, &offset);
								if (compare_cstr(bracket_fetch, ",") != 0 && compare_cstr(bracket_fetch, ")") != 0)
								{
									printf("error: expected ',' or ')' !");
									exit(0);
								}
								while ((tok_pos = findStringPos(Deffunc[index].body, Deffunc[index].args_def[q], tok_pos)) != -1)
								{
									if ((tok_pos - 1) != Deffunc[index].Def_fun_pos)
									{
										delete_str(Deffunc[index].body, tok_pos, tok_pos + Deffunc[index].args_def[q]->len - 1);
										insert_str(Deffunc[index].body, args_fetch, tok_pos);
									}
									tok_pos++;
								}
								string_destroy(args_fetch);
							}

						}
						string_destroy(new_tok);
						continue;
					}
				}
			}
			temp_str2 = removeFromBothEnds(temp_str2, '(', ')', &isRemoved);
			if (isRemoved == 1)
			{
				rep_str = removeExtraSpacesInFront(temp_str2);
			}
			if (compare_cstr(rep_str, "\t") == 0)
			{
				loc++;
				continue;
			}
			isRemoved = 0;
			if (is_id(rep_str))
			{
				index = search_hash_defination_var(rep_str);
				if (index != -1)
				{
					for (trav_index = 0; trav_index < index; trav_index++)
					{
						if (Defvar[trav_index].Def_str == rep_str)
						{
							Defvar[index].Rep_str = Defvar[trav_index].Rep_str;
							found = True;
							break;
						}
					}
					if (found)
					{
						delete_str(expandedFileContent, rep_str_pos, tokenOffset);
						insert_str(expandedFileContent, Defvar[trav_index].Rep_str, rep_str_pos);
					}

				}
			}
		/*	else if (!is_num(rep_str)
				    && !is_id(rep_str)
				    && rep_str->str[0] != '\"'
				    && rep_str->str[0] != '*'
				    && rep_str->str[0] != '&'
				    )
			{
				    printf("last expr string is:%s", rep_str->str);
					struct ListNode** expr = expression(rep_str->str);
					if (expr != NULL)
					{
						calculated_expr = ConstExprResolv(expr);
						sprintf_s(buff, sizeof(buff), "%d", calculated_expr);
						//itoa_s(calculated_expr, buff, 10);
						string_t ConstExpr = string(buff);
						delete_str(expandedFileContent, rep_str_pos, tokenOffset);
						insert_str(expandedFileContent, ConstExpr, rep_str_pos);
					}
			} */



		}
		loc++;
	}
	tok_pos = 0;
	for (int i = 0; i < SetGetIndex_hash_def_var(0, False); i++)
	{
		string_t symbol_to_be_replaced = Defvar[i].Def_str;
		string_t replace_str = Defvar[i].Rep_str;
		
		while ((tok_pos = findStringPos(OriginalFilecontent, symbol_to_be_replaced, tok_pos)) != -1)
		{
			if ((tok_pos - 1) > Defvar[i].def_pos)
			{
				delete_str(OriginalFilecontent, tok_pos-1, tok_pos + symbol_to_be_replaced->len-1);
				insert_str(OriginalFilecontent, replace_str, tok_pos-1);
			}
			tok_pos++;
		}
		tok_pos = 0;

	}

	int args_replace_pos = 0;
    offset = 0;
	int args_counter = 0;
	int length_reduced = 0;
	for (int i = 0; i <=  SetGetIndex_hash_def_func(0, False); i++)
	{
		string_t symbol_to_be_replaced = Deffunc[i].Def_fun_str;
		string_t replace_str = Deffunc[i].body;

		while ((tok_pos = findStringPos(OriginalFilecontent, symbol_to_be_replaced, tok_pos)) != -1)
		{
			if ((tok_pos - 1) > Deffunc[i].Def_fun_pos)
			{
				offset = (tok_pos + symbol_to_be_replaced->len);
				string_t new_tok = tokenizeFullWord(OriginalFilecontent, &offset);
				while (compare_cstr(new_tok, ")") != 0 && offset < OriginalFilecontent->len)
				{
					if (compare_cstr(new_tok, ",") == 0)
						;
					else
					{
						Deffunc[i].args_caller[args_counter] = copy(new_tok);
						args_counter++;
					}

					string_destroy(new_tok);
					new_tok = tokenizeFullWord(OriginalFilecontent, &offset);
				}
				string_destroy(new_tok);
				delete_str(OriginalFilecontent, tok_pos, offset-1);
				length_reduced = offset - tok_pos - 1;
				for (int j = 0; j < Deffunc[i].args_len; j++)
				{
					args_replace_pos = findStringPos(replace_str, Deffunc[i].args_def[j], args_replace_pos);
					if (args_replace_pos != -1)
					{
						delete_str(replace_str, args_replace_pos, args_replace_pos + Deffunc[i].args_def[j]->len - 1);
						insert_str(replace_str, Deffunc[i].args_caller[j], args_replace_pos);
					}
				}
				insert_str(OriginalFilecontent, replace_str, tok_pos- 1);
			}
			tok_pos++;
		}
		tok_pos = 0;

	}

	printf("The original file content is:\n %s\n", OriginalFilecontent->str);

}

int findMin(int a, int b, int c)
{
	if (a < b)
	{
		if (a < c)
			return a;
		else
			return c;
	}
	else
	{
		if (b < c)
			return b;
		else
			return c;
	}
}

bool_t GetSingleIfBlocksByPos(string_t newExpandedSrcCode, long int *pos, long int *start, long int *end)
{
//	static string_t HashIf = string_const("if");
//	static string_t HashIfDef = string_const("ifdef");
//	static string_t HashIfNotDef = string_const("ifndef");
//	static string_t HashEndIf = string_const("endif");
	int loc = *pos;
	int tokenOffset = 0;
	int loc_hashif = -1;
	int loc_hashifdef = -1;
	int loc_hashifnotdef = -1;
	int loc_hashendif = -1;
	long int start_pos = -1, end_pos = -1;
	int if_counter = 0;
	bool endif_flag = false;
	
	while ((loc = findCharNext(newExpandedSrcCode, '#', &loc)) != -1)
	{
		//loc_hashendif = -1;
		tokenOffset = loc+1;
		string_t new_tok = tokenizeUsingOffset(newExpandedSrcCode, &tokenOffset);
		if (compare_cstr(new_tok, "if") != -1)
		{
			loc_hashif = loc-1;
			/*if (if_counter == 0 && endif_flag == true)
			{
				endif_flag = false;
				break;
			} */
			if_counter++;
			if (start_pos == -1)
				start_pos = loc-1;

			loc++;
			continue;
		}
        else if (compare_cstr(new_tok, "ifdef") != -1)
		{
			loc_hashifdef = loc-1;
			/*if (if_counter == 0 && endif_flag == true)
			{
				endif_flag = false;
				break;
			} */
			if_counter++;
			if (start_pos == -1)
				start_pos = loc-1;
			loc++;
			continue;
		}
        else if (compare_cstr(new_tok, "ifndef") != -1)
		{
			loc_hashifnotdef = loc-1;
			/*if (if_counter == 0 && endif_flag == true)
			{
				endif_flag = false;
				break;
			} */
			if_counter++;
			if (start_pos == -1)
				start_pos = loc-1;
			loc++;
			continue;
		}
	/*	else if (compare_cstr(new_tok, "else") != -1)
		{
			loc_hashifnotdef = loc - 1;
			//if (if_counter == 0 && endif_flag == true)
			//{
			//	endif_flag = false;
			//	break;
			// } 
			if_counter++;
			if (start_pos == -1)
				start_pos = loc - 1;
			loc++;
			continue;
		} */
        else if (compare_cstr(new_tok, "endif") != -1)
		{
			if_counter--;
			if (if_counter == 0)
			{
				loc_hashendif = loc + 6;
				break;
			}
			endif_flag = true;
			loc_hashendif = loc+6;
			loc++;
			continue;
		}
		else 
		 loc++;

		string_destroy_unalloc(new_tok);
		
	}
	end_pos = loc_hashendif;
//	printf("\nstart line is:");
//	printline(newExpandedSrcCode, end_pos-6);

/*	if (loc == -1 && if_counter != 0)
	{
		if (if_counter < 0)
		{
			printf("error: #ifdef is missing!");
			exit(0);
		}
		else if (if_counter > 0)
		{
			printf("error: #endif is missing!");
			exit(0);
		}

	}*/
	//printf("GetSingleIfBlocksByPos start:%d end:%d\n", start_pos, end_pos);
	*start = start_pos;
	*end = end_pos;
	*pos = end_pos;

	if (start_pos == -1 || end_pos == -1)
		return False;
	else 
		return True;
}

int get_nearest_y_coordinates_using_x(int index,int fixed_x,int y)
{
	int curr_y = y-1;
	for (;curr_y != 0; curr_y--)
	{
		
		if (IfBlockAnalyzer[index].single_block[fixed_x][curr_y].isFilled == True)
		{
			//printf("\n(%d,%d) is Filled value:%d\n", fixed_x, curr_y, IfBlockAnalyzer[index].single_block[fixed_x][curr_y].isFilled);
			return curr_y;
		}
	}
	return -1;
}


bool_t IsAllParentConditionTrue(int index,MacroType m_type,int x,int y)
{
	//int x = -1;
	//int y = -1;
	int nearest_y = -1;
	//int index = -1;
	int accumulated_true = 0;
	int curr_x = -1;
	struct single_if_def_map* singleBlkPtr = NULL;
	//get_coordinates_by_char_pos(&x,&y,&index,m_type,char_pos);
	singleBlkPtr = &IfBlockAnalyzer[index].single_block[x][y];
	if (x == 1) // no parent is present
	{
		if ((m_type == ifdef_t || m_type == if_t) && singleBlkPtr->calculated_expr != 0)
			return True;
		if (m_type == ifndef_t && singleBlkPtr->calculated_expr == 0)
			return True;
		else
			return False;
	}
	else {
		curr_x = x;
		
		while (curr_x != 0)
		{

			nearest_y = get_nearest_y_coordinates_using_x(index,curr_x, y);
			if (nearest_y == -1)
			{
				printf("error: no nearest y coordinate found ! index:%d (%d,%d) ", index, curr_x, y);
				break;
			}
			if (IfBlockAnalyzer[index].single_block[curr_x][nearest_y].macro_type == ifdef_t ||
				IfBlockAnalyzer[index].single_block[curr_x][nearest_y].macro_type == if_t
				)
			{
				if (IfBlockAnalyzer[index].single_block[curr_x][nearest_y].calculated_expr > 0)
				{
					accumulated_true |= 1;
				}
				else
				{
					accumulated_true |= 0;
				}
			}
			else if (IfBlockAnalyzer[index].single_block[curr_x][nearest_y].macro_type == ifndef_t)
			{
				if (IfBlockAnalyzer[index].single_block[curr_x][nearest_y].calculated_expr == 0)
				{
					accumulated_true |= 1;
				}
				else
				{
					accumulated_true |= 0;
				}

			}

			curr_x--;
		}
	}
	if (accumulated_true == 0)
		return False;
	else if (accumulated_true == 1)
		return True;
}

string_t EvauateSingleHashIfBlock(string_t newExpandedSrc,int start_pos, int end_pos, int i)
{

	int x = -1, y = -1;
	//int start_pos = -1, end_pos = -1;
	printf("evaluate Single Hash If Block\n");
	string_t Concat_Body = string_const(" ");
	string_t Ifline=NULL;
	string_t body_to_be_concat;
	bool_t insideIfblock = False;
	bool_t insideElifblock = False;
	int loc = -1;
	int tokenOffset = -1;
	x = 0;
	y = 0;
	int actual_offset = start_pos;
	while ( actual_offset < end_pos)
	{
		Ifline = getlineByPos(newExpandedSrc, actual_offset);


		//printline(newExpandedSrc, actual_offset);
		//printf("\nevaluate Single Hash Ifline is  : %s len:%d\n", Ifline->str, strlen(Ifline->str));
		loc = 0;
		
		while (Ifline->str[loc] == ' ' || Ifline->str[loc] == '\v' || Ifline->str[loc] == '\t')
			loc++;
		if (Ifline->str[loc] == '#')
		{
			tokenOffset = loc+1;
			string_t new_tok = tokenizeUsingOffset(Ifline, &tokenOffset);
			//printf("tokenized Ifline is  : %s\n", new_tok->str);
			if (compare_cstr(new_tok, "if") == 0)
			{
				
				setCommonChars(newExpandedSrc, actual_offset+loc, actual_offset + loc+ Ifline->len, ' ');
				//delete_str(newExpandedSrc, actual_offset + loc, actual_offset + loc + Ifline->len-1);
				x++;
				y++;
				//printf("x,y: %d,%d inside #if is  : %s\n", x, y, Ifline->str);

			}
			else if (compare_cstr(new_tok, "ifdef") == 0)
			{
				
				setCommonChars(newExpandedSrc, actual_offset+loc, actual_offset + loc + Ifline->len, ' ');
				//delete_str(newExpandedSrc, actual_offset + loc, actual_offset + loc + Ifline->len-1);
				x++;
				y++;
				//printf("x,y: %d,%d inside #ifdef is  : %s\n", x, y, Ifline->str);

			}
			else if (compare_cstr(new_tok, "ifndef") == 0)
			{
				
				setCommonChars(newExpandedSrc, actual_offset+loc, actual_offset + loc + Ifline->len, ' ');
				//delete_str(newExpandedSrc, actual_offset + loc, actual_offset + loc + Ifline->len-1);
				x++;
				y++;
				//printf("x,y: %d,%d inside #ifndef is  : %s\n", x, y, Ifline->str);
			}
			else if (compare_cstr(new_tok, "elif") == 0)
			{
				
				setCommonChars(newExpandedSrc, actual_offset+loc, actual_offset + loc + Ifline->len, ' ');
				//delete_str(newExpandedSrc, actual_offset + loc, actual_offset + loc + Ifline->len-1);
				y++;
				//printf("x,y: %d,%d inside #elif is  : %s\n", x, y, Ifline->str);
			}
			else if (compare_cstr(new_tok, "else") == 0)
			{
				
				setCommonChars(newExpandedSrc, actual_offset+loc, actual_offset + loc + Ifline->len, ' ');
				//delete_str(newExpandedSrc, actual_offset + loc, actual_offset + loc + Ifline->len-1);
				y++;
				//printf("x,y: %d,%d inside #elif is  : %s\n", x, y, Ifline->str);
			}
			else if (compare_cstr(new_tok, "endif") == 0)
			{
				
				setCommonChars(newExpandedSrc, actual_offset+loc, actual_offset + loc + Ifline->len, ' ');
				//delete_str(newExpandedSrc, actual_offset + loc, actual_offset + loc + Ifline->len-1);
				y++;
				//printf("x,y: %d,%d inside #endif is  : %s\n", x, y, Ifline->str);

				x--;

			}
			else if (IsAllParentConditionTrue(i,IfBlockAnalyzer[i].single_block[x][y].macro_type, x, y) == True)
			{
				//printf("index:%d (x,y) is: %d,%d inside inner IsAllParentConditionTrue() is  : %s %s %d\n",i, x,y,Concat_Body->str,  Ifline->str, Ifline->len);
				//int corrected_len = strlen(Ifline->str);
				//Ifline->len = corrected_len;
				 if (strcmp(Ifline->str, "") != 0)
					Concat_Body=concat_seperator(Concat_Body, Ifline, '\n');
			}

			string_destroy_unalloc(new_tok);

		}
		else if (IsAllParentConditionTrue(i,IfBlockAnalyzer[i].single_block[x][y].macro_type, x, y) == True)
		{
			//printf("index:%d (x,y) is: %d,%d inside outer IsAllParentConditionTrue() is  : %s %s %d\n",i, x,y,Concat_Body->str, Ifline->str, Ifline->len);
			 if (strcmp(Ifline->str,"") != 0)
				 Concat_Body= concat_seperator(Concat_Body, Ifline, '\n');
		}

		actual_offset += Ifline->len;
		if (Ifline != NULL)
			string_destroy(Ifline);
	}
	//printf("evaluate Single Concat_body is  : %s\n", Concat_Body->str);
	return Concat_Body;
}

void FillAndEvaluateAllSingleHashIfBlocks(string_t newExpandedSrcCode)
{
	int index = SetGetIndex_ifdef_map(-1,False);
	long int start = -1;
	long int end = -1;
	long int pos = 0;
	int loc_singleblock = 0;
	int tokenOffset = 0;
	int x = 0, y= 0;
	bool_t insideIfblock = False;
	string_t EvalBlock = string_const("");
	string_t FinalBlock = string_const("");
	while(GetSingleIfBlocksByPos(newExpandedSrcCode,&pos,&start,&end) == True)
	{
		printf("\nthe IfBlock start pos & end pos is:%d %d\n", start,end);
		
		//string_t IfBlock = partOfstring(newExpandedSrcCode, start,end-start+1);
		set_body_pos_by_index(index, start, end);
		x = 0;
		y = 0;
		loc_singleblock = start;
		printf("\nthe IfBlock fetched is !\n\n");
		while ((loc_singleblock = findCharNext(newExpandedSrcCode, '#', &loc_singleblock)) != -1 && 
			    loc_singleblock < end)
		{
		//	printline(newExpandedSrcCode, loc_singleblock);
			tokenOffset = loc_singleblock+1;
			string_t new_tok = tokenizeUsingOffset(newExpandedSrcCode, &tokenOffset);
		//	printf("\nthe IfBlock loc_singleblock is:%d %s\n", loc_singleblock, new_tok->str);
			if (compare_cstr(new_tok, "if") == 0)
			{
				insideIfblock = True;
				x++;
				y++;
				fill_ifblock_map_by_coordinates(index,x,y, if_t, loc_singleblock, -1); // -1 means Not Available

				string_t IfDefLine = getlineByPos(newExpandedSrcCode, tokenOffset);
				string_t IfDef = tokenizeUsingOffset(newExpandedSrcCode, &tokenOffset);
				//printf(": calling is_id()\n");
				bool_t id_flag = is_id(IfDef);
				if (id_flag == True)
				{
					int search_index = search_hash_defination_var(IfDef);
					if (search_index != -1)
					{
						update_expr_by_coordinates(index, x, y, 1);
					}
					else
					{
						update_expr_by_coordinates(index, x, y, 0);
					}

				} 
				else
				{
					if (IfDefLine->str[0] == '(')
					{
						IfDefLine->str[0] = ' ';
						for (int k = IfDefLine->len - 1; k > 0; k--)
						{
							if (IfDefLine->str[k] == ')')
							{
								IfDefLine->str[k] = ' ';
								break;
							}
						}
					}
					struct ListNode** expr = expression(IfDefLine->str);
					//printf("The expression() completed !\n");
					if (expr == NULL)
					{
						printf("%s:%d error: %s is not a valid expression in character no. %d !", __func__, __LINE__, IfDef->str, tokenOffset);
					//	printline(mutableTempFileContent, tokenOffset);
						exit(0);
					}
					
					int k = ConstExprResolv(expr);
					printf("The const expr resolv is: %d\n", k);
					update_expr_by_coordinates(index,x,y,k);
				}
				//string_destroy(IfDefLine);
				string_destroy_unalloc(IfDef);

				loc_singleblock++;
				continue;

			}
			else if (compare_cstr(new_tok, "ifdef") == 0)
			{
				insideIfblock = True;
				x++;
				y++;
				fill_ifblock_map_by_coordinates(index, x, y, ifdef_t, loc_singleblock, -1);
				loc_singleblock++;
				continue;

			}
			else if (compare_cstr(new_tok, "ifndef") == 0)
			{
				insideIfblock = True;
				x++;
				y++;
				fill_ifblock_map_by_coordinates(index, x, y, ifndef_t, loc_singleblock, -1);
				loc_singleblock++;
				continue;
			}
			else if (compare_cstr(new_tok, "elif") == 0)
			{
				insideIfblock = True;
				y++;
				fill_ifblock_map_by_coordinates(index, x, y, elif_t, loc_singleblock, -1);
				loc_singleblock++;
				continue;
			}
			else if (compare_cstr(new_tok, "else") == 0)
			{
				insideIfblock = True;
				y++;
				fill_ifblock_map_by_coordinates(index, x, y, elif_t, loc_singleblock, -1);
				loc_singleblock++;
				continue;
			}
			else if (compare_cstr(new_tok, "endif") == 0)
			{
				insideIfblock = False;
				y++;
				int getStartPos = get_start_pos_by_coordinates(index, x,y);
				//string_t subBody = partOfstring(IfBlock, getStartPos, loc_singleblock);
				fill_ifblock_map_by_coordinates(index, x, y, endif_t, -1, loc_singleblock);
				x--;
				loc_singleblock++;
				continue;
			}
			else
				loc_singleblock++;
			

			string_destroy_unalloc(new_tok);
		}
		printf("\njust before EvalBlock start & end is: %d and %d\n", start, end);
		EvalBlock = EvauateSingleHashIfBlock(newExpandedSrcCode, start,end, index);
		//printf("The evaluated block is:%s\n", EvalBlock->str);
		//printf("\njust fetching the EvalBlock start & end is: %d and %d\n", start, end);
		//setCommonChars(newExpandedSrcCode, start, end + 1, ' ');
		//setCharsByString(newExpandedSrcCode, EvalBlock, start);
		FinalBlock = concat_seperator(FinalBlock, EvalBlock, '\n');
		index++;
		SetGetIndex_ifdef_map(index,True);
		string_destroy_unalloc(EvalBlock);
		//string_destroy_unalloc(IfBlock);
		
	}
	
    printf("The evaluated final block is:%s\n", FinalBlock->str);
	printf("Hashifdef completed !");
}

void OriginalSourceFileProcessing(string_t OriginalSrcContent)
{
	int pos_hash = 0;
	int offset = 0;
	int index = SetGetIndex_compilerdata(-1, False);
	string_t line = NULL;
	int line_no=0;
	while ((line = getlines(OriginalSrcContent)) != NULL)
	{
		insert_non_macro_data(++line_no,line);
	}

}

void LineAndOptimizationProcessing(string_t OriginalSrcContent)
{
	int loc = 0;
	int tokenOffset = 0;
	int fetch_line_no = -1;
	while ((loc = findCharNext(OriginalSrcContent, '#', &loc)) != -1)
	{
		tokenOffset = loc+1;
		string_t new_tok = tokenizeUsingOffset(OriginalSrcContent, &tokenOffset);
		if (compare_cstr(new_tok, "line") == 0)
		{
			string_t lineno_tok = tokenizeUsingOffset(OriginalSrcContent, &tokenOffset);
			if (is_num(lineno_tok))
			{
				fetch_line_no = getlineNumberbyPos(OriginalSrcContent, tokenOffset);
				CompilerData[fetch_line_no].line_no = atoi(lineno_tok->str);
			}
			else
			{
				printf("error: line number expected for #line!\n");
				exit(0);
			}
		}
		else if (compare_cstr(new_tok, "pragma") == 0)
		{
			string_t option_tok = tokenizeUsingOffset(OriginalSrcContent, &tokenOffset);
			if (!is_id(option_tok))
			{
				printf("error: #pragma missing option!");
				exit(0);
			}


		}
		else if (compare_cstr(new_tok, "warning") == 0)
		{
			string_t warn_line_str = getlineByPos(OriginalSrcContent, tokenOffset);
			printf("warning: %s\n", warn_line_str->str);
		}
		else if (compare_cstr(new_tok, "error") == 0)
		{
			string_t error_line_str = getlineByPos(OriginalSrcContent, tokenOffset);
			if (error_line_str != NULL)
			{
				printf("error: %s\n", error_line_str->str);
				exit(0);
			}

		}
		loc++;
	}


}

void ReplacePredefinedMacros(string_t ExpandedHeaderSrcCode,string_t OriginalSrcContent, string_t srcFile)
{
	string_t Predefined_List[5] = {string_const("__LINE__"), string_const("__FILE__"), string_const("__DATE__"), string_const("__TIME__"), string_const("__STDC__")};
	int i = 0;
	int tok_pos = 0;
	int fetch_line_no = 0;
	char buff[32] = {'\0'};
	string_t buff_str = NULL;
	time_t t = time(NULL);
	struct tm* time_buf= (struct tm*)malloc(sizeof(struct tm));
#ifdef _MSC_VER
	gmtime_s(time_buf, &t);
#else
	time_buf = gmtime(&t);
#endif
	OriginalSrcContent->len = strlen(OriginalSrcContent->str);
	for (i = 0; i < 5; i++)
	{
		while ((tok_pos = findStringPos(OriginalSrcContent, Predefined_List[i], tok_pos)) != -1)
		{
			
			delete_str(OriginalSrcContent, tok_pos, tok_pos + Predefined_List[i]->len - 1);

			switch (i)
			{
			    case 0: // __LINE__
					fetch_line_no = getlineNumberbyPos(OriginalSrcContent, tok_pos);
#ifdef _MSC_VER
					sprintf_s(buff, "%d", CompilerData[fetch_line_no].line_no);
#else
					sprintf(buff, "%d", CompilerData[fetch_line_no].line_no);
#endif
					buff_str = string(buff);
					insert_str(OriginalSrcContent, buff_str, tok_pos);
				break;
				case 1: // __FILE__
				    insert_str(OriginalSrcContent, srcFile, tok_pos);
				break;

				case 2: // __DATE__
					strftime(buff, sizeof buff, "%b %d %Y", time_buf);
					buff_str = string(buff);
					insert_str(OriginalSrcContent, buff_str, tok_pos);

				break;

				case 3: // __TIME__
					strftime(buff, sizeof buff, "%H:%M:%S", time_buf);
					buff_str = string(buff);
					insert_str(OriginalSrcContent, buff_str, tok_pos);
				break;

				case 4:// __STDC__
					buff[0] = '1';
					buff[1] = '\0';
					buff_str = string(buff);
					insert_str(OriginalSrcContent, buff_str, tok_pos);

				break;
			}

			tok_pos++;
		}
		tok_pos = 0;

	}

}


/* void EvaluateAllHashIf()
{
	int max_index = SetGetIndex_ifdef_map(-1, False);
	int i = 0;
	string_t IfBlock;
	for (int i = 0; i < max_index; i++)
	{
		IfBlock = get_body_by_index(i);
		EvauateSingleHashIf(IfBlock, i);
	}

} */

bool_t IsEmptyLine(string_t strline)
{
	int i = 0;
	while ((strline->str[i] == ' ' || strline->str[i] == '\t' || strline->str[i] == '\v' || strline->str[i] == '\r') && i < strline->len)
	{
		
		i++;
	}

	if (i == strline->len - 1)
		return True;

	
	return False;
}

void RemoveAllExtraEmptyLines(string_t newExpandedSrcCode)
{
	string_t line = NULL;
	int pos = 0;
	while (pos < newExpandedSrcCode->len)
	{
		line = getlineByPos(newExpandedSrcCode, pos);
		if (line == NULL)
			return;
		if (IsEmptyLine(line) == True)
		{
			delete_str(newExpandedSrcCode, pos, pos+line->len);
			pos++;
		}
		else
		{
			pos += line->len;
		}
		
	}


} 

void RemoveAllExtraNewLines(string_t newExpandedSrcCode)
{
	int pos = 0;
	string_t nl = string_const("\n\n");
	while ((pos = findStringPos(newExpandedSrcCode, nl, pos)) != -1)
	{
			delete_str(newExpandedSrcCode, pos, pos + nl->len-1);
			pos++;
	}


}
void RemoveAllPreprocessorDirectives(string_t newExpandedSrcCode)
{
	int loc = 0;
	int tokenOffset = 0;
	while ((loc = findCharNext(newExpandedSrcCode, '#', &loc)) != -1)
	{
		tokenOffset = loc+1;
		string_t new_tok = tokenizeUsingOffset(newExpandedSrcCode, &tokenOffset);
		if (compare_cstr(new_tok, "include") == 0 ||
			compare_cstr(new_tok, "error") == 0 ||
			compare_cstr(new_tok, "warning") == 0
			)
		{
			string_t headerfile = tokenizeFullWord(newExpandedSrcCode, &tokenOffset);
			delete_str(newExpandedSrcCode, loc, tokenOffset);

		}
		else if (compare_cstr(new_tok, "define") == 0)
		{
			string_t Defstr = tokenizeUsingOffset(newExpandedSrcCode, &tokenOffset);
			string_t Repstr = getlineByPos(newExpandedSrcCode, tokenOffset);
			if(Repstr != NULL)
			    delete_str(newExpandedSrcCode, loc, tokenOffset + Repstr->len-1);
			else
				delete_str(newExpandedSrcCode, loc, tokenOffset);

		}
		else if (compare_cstr(new_tok, "if") == 0 || 
			     compare_cstr(new_tok, "ifdef") == 0 || 
			     compare_cstr(new_tok, "undef") == 0 ||
			     compare_cstr(new_tok, "elif") == 0)
		{
			string_t conditionalvar = tokenizeUsingOffset(newExpandedSrcCode, &tokenOffset);
			delete_str(newExpandedSrcCode, loc, tokenOffset);

		}
		else if (compare_cstr(new_tok, "endif") == 0)
		{
			delete_str(newExpandedSrcCode, loc, tokenOffset);
		}
		loc++;
	}


}
string_t split_hashinclude_data_from_clang(string_t srcfilecontent) // #include only need to seperate from clang and return the expanded file
{
	int pos_hash = 0;
	int TokOffset = 0, start = 0;
	string_t s = string_const("");
	while ((pos_hash = findCharNext(srcfilecontent, '#', &pos_hash)) != -1)
	{
		//string_t truc = truncate(mutableTempFileContent, pos_hash+1);
		start = TokOffset;
		string_t new_tok = tokenizeUsingOffset(srcfilecontent, &TokOffset);
		if (compare_cstr(new_tok, "#include") == 0)
		{
			// string_t truc2 = truncate(trunc, pos_hash+8);
			string_t tok = tokenizeUsingOffset(srcfilecontent, &TokOffset);
			string_t fetchHeaderline = partOfstring(srcfilecontent, pos_hash, TokOffset - start);
			s = concat(s, fetchHeaderline);

		}
		pos_hash++;
	}

	inliner(s);
	string_t ret = copy(s);
	return ret;// return the expanded code without #include
}





void copy_from_hashdef_entries_to_constexpr_symbols_entries()
{
	enum Registers r = ax;
	for (int i = 0; i < SetGetIndex_hash_def_var(0, False); i++)
	{
		if (r >= LAST_REG)
			r = ax;
		Symbol_insert(Defvar[i].Def_str->str, Ty::INT, Defvar[i].Rep_str->str, r);
		r = (enum Registers)(r + 1);
	}

	return;
}

void lexer(string_t srcfilecontent)
{
	int pos_hash = -1;
	string_t mutableTempFileContent = copy(srcfilecontent);
	int offset = 0, ln = 0;
	int start_pos_func = 0;
	int start_pos_var = 0;
	int hash_line_no = 1;
	int if_counter = 0;
	int hash_if_missing_line = -1;
	//removeMultilines(srcfilecontent);
	while ((pos_hash = findCharNext(mutableTempFileContent, '#', &pos_hash)) != -1)
	{
		//string_t truc = truncate(mutableTempFileContent, pos_hash+1);
	 	
		offset = pos_hash+1;
		//printf("the last line is: ");
      //  printline(mutableTempFileContent, pos_hash);
		string_t new_tok = tokenizeUsingOffset(mutableTempFileContent, &offset);
		if (compare_cstr(new_tok, "define") == 0)
		{
			//printf("the last line is: ");
			//printline(mutableTempFileContent, pos_hash);
			//string_t truc2 = truncate(trunc, pos_hash+8);
			start_pos_func = offset;
			string_t Define_var_name = tokenizeUsingOffset(mutableTempFileContent, &offset);
			ln = length(Define_var_name);
			start_pos_var = offset;

			if (mutableTempFileContent->str[offset] == ' ' || mutableTempFileContent->str[offset] == '\t' || mutableTempFileContent->str[offset] == '\v' || mutableTempFileContent->str[offset] == '\r')
			{
				offset++;
			}
			

			string_t line_rep_str = getlineByPos(mutableTempFileContent, offset);
			string_t Define_bracket_or_rep_name = tokenizeUsingOffset(mutableTempFileContent, &offset);
			printf("\nDefine_bracket_or_rep_name:%s len:%d", Define_bracket_or_rep_name->str, Define_bracket_or_rep_name->len);
			if (mutableTempFileContent->str[start_pos_var] == '(')
			{
				string_t entire_func = truncate(mutableTempFileContent, start_pos_func);
				parseHashFuncDef(entire_func, start_pos_func);
			}
			else
			{
				insert_hash_defination_var(hash_line_no, start_pos_var-ln,Define_var_name, line_rep_str);

			}
			hash_line_no++;
		}
		if (compare_cstr(new_tok, "undef") == 0)
		{
			string_t UndefVar = getlineByPos(mutableTempFileContent, offset);
			if (is_id(UndefVar) == False)
			{
				printf("error: #undef only one variable is accepted !");
				exit(0);
			}

			int index = search_hash_defination_var(UndefVar);
			if (index != -1)
			{
				delete_hash_defination_var_by_index(index);
			} // else do nothing

		}
		else if (compare_cstr(new_tok, "ifndef") == 0)
		{
			if_counter++;
			string_t new_tok = tokenizeUsingOffset(mutableTempFileContent, &offset);
		}
		else if (compare_cstr(new_tok, "ifdef") == 0)
		{
			//printf("the last line is: ");
			//printline(mutableTempFileContent, offset);
			if_counter++;
			string_t IfDef_check_name = tokenizeUsingOffset(mutableTempFileContent, &offset);
			
			if (is_id(IfDef_check_name) == False)
			{
				printf("%s: line no: %d error: Ivalid symbol name : %s\n", __func__, __LINE__, IfDef_check_name->str);
				exit(0);
			}

			hash_line_no++;
		}
		else if (compare_cstr(new_tok, "if") == 0)
		{
			if_counter++;
			if (hash_if_missing_line == -1)
			{
				hash_if_missing_line = offset;
			}
			//printf("the last line is: ");
			//printline(mutableTempFileContent, offset);
			/*string_t IfDefLine = getline(mutableTempFileContent, offset);
			printf(": calling tokenizeUsingOffset()\n");
			string_t IfDef = tokenizeUsingOffset(mutableTempFileContent, &offset);
			printf(": calling is_id()\n");
			bool_t id_flag = is_id(IfDef);
			if (is_id(IfDef) == True)
			{
				int index = search_hash_defination_var(IfDef);
				if (index != -1)
				{

				}
				else
				{

				}

			}
			else 
			{
				
				if (IfDefLine->str[0] == '(')
				{
					IfDefLine->str[0] = ' ';
					for (int k = IfDefLine->len - 1; k > 0; k--)
					{
						if (IfDefLine->str[k] == ')')
						{
							IfDefLine->str[k] = ' ';
							break;
						}
					}
				}
				struct ListNode **expr=expression(IfDefLine->str);
				printf("The expression() completed !\n");
				if (expr == NULL)
				{
					printf("%s:%d error: %s is not a valid expression in character no. %d !", __func__,__LINE__, IfDef->str, offset);
					printline(mutableTempFileContent,offset);
					exit(0);
				}
				printf("The const expr resolv\n");
				int k = ConstExprResolv(expr);
				if (k)
				{

				}
			
				printf("The const expr resolv, completed\n");
				//free(expr);
				//printf("printll is:");
				//printll(*expr);

			}*/

			hash_line_no++;
		}
		else if (compare_cstr(new_tok, "elif") == 0)
		{
			//printf("the last line is: ");
			//printline(mutableTempFileContent, offset);
			string_t ElIf = tokenizeUsingOffset(mutableTempFileContent, &offset);
			if (is_id(ElIf) == False)
			{
				printf("%s: line no: %d error: Ivalid symbol name : %s\n", __func__, __LINE__, ElIf->str);
				exit(0);
			}
			hash_line_no++;
		}
		else if (compare_cstr(new_tok, "endif") == 0)
		{
			//printf("the last line is: ");
			//printline(mutableTempFileContent, offset);
			if_counter--;
			if (hash_if_missing_line == -1)
			{
				hash_if_missing_line = offset;
			}
			hash_line_no++;
		}
		else if (compare_cstr(new_tok, "#") == 0)
		{
			//printf("the last line is: ");
			//printline(mutableTempFileContent, offset);

			hash_line_no++;
		}
		/*else if (compare_cstr(new_tok, "pragma") == 0)
		{
			hash_line_no++;
		} */

		pos_hash++;
		string_destroy_unalloc(new_tok);

	}
	if (if_counter != 0)
	{
		if (if_counter < 0)
		{
			printf("error: #ifdef is missing at pos:%d!", hash_if_missing_line);
			exit(0);
		}
		else if (if_counter > 0)
		{
			printf("error: #endif is missing at pos:%d!", hash_if_missing_line);
			exit(0);
		}
	}
	if (pos_hash == -1)
	{
		printf("completed !");
		return;
	}

}
void parser(string_t expandedsrcfilecontent, string_t OriginalFilecontent, string_t srcFile)
{

	// Order of calling the macro functions
	FillAndEvaluateAllSingleHashIfBlocks(expandedsrcfilecontent); // only the nonmacro content is required as input
	StringReplacer(expandedsrcfilecontent, OriginalFilecontent); // input the concatanated header and clang content
	OriginalSourceFileProcessing(OriginalFilecontent);
	LineAndOptimizationProcessing(OriginalFilecontent);
	//ReplacePredefinedMacros(expandedsrcfilecontent, OriginalFilecontent, srcFile);
	RemoveAllPreprocessorDirectives(expandedsrcfilecontent);
	RemoveAllPreprocessorDirectives(OriginalFilecontent);
	RemoveAllExtraEmptyLines(expandedsrcfilecontent);
	RemoveAllExtraNewLines(expandedsrcfilecontent);
}

void Init_all_structs()
{


}

string_t ChangeFileFormat(string_t originalFilename, const char* lastSuffix)
{
	int len = strlen(lastSuffix);
	string_t Suffix = string_const(lastSuffix);
	int j = 0;
	int loc = 0;
	string_t ret = NULL;
	if ((loc = findCharNext(originalFilename, '.', &loc)) != -1)
	{
		ret = partOfstring(originalFilename, 0, loc);
		ret = concat(ret, Suffix);
	}
	else
	{
		ret = copy(originalFilename);
		ret = concat(ret, Suffix);
	}
	return ret;
}

void start_cpp_driver(string_t src_file)
{
	char* tape = {};
	string_t outputFilename = NULL;
	int t = 0;
	tape = (char*)calloc(1, sizeof(char));
	tape_read(src_file->str, &tape);
	int file_length;
	string_t OriginalFilecontent = string_file(tape, &file_length);
	string_t ExpandedHeaderContent = split_hashinclude_data_from_clang(OriginalFilecontent);
	removeMultilines(ExpandedHeaderContent);
	removeMultilines(OriginalFilecontent);
	//printf("%s: %d entire combined header is:%s\n", __func__, __LINE__, ExpandedHeaderContent->str);
	//string_destroy(OriginalFilecontent);
	copy_from_hashdef_entries_to_constexpr_symbols_entries(); // copies all #define symbols to symbol entry for const expr evaluation in #if
	lexer(ExpandedHeaderContent);
	lexer(OriginalFilecontent);
	parser(ExpandedHeaderContent, OriginalFilecontent, src_file);
	ExpandedHeaderContent = concat(ExpandedHeaderContent, OriginalFilecontent);
	outputFilename = ChangeFileFormat(src_file, ".i");
	tape_write(outputFilename->str, ExpandedHeaderContent->str, ExpandedHeaderContent->len);
	string_destroy(OriginalFilecontent);
	string_destroy(ExpandedHeaderContent);
	free(tape);
}
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Error:No input file ! no of args is:%d", argc);
		exit(0);
	}

	for (int i = 1; i < argc; i++)
	{
		string_t argfile = string(argv[i]);
		int len = length(argfile);
		if ((at(argfile, len - 2) == 'c' || at(argfile, len - 2) == 'h') && at(argfile, len - 3) == '.')
		{
			start_cpp_driver(argfile);
		}
		else
		{
			printf("Error:input file is invalid !");
			exit(0);
		}
	}


}

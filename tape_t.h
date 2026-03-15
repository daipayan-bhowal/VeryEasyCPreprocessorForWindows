
#pragma once
#include <stdlib.h>
#include <stdio.h>
enum Bool
{
	FALSE,
	TRUE
};
char getch(char* Tape, int current_ptr)
{
	return Tape[current_ptr];
}
char getchIncr(char* Tape, int current_ptr)
{
	return Tape[current_ptr++];
}

int tapeIncr(int current_ptr, int i)
{
	current_ptr += i;
	return current_ptr;
}

void necessary_expect(char* Tape, int current_ptr, char expected_symbol)
{
	if (getch(Tape, current_ptr) != expected_symbol)
	{
		printf("Missing char is %c but input char is (%c)%d\n", expected_symbol, getch(Tape, current_ptr), getch(Tape, current_ptr));
		exit(0);
	}
	current_ptr++;
}
Bool optional_expect(char* Tape, int current_ptr, char expected_symbol)
{
	if (getch(Tape, current_ptr) != expected_symbol)
	{
		return FALSE;
	}
	current_ptr++;
	return TRUE;
}

void jumpline(char* Tape, int current_ptr)
{
	if (getch(Tape, current_ptr) == '\n')
	{
		current_ptr++;
		return;
	}

	while (getch(Tape, current_ptr) != '\n')
		current_ptr++;

	current_ptr++; // for reading '\n'
}

void tape_read(char* cwd, char** Tape)
{
	FILE* sys_hdr;
	errno_t error_code;
	error_code = fopen_s(&sys_hdr, cwd, "r+");
	int ch;
	long long file_size = 0;
	if (sys_hdr == NULL)
	{
		printf("\nerror: Header file(%s) doesn't exist!\n", cwd);
		exit(0);
	}

	while ((ch = fgetc(sys_hdr)) != EOF)
	{
		*(*Tape + file_size++) = ch;

		*Tape = (char*)realloc(*Tape, file_size + 1);

	}
	if ((ch = fgetc(sys_hdr)) != EOF)
	{
		*(*Tape + file_size++) = ' ';
	}

	printf("\nTape length is:%lld\n", file_size);
	*(*Tape + file_size) = '\0';
	//printf("\nTape string is:%s\n", (*Tape));
	// *(*Tape + file_size) = ' ';
	//printf("tape_read() %s", *Tape);
}

void tape_write(char* cwd, char* Tape, int length)
{
	FILE* sys_hdr;
	int i = 0;
	errno_t error_code;
	error_code = fopen_s(&sys_hdr, cwd, "w+");

	if (sys_hdr == NULL)
	{
		printf("\nerror: Preprocessor file couldn't be generated!\n", cwd);
		exit(0);
	}

	for (i = 0; i < length; i++)
	{
		fputc(Tape[i], sys_hdr);
	}

	fclose(sys_hdr);
}

void tape_readline(char* cwd, char* Tape)
{
	FILE* sys_hdr;
	errno_t error_code;
	error_code = fopen_s(&sys_hdr, cwd, "r+");
	char ch;
	int file_size = 0;
	if (sys_hdr == NULL)
	{
		printf("\nerror: Header file(%s) doesn't exist!\n", cwd);
		exit(0);
	}


	//Tape[file_size] = EOF;
	//printf("%s", Tape);
}
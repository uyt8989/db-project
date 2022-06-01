#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include "mysql.h"

const char* host = "localhost";
const char* user = "root";
const char* pw = "1234";
const char* db = "ky_db";

MYSQL* connection = NULL;
MYSQL conn;

int get_type(int*);
int read_file(MYSQL*, const char*);
void type1(MYSQL*);
void type2(MYSQL*);
void type3(MYSQL*);
void type4(MYSQL*);
void type5(MYSQL*);
void type6(MYSQL*);
void type7(MYSQL*);

int main(void) {

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	if (mysql_select_db(&conn, db))
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	printf("Connection Succeed\n");

	if (read_file(&conn, "CREATE.txt") < 0) {
		printf("Can't start program\n");
		return 0;
	}

	printf("Initialization complete!\n");
	printf("Start program\n");

	int type;
	while (get_type(&type)) {
		switch (type) {
		case 1: 
			type1(connection); 
			break;
		case 2: 
			type2(connection);
			break;
		case 3: 
			type3(connection);
			break;
		case 4: 
			type4(connection);
			break;
		case 5: 
			type5(connection);
			break;
		case 6: 
			type6(connection);
			break;
		case 7: 
			type7(connection);
			break;
		default: 
			printf("Invalid type. Try again.\n");
			break;
		}
	}

	if (read_file(&conn, "DROP.txt")) {
		printf("Terminate program falied!\n");
		printf("Check DROP.txt file\n");
		return 0;
	}

	printf("Program is terminated.\n");
	
	mysql_close(connection);

	return 0;
}
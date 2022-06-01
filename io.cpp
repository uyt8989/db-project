#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "mysql.h"

using namespace std;

/*
	** REATURN VALUE **
	0 : Get 0
	1 : Get ordinary type
*/

int get_type(int* type) {
	printf("\n------- SELECT QUERY TYPES -------\n\n");
	printf("\t1. TYPE 1\n");
	printf("\t2. TYPE 2\n");
	printf("\t3. TYPE 3\n");
	printf("\t4. TYPE 4\n");
	printf("\t5. TYPE 5\n");
	printf("\t6. TYPE 6\n");
	printf("\t7. TYPE 7\n");
	printf("\t0. QUIT\n");
	printf("\n----------------------------------\n");

	printf("Select type : ");
	(void)scanf("%d", type);

	if (*type == 0) return 0;
	return 1;
}

/* 
	** REATURN VALUE **
	 0 : Success
	-1 : Fail
*/
int read_file(MYSQL *conn, const char *filename) {
	ifstream file(filename);
	string line;

	// open the file
	if (!file.is_open()) {
		printf("File open error\n");
		printf("File name : %s\n", filename);
		return -1;
	}

	// read file line by line
	while (getline(file, line)) {
		// make guery
		if (mysql_query(conn, line.c_str())){
			printf("Query error\n");
			printf("Error query : %s\n", line.c_str());
			return -1;
		}
	}

	return 0;
}
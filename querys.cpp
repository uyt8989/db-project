#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "mysql.h"

using namespace std;

typedef struct SHPPING {
	// tracking_number, company, destination, status, promised_time
	string no, cop, dest, st, pt;
} shipping_t;

void type1(MYSQL* connection) {
	int type, state, cnt;
	char query[200], new_tracking[10];
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	vector<string> order_ids;
	vector<string> new_tracking_numbers;
	vector<shipping_t> shippings;

	printf("\n---- TYPE 1 ----\n\n");
	
	/* find all shippings that is delivered by USPS */
	sprintf(query, "select shipping.*, order.id \
					from ky_db.shipping, ky_db.order\
					where order.tracking_number = shipping.tracking_number and\
					company = 'USPS' and status = 'Shipping'");
	state = mysql_query(connection, query);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
			shippings.push_back({ sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4] });
			order_ids.push_back(sql_row[5]);
		}
		mysql_free_result(sql_result);
	}
	
	/* find customer's information */
	printf("Order_id\tCostomer_name\tEmail\n");
	printf("------------------------------------------\n");
	for (auto iter : order_ids) {
		sprintf(query, "select customer.name, customer.email\
						from ky_db.customer, ky_db.order\
						where order.id='%s' and order.cid=customer.id", iter.c_str());
		state = mysql_query(connection, query);
		if (state == 0)
		{
			sql_result = mysql_store_result(connection);
			sql_row = mysql_fetch_row(sql_result);
			printf("%s\t%s\t\t%s\n", iter.c_str(), sql_row[0], sql_row[1]);
			mysql_free_result(sql_result);
		}
	}

	/* get subtype from user */
	printf("\n---- Subtypes in TYPE 1 ----\n");
	printf("\t1. TYPE 1-1\n");
	printf("\nSelect type : ");
	(void)scanf("%d", &type);
	while (type != 1) {
		printf("Invalid type!\n");
		printf("Select type again : ");
		(void)scanf("%d", &type);
	}

	/* find the content of shipment */
	printf("\nOrder_id\tProduct_name\n");
	printf("----------------------------\n");
	for (auto iter : order_ids) {
		sprintf(query, "select product.name\
						from ky_db.order, ky_db.product\
						where order.id='%s' and order.pid=product.id", iter.c_str());
		state = mysql_query(connection, query);
		if (state == 0)
		{
			sql_result = mysql_store_result(connection); 
			sql_row = mysql_fetch_row(sql_result);
			printf("%s\t%s\n", iter.c_str(), sql_row[0]);
			mysql_free_result(sql_result);
		}
	}

	/* get number of total tracking numbers */
	sprintf(query, "select count(tracking_number) FROM ky_db.shipping");
	state = mysql_query(connection, query);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		sql_row = mysql_fetch_row(sql_result);
		cnt = atoi(sql_row[0]);
		mysql_free_result(sql_result);
	}

	/* update status of shipment : shipping -> cancled */
	for (auto iter : shippings) {
		sprintf(query, "update ky_db.shipping set status='Cancled' where tracking_number='%s'", iter.no.c_str());
		state = mysql_query(connection, query);
	}

	/* create new shipment */
	printf("\nTrackinng_number -> new Trackinng_number\n");
	printf("----------------------------------------\n");
	for (auto iter : shippings) {
		sprintf(new_tracking, "no%06d", ++cnt);
		new_tracking_numbers.push_back(new_tracking);
		printf("%s\t->\t%s\n", iter.no.c_str(), new_tracking);
		sprintf(query, "insert into ky_db.shipping values ('%s','USPS','%s','Shipping','%s')", 
											new_tracking, iter.dest.c_str(), iter.pt.c_str());
		state = mysql_query(connection, query);
	}
	
	/* change order's tracking number */
	for (int i = 0; i < order_ids.size(); i++) {
		sprintf(query, "update ky_db.order set tracking_number='%s' where id='%s'", 
			new_tracking_numbers[i].c_str(), order_ids[i].c_str());
		state = mysql_query(connection, query);
	}

	return;
}

void type2(MYSQL* connection) {
	int type, state;
	char query[300], name[20];
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	printf("\n---- TYPE 2 ----\n\n");
	/* find the customer who bougth the most */
	sprintf(query, "select order.cid, sum(tot_price)\
					FROM ky_db.order\
					where year(time) = left(date_sub(now(), INTERVAL 1 year), 4)\
					group by order.cid\
					order by sum(tot_price) desc\
					limit 1");

	state = mysql_query(connection, query);
	if (state == 0)
	{
		printf("Customer_id\tSum\n");
		printf("----------------------------\n");
		sql_result = mysql_store_result(connection);
		sql_row = mysql_fetch_row(sql_result);
		strcpy(name, sql_row[0]);
		printf("%s\t%s\n", sql_row[0], sql_row[1]);
		mysql_free_result(sql_result);
	}

	/* get new input from user */
	printf("\n---- Subtypes in TYPE 2 ----\n");
	printf("\t1. TYPE 2-1\n");
	printf("\nSelect type : ");
	(void)scanf("%d", &type);
	while (type != 1) {
		printf("Invalid type!\n");
		printf("Select type again : ");
		(void)scanf("%d", &type);
	}

	/* find the product that the customer bought the most */
	sprintf(query, "select order.pid, sum(tot_price)\
					from ky_db.order\
					where order.cid='%s' and year(time) = left(date_sub(now(), INTERVAL 1 year), 4)\
					group by order.pid\
					order by sum(tot_price) desc\
					limit 1", name);

	state = mysql_query(connection, query);
	if (state == 0)
	{
		printf("\nProduct_id\tSum\n");
		printf("----------------------------\n");
		sql_result = mysql_store_result(connection);
		sql_row = mysql_fetch_row(sql_result);
		printf("%s\t%s\n", sql_row[0], sql_row[1]);
		mysql_free_result(sql_result);
	}

	return;
}

void type3(MYSQL* connection) {
	int type, state, k, cnt = 0;
	char query[300];
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	printf("\n---- TYPE 3 ----\n\n");

	/* find the all products sold in the past */
	printf("Product_Id\tSum\n");
	printf("--------------------\n");
	sprintf(query, "select order.pid, sum(tot_price)\
					from ky_db.order\
					where year(time) = left(date_sub(now(), INTERVAL 1 year), 4)\
					group by pid");
	
	state = mysql_query(connection, query);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
			printf("%s\t%s\n", sql_row[0], sql_row[1]);
			cnt++;
		}
		mysql_free_result(sql_result);	
	}

	/* get subtype from user */
	printf("\n---- Subtypes in TYPE 3 ----\n");
	printf("\t1. TYPE 3-1\n");
	printf("\t2. TYPE 3-2\n");
	printf("\nSelect type : ");
	(void)scanf("%d", &type);
	while (type < 1 || type > 2) {
		printf("Invalid type!\n");
		printf("Select type again : ");
		(void)scanf("%d", &type);
	}

	
	switch (type) {
		/* find the top k products by dollor-amount */
	case 1: 
		printf("\nFind top k products. Input K : ");
		(void)scanf("%d", &k);
		while (k > cnt || k < 0) {
			printf("Invalid K!\n");
			printf("Select K again : ");
			(void)scanf("%d", &k);
		}

		sprintf(query, "select distinct order.pid, sum(tot_price)\
						from ky_db.order \
						where year(time) = left(date_sub(now(), INTERVAL 1 year), 4) \
						group by pid\
						order by sum(tot_price) desc\
						limit %d", k);
		break;
		/* find the top 10% products by dollor-amount */
	case 2:
		printf("Find top 10%%(%d products) products.\n", (int)ceil(cnt / 10) + 1);
		sprintf(query, "select distinct order.pid, sum(tot_price)\
						from ky_db.order \
						where year(time) = left(date_sub(now(), INTERVAL 1 year), 4) \
						group by pid\
						order by sum(tot_price) desc\
						limit %d", (int)ceil(cnt / 10) + 1);
		break;
	default :
		printf("Invalid type!\n");
	}

	/* find top k products or top 10% sold in the past by dollor-amount */
	state = mysql_query(connection, query);
	if (state == 0)
	{
		printf("\nProduct_Id\tSum\n");
		printf("--------------------\n");
		sql_result = mysql_store_result(connection);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
			printf("%s\t%s\n", sql_row[0], sql_row[1]);
		}
		mysql_free_result(sql_result);
	}
}

void type4(MYSQL* connection) {
	int type, state, k, cnt = 0;
	char query[300];
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	printf("\n---- TYPE 4 ----\n\n");

	/* find the all products sold in the past */
	printf("Product_Id\tSum\n");
	printf("--------------------\n");
	sprintf(query, "select order.pid, sum(quantity)\
					from ky_db.order\
					where year(time) = left(date_sub(now(), INTERVAL 1 year), 4)\
					group by pid");

	state = mysql_query(connection, query);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
			printf("%s\t%s\n", sql_row[0], sql_row[1]);
			cnt++;
		}
		mysql_free_result(sql_result);
	}

	/* get subtype from user */
	printf("\n---- Subtypes in TYPE 4 ----\n");
	printf("\t1. TYPE 4-1\n");
	printf("\t2. TYPE 4-2\n");
	printf("\nSelect type : ");
	(void)scanf("%d", &type);
	while (type < 1 || type > 2) {
		printf("Invalid type!\n");
		printf("Select type again : ");
		(void)scanf("%d", &type);
	}

	switch (type) {
		/* find top k products by unit sales */
	case 1:
		printf("\nFind top k products. Input K : ");
		(void)scanf("%d", &k);
		while (k > cnt || k < 0) {
			printf("Invalid K!\n");
			printf("Select K again : ");
			(void)scanf("%d", &k);
		}

		sprintf(query, "select distinct order.pid, sum(quantity)\
						from ky_db.order \
						where year(time) = left(date_sub(now(), INTERVAL 1 year), 4) \
						group by pid\
						order by sum(quantity) desc\
						limit %d", k);
		break;
		/* find top 10% products by unit sales */
	case 2:
		printf("Find top 10%%(%d products) products.\n", (int)ceil(cnt / 10) + 1);
		sprintf(query, "select distinct order.pid, sum(quantity)\
						from ky_db.order \
						where year(time) = left(date_sub(now(), INTERVAL 1 year), 4) \
						group by pid\
						order by sum(quantity) desc\
						limit %d", (int)ceil(cnt / 10) + 1);
		break;
	default:
		printf("Invalid type!\n");
	}

	/* find top k products or top 10% sold in the past */
	printf("\nProduct_Id\tSum\n");
	printf("--------------------\n");
	state = mysql_query(connection, query);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
			printf("%s\t%s\n", sql_row[0], sql_row[1]);
		}
		mysql_free_result(sql_result);
	}
}

void type5(MYSQL* connection) {
	int state;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	printf("\n---- TYPE 5 ----\n\n");

	/* find thos products that are out-of-stock at every store in California */
	const char* query = "select inventory.sid, inventory.pid, product.name\
						from ky_db.inventory, ky_db.product\
						where product.id = inventory.pid and inventory.sid in(select store.id\
																			from ky_db.store\
																			where store.state = 'California')";
	state = mysql_query(connection, query);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		printf("Store_id\tProduct_id\tProduct_name\n");
		printf("--------------------------------------------\n");
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			printf("%s\t%s\t%s\n", sql_row[0], sql_row[1], sql_row[2]);
		}
		mysql_free_result(sql_result);
	}
}

void type6(MYSQL* connection) {
	int type, state, k, cnt = 0;
	char query[200];
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	printf("\n---- TYPE 6 ----\n\n");

	/* find those packages that were not delivered within the promised time */
	sprintf(query, "select tracking_number, promised_time\
					from ky_db.shipping\
					where promised_time < now() and status != 'Complete'");
	state = mysql_query(connection, query);

	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		printf("Tracking_number\tPromised_time\n");
		printf("----------------------\n");
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			printf("%s\t%s\n", sql_row[0], sql_row[1]);
		}
		mysql_free_result(sql_result);
	}
}

void type7(MYSQL* connection) {
	int state;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	printf("\n---- TYPE 7 ----\n\n");
	/* generate the bill for each customer for the past month */
	const char* query = "select ky_db.order.cid, sum(ky_db.order.tot_price)\
						from ky_db.order\
						where left(ky_db.order.time, 7) = left(DATE_SUB(now(), INTERVAL 1 month), 7)\
						group by ky_db.order.cid";

	state = mysql_query(connection, query);

	if (state == 0)
	{
		sql_result = mysql_store_result(connection);
		printf("Customer_id\tBill\n");
		printf("----------------------\n");
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
		{
			printf("%s\t%s\n", sql_row[0], sql_row[1]);
		}
		mysql_free_result(sql_result);
	}

}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


void execute_query(char *query, SQLHSTMT stmt){
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
    SQLCloseCursor(stmt);
}


int main(int argc, char **argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret;
    SQLINTEGER film_id;
    char query[512];

    if (argc != 3){
        printf("Número de parámetros incorrecto\n");
        return EXIT_FAILURE;
    }

    else if(strcmp(argv[1], "remove") != 0){
        printf("Esa funcíon no existe.\n");
        return EXIT_FAILURE;
    }

    film_id = atoi(argv[2]);

    if(film_id <= 0){
        printf("El identificador no es válido.\n");
        return EXIT_FAILURE;
    }

    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    sprintf(query, "DELETE FROM film_actor WHERE film_id ='%d';", film_id);
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    sprintf(query, "DELETE FROM film_category WHERE film_id = '%d';", film_id);
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    sprintf(query, "DELETE FROM payment WHERE rental_id IN (SELECT rental_id FROM rental WHERE inventory_id IN (SELECT inventory_id FROM inventory WHERE film_id = '%d'));", film_id);
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    sprintf(query,"DELETE FROM rental WHERE inventory_id IN (SELECT inventory_id FROM inventory WHERE film_id = '%d');",film_id);
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    sprintf(query, "DELETE FROM inventory WHERE film_id = '%d';", film_id);
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    sprintf(query, "DELETE FROM film WHERE film_id = '%d';", film_id);
    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    printf("Se ha borrado la película.\n");

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

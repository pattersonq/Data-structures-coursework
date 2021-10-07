#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <time.h>
#include "odbc.h"


int main(int argc, char **argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLINTEGER inventory_id, new_rental_id, new_payment_id, res, inv_copy, rent_copy, pay_copy;
    SQLSMALLINT columns;



    int i=2, j=0, cont = 0;
    char tablas[4][9] = {"customer", "film", "staff", "store"};
    char claves[4][12] = {"customer_id", "film_id", "staff_id", "store_id"};
    char query_check [512], tiempo[512], query[512];
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(tiempo, "%d-%d-%d %d:%d:%d",timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);


    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    if (argc != 7 && argc != 3) {
    fprintf(stderr, "Error en los parametros de entrada\n\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);

    return EXIT_FAILURE;
    }


  /* comprueba la linea de comandos */
    if (strcmp(argv[1], "new") == 0) {

      while(i < argc-1){
        sprintf(query_check, "select %s from %s where %s = %s;",claves[j], tablas[j], claves[j], argv[i]);

        SQLExecDirect(stmt, (SQLCHAR*) query_check, SQL_NTS);

        SQLBindCol(stmt, 1, SQL_C_SLONG, &res, sizeof(res), NULL);

        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
          if (atoi(argv[i]) != res){
            fprintf(stderr, "Error en los parametros de entrada\n\n");
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            /* DISCONNECT */
            ret = odbc_disconnect(env, dbc);

            return EXIT_FAILURE;
          }
          else{
            printf("Parametro %d correcto\n", i+1);
          }
        }

        if (i == 2){
          SQLCloseCursor(stmt);
          sprintf(query_check, "select store_id from customer where customer_id = %s;", argv[2]);
          SQLExecDirect(stmt, (SQLCHAR*) query_check, SQL_NTS);

          SQLBindCol(stmt, 1, SQL_C_SLONG, &res, sizeof(res), NULL);

          while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            if (atoi(argv[5]) != res){
              fprintf(stderr, "El customer no compra en esta tienda\n\n");
              SQLFreeHandle(SQL_HANDLE_STMT, stmt);
              /* DISCONNECT */
              ret = odbc_disconnect(env, dbc);

              return EXIT_FAILURE;
            }
          }
        }
        if (i == 4){
          SQLCloseCursor(stmt);
          sprintf(query_check, "select store_id from staff where staff_id = %s;", argv[4]);
          SQLExecDirect(stmt, (SQLCHAR*) query_check, SQL_NTS);

          SQLBindCol(stmt, 1, SQL_C_SLONG, &res, sizeof(res), NULL);

          while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            if (atoi(argv[5]) != res){
              fprintf(stderr, "El staff no trabaja en esa tienda\n\n");
              SQLFreeHandle(SQL_HANDLE_STMT, stmt);
              /* DISCONNECT */
              ret = odbc_disconnect(env, dbc);

              return EXIT_FAILURE;
            }
          }
        }
        j++;
        i++;
        SQLCloseCursor(stmt);
      }

      sprintf(query, "SELECT inventory.inventory_id FROM inventory, rental WHERE rental.inventory_id = inventory.inventory_id AND store_id = %s AND film_id = %s AND rental.return_date < '%s' UNION SELECT inventory_id FROM inventory WHERE  store_id = %s AND film_id = %s AND inventory_id NOT IN ( SELECT inventory_id FROM rental) LIMIT 1;", argv[5], argv[3], tiempo, argv[5], argv[3]);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error al buscar un inventory disponible\n" );
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      SQLBindCol(stmt, 1, SQL_C_SLONG, &inv_copy, sizeof(inv_copy), NULL);
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          inventory_id = inv_copy;
          cont++;
      }

      if (cont == 0){
        printf("Error al buscar un inventory disponible\n" );
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("\nSe ha encontrado un inventory disponible\n" );
      }

      cont = 0;

      SQLCloseCursor(stmt);

      sprintf(query, "SELECT rental_id +1  FROM rental ORDER BY rental_id DESC LIMIT 1;");

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error reservando el alquiler\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }

      SQLBindCol(stmt, 1, SQL_C_SLONG, &rent_copy, sizeof(rent_copy), NULL);

      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          new_rental_id = rent_copy;
          cont++;
      }

      if (cont == 0){
        printf("Error reservando el alquiler\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("\nSe ha reservado el alquiler\n" );
      }

      cont = 0;

      SQLCloseCursor(stmt);


      sprintf(query, "INSERT INTO rental (rental_id, rental_date, inventory_id, customer_id, staff_id, last_update) VALUES (%d, '%s', %d, %s, %s, '%s');", new_rental_id, tiempo, inventory_id, argv[2], argv[4], tiempo);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLCloseCursor(stmt);

      sprintf(query, "SELECT * FROM rental WHERE rental_id = %d AND rental_date = '%s' AND inventory_id = %d AND  customer_id = %s AND staff_id = %s AND last_update = '%s';", new_rental_id, tiempo, inventory_id, argv[2], argv[4], tiempo);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error al insertar valores en rental\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          cont++;
      }

      if (cont == 0){
        printf("Error al insertar valores en rental\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("\nSe han insertado los datos del alquiler en la base de datos\n" );
      }

      cont = 0;


      SQLCloseCursor(stmt);

      sprintf(query, "SELECT payment_id +1  FROM payment ORDER BY payment DESC LIMIT 1;");

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error reservando el pago\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }

      SQLBindCol(stmt, 1, SQL_C_SLONG, &pay_copy, sizeof(pay_copy), NULL);
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          new_payment_id = pay_copy;
          cont++;
      }

      if (cont == 0){
        printf("Error reservando el pago\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("\nSe ha reservado el pago\n" );
      }

      cont = 0;

      SQLCloseCursor(stmt);

      sprintf(query, "INSERT INTO payment VALUES (%d, %s, %s, %d, %s, '%s');", new_payment_id, argv[2], argv[4], new_rental_id, argv[6], tiempo);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLCloseCursor(stmt);

      sprintf(query, "SELECT * FROM payment WHERE payment_id = %d AND customer_id = %s AND staff_id = %s AND  rental_id = %d AND amount = %s AND payment_date = '%s';", new_payment_id, argv[2], argv[4], new_rental_id, argv[6], tiempo);
      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error al insertar valores en payment\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          cont++;
      }

      if (cont == 0){
        printf("Error al insertar valores en payment\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("\nSe han insertado los datos del pago en la base de datos\n" );
      }

      cont = 0;

      SQLCloseCursor(stmt);
      printf ("\nAlquiler con id: %d realizado correctamente\n", new_rental_id);



    } else if (strcmp(argv[1], "remove") == 0) {

      char query_check [512];
      sprintf(query_check, "select rental_id from rental where rental_id = %s;", argv[2]);

      SQLExecDirect(stmt, (SQLCHAR*) query_check, SQL_NTS);


      SQLBindCol(stmt, 1, SQL_C_SLONG, &res, sizeof(res), NULL);

      while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        cont++;
      }
      if (cont == 0){
        fprintf(stderr, "Error en los parametros de entrada\n\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("Parametros correctos\n");
      }
      cont=0;

      SQLCloseCursor(stmt);

      sprintf(query, "DELETE FROM payment CASCADE WHERE rental_id = %s;", argv[2]);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLCloseCursor(stmt);

      sprintf(query, "SELECT * FROM payment WHERE rental_id = %s;", argv[2]);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error al borrar el pago\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          cont++;
      }

      if (cont != 0){
        printf("Error al borrar el pago\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("Se ha borrado el pago\n");
      }

      cont = 0;



      SQLCloseCursor(stmt);

      sprintf(query, "DELETE FROM rental CASCADE WHERE rental_id = %s;", argv[2]);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      SQLCloseCursor(stmt);

      sprintf(query, "SELECT * FROM rental WHERE rental_id = %s;", argv[2]);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
      SQLNumResultCols(stmt, &columns);
      if (columns == 0){
        printf("Error al borrar el alquiler\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          cont++;
      }

      if (cont != 0){
        printf("Error al borrar el alquiler\n");
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);

        return EXIT_FAILURE;
      }
      else{
        printf("Se ha borrado el alquiler\n");
      }

      cont = 0;


      SQLCloseCursor(stmt);

    } else {
      fprintf(stderr, "Parametro %s es incorrecto\n", argv[1]);
    }

    SQLCloseCursor(stmt);


    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

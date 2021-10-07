#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int main(int argc, char **argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret;
    char query[2048];
    double amount;
    SQLSMALLINT columns;
    SQLINTEGER customer_id, film_id, rental_id, release_year, length, postal_code, flag, i, staff_id, store_id, category_id;
    char first_name[256], rental_date[256], last_name[256],create_date[256],address[256],address2[256],district[256],city[256],country[256], phone[256], title[256], language[256], full_text[256], film[256];


    if (argc > 6){
      printf("Número de parámetros incorrecto.");
      return -1;
    }

    if(strcmp(argv[1],"customer") == 0){
      flag = 1;
    }
    else if(strcmp(argv[1], "film") == 0){
      flag = 2;
    }
    else if(strcmp(argv[1], "rent") == 0){
      flag = 3;
    }
    else if(strcmp(argv[1], "recommend")==0){
      flag = 4;
    }
    else{
      printf("El programa que intentas ejecutar no existe.");
      return -1;
    }


  /*
  CONNECT
  */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
    /*
    Allocates memory for stmt
    */

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    if (flag == 1){
      if(argc != 6){
        printf("Número de parámetros incorrecto.");
        return -1;
      }
      else if(strcmp(argv[2],"-n")!= 0 || strcmp(argv[4], "-a") != 0){
        printf("Parámetros de entrada para el programa dvdrent customer incorrectos");
        return -1;
      }
      else{
        sprintf(query, "SELECT customer_id, first_name, last_name, create_date, address, address2, district, city, country_id, postal_code, phone FROM customer, address, city WHERE customer.address_id = address.address_id AND address.city_id = city.city_id AND city.country_id = city.country_id AND (customer.first_name = \'%s\' OR customer.last_name = \'%s\');", argv[3], argv[5]);

        printf("%s\n", query);
        fflush(stdout);

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
        SQLNumResultCols(stmt, &columns);


        for (i = 1; i <= columns; i++) {
            SQLCHAR buf[512];
            SQLDescribeCol(stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
            printf("%s\t", buf);
        }
        printf("\n");


        SQLBindCol(stmt, 1,SQL_C_SLONG, &customer_id, sizeof(customer_id), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, first_name, sizeof(first_name), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, last_name, sizeof(last_name), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, create_date, sizeof(create_date), NULL);
        SQLBindCol(stmt, 5, SQL_C_CHAR, address, sizeof(address), NULL);
        SQLBindCol(stmt, 6, SQL_C_CHAR, address2, sizeof(address2), NULL);
        SQLBindCol(stmt, 7, SQL_C_CHAR, district, sizeof(district), NULL);
        SQLBindCol(stmt, 8, SQL_C_CHAR, city, sizeof(city), NULL);
        SQLBindCol(stmt, 9, SQL_C_CHAR, country, sizeof(country), NULL);
        SQLBindCol(stmt, 10, SQL_C_SLONG, &postal_code, sizeof(postal_code), NULL);
        SQLBindCol(stmt, 11, SQL_C_CHAR, &phone, sizeof(phone), NULL);

        while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          fprintf(stdout, "%d, %s, %s, %s, %s, %s, %s, %s, %s, %d, %s\n", customer_id, first_name, last_name, create_date, address, address2, district, city, country, postal_code, phone);
        }
      }
    }
    else if(flag == 2){
      if(argc != 3){
        printf("Número de parámetros incorrecto.");
        return -1;
      }
      else{
        char film_test[128] = "test";
        sprintf(query, "SELECT film_id FROM film WHERE film.title LIKE '%%%s%%';", argv[2]);
        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
        SQLBindCol(stmt, 1,SQL_C_CHAR, film_test, sizeof(film_test), NULL);
        ret = SQLFetch(stmt);

        if(!SQL_SUCCEEDED(ret)){
          printf("La película que buscas no existe.\n");
          return EXIT_SUCCESS;
        }

        SQLCloseCursor(stmt);

        sprintf(query, "SELECT film.film_id, title, release_year, length, language.name, fulltext FROM film, language WHERE film.language_id = language.language_id AND film.title LIKE '%%%s%%';", argv[2]);
        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
        SQLNumResultCols(stmt, &columns);

        for (i = 1; i <= columns; i++) {
          SQLCHAR buf[512];
          SQLDescribeCol(stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
          printf("%s\t", buf);
        }
        printf("\n");

        SQLBindCol(stmt, 1,SQL_C_SLONG, &film_id, sizeof(film_id), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, title, sizeof(title), NULL);
        SQLBindCol(stmt, 3, SQL_C_SLONG, &release_year, sizeof(release_year), NULL);
        SQLBindCol(stmt, 4, SQL_C_SLONG, &length, sizeof(length), NULL);
        SQLBindCol(stmt, 5, SQL_C_CHAR, language, sizeof(language), NULL);
        SQLBindCol(stmt, 6, SQL_C_CHAR, full_text, sizeof(full_text), NULL);

        while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          fprintf(stdout, "%d | %s | %d | %d | %s | %s | ", film_id, title, release_year, length, language, full_text);
          char query2[512];
          SQLHSTMT stmt2;
          SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt2);

          sprintf(query2, "SELECT actor.first_name, actor.last_name FROM film, film_actor, actor WHERE film.film_id = film_actor.film_id AND film_actor.actor_id = actor.actor_id AND film.film_id = '%d';", film_id);

          SQLExecDirect(stmt2, (SQLCHAR*) query2, SQL_NTS);

          SQLBindCol(stmt2, 1,SQL_C_CHAR, first_name, sizeof(first_name), NULL);
          SQLBindCol(stmt2, 2, SQL_C_CHAR, last_name, sizeof(last_name), NULL);

          while(SQL_SUCCEEDED(ret = SQLFetch(stmt2))){
            fprintf(stdout, "%s %s, ", first_name, last_name);
          }

          SQLCloseCursor(stmt2);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt2);

          printf("\n");
        }
      }
    }
    else if(flag == 3){
      if(argc != 5){
        printf("Número de parámetros incorrecto.");
        return -1;
      }
      else{
        sprintf(query, "SELECT rental.rental_id, rental_date, film.film_id, title, staff.staff_id, staff.store_id, amount FROM rental, payment, staff, inventory, film WHERE	rental.inventory_id = inventory.inventory_id AND inventory.film_id = film.film_id AND rental.staff_id = staff.staff_id AND payment.rental_id = rental.rental_id AND rental.customer_id = %d AND rental_date > '%s' AND rental_date < '%s' ORDER BY rental_date", atoi(argv[2]), argv[3], argv[4]);
        fprintf(stdout, "%s\n", query);

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
        SQLNumResultCols(stmt, &columns);


        for (i = 1; i <= columns; i++) {
            SQLCHAR buf[512];
            SQLDescribeCol(stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
            printf("%s\t", buf);
        }
        printf("\n");

        SQLBindCol(stmt, 1,SQL_C_SLONG, &rental_id, sizeof(rental_id), NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, rental_date, sizeof(rental_date), NULL);
        SQLBindCol(stmt, 3, SQL_C_SLONG, &film_id, sizeof(film_id), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, title, sizeof(title), NULL);
        SQLBindCol(stmt, 5, SQL_C_SLONG, &staff_id, sizeof(staff_id), NULL);
        SQLBindCol(stmt, 6, SQL_C_CHAR, first_name, sizeof(first_name), NULL);
        SQLBindCol(stmt, 7, SQL_C_SLONG, &store_id, sizeof(store_id), NULL);
        SQLBindCol(stmt, 8, SQL_C_NUMERIC, &amount, sizeof(amount), NULL);

        while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          fprintf(stdout, "%d, %s, %d, %s, %d, %s, %d, %f\n", rental_id, rental_date, film_id, title, staff_id, first_name, store_id, amount);
        }
      }
    }

    else if (flag == 4){
      if(argc != 3){
        printf("Número de parámetros incorrecto.");
        return -1;
      }
      sprintf(query, "SELECT film_category.category_id, Count(*) suma  FROM   rental,inventory, film, film_category, customer WHERE inventory.inventory_id = rental.inventory_id AND rental.customer_id = customer.customer_id AND inventory.film_id = film.film_id AND film_category.film_id = film.film_id AND customer.customer_id = '%d'  GROUP  BY film_category.category_id  ORDER  BY suma DESC  LIMIT  1;", atoi(argv[2]));

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      SQLBindCol(stmt, 1, SQL_C_SLONG, &category_id, sizeof(category_id), NULL);
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          fprintf(stdout, "Categoría más vista: %d\nPelículas recomendadas: \n", category_id);
      }

      SQLCloseCursor(stmt);

      sprintf(query, "  SELECT film.title,         Count(*) cuenta  FROM   rental,         inventory,         film,         film_category  WHERE  inventory.inventory_id = rental.inventory_id         AND inventory.film_id = film.film_id         AND film_category.film_id = film.film_id         AND film_category.category_id ='%d'	 AND film.film_id NOT IN (SELECT film.film_id FROM film, inventory, rental WHERE inventory.inventory_id = rental.inventory_id AND inventory.film_id = film.film_id AND rental.customer_id = '%d')  GROUP  BY film.title  ORDER  BY cuenta DESC  LIMIT 3;", category_id, atoi(argv[2]));
      
      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      SQLBindCol(stmt, 1, SQL_C_CHAR, film, sizeof(film), NULL);
      while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
          fprintf(stdout, "%s\n", film);
      }
    }
    SQLCloseCursor(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

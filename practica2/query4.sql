DROP view IF EXISTS categoria_customer CASCADE;

CREATE view categoria_customer
AS
  SELECT film_category.category_id,
         Count(*) suma
  FROM   rental,
         inventory,
         film,
         film_category,
         customer
  WHERE  inventory.inventory_id = rental.inventory_id
         AND rental.customer_id = customer.customer_id
         AND inventory.film_id = film.film_id
         AND film_category.film_id = film.film_id
         AND customer.customer_id = '%d'
  GROUP  BY film_category.category_id
  ORDER  BY suma DESC
  LIMIT  1;

DROP view IF EXISTS peliculas_alquiladas_categoria CASCADE;

CREATE view peliculas_alquiladas_categoria
AS
  SELECT film.title,
         Count(*) cuenta
  FROM   rental,
         inventory,
         film,
         film_category
  WHERE  inventory.inventory_id = rental.inventory_id
         AND inventory.film_id = film.film_id
         AND film_category.film_id = film.film_id
         AND film_category.category_id ='15'
	 AND film.film_id NOT IN (SELECT film.film_id FROM film, inventory, rental WHERE inventory.inventory_id = rental.inventory_id AND inventory.film_id = film.film_id AND rental.customer_id = '%d')
  GROUP  BY film.title,
  ORDER  BY cuenta DESC
  LIMIT 3;

SELECT peliculas_alquiladas_categoria.title
FROM   peliculas_alquiladas_categoria
WHERE  peliculas_alquiladas_categoria.film_id NOT IN (SELECT film.film_id
                                                      FROM   film,
                                                             inventory,
                                                             rental
                                                      WHERE
              inventory.inventory_id = rental.inventory_id
              AND inventory.film_id = film.film_id
              AND rental.customer_id = '%d')
LIMIT  3;  

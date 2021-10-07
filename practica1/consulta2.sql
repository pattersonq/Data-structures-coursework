drop view if exists rents;
create view rents AS
(	SELECT   COUNT(*) peliculas, customer.customer_id
	FROM customer, rental
	WHERE customer.customer_id = rental.customer_id
	GROUP BY customer.customer_id
	);
	
SELECT  peliculas, customer.customer_id, customer.first_name, customer.last_name
FROM rents natural join customer
WHERE peliculas = (SELECT MAX(peliculas)
			FROM rents 
		   );

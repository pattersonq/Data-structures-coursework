DROP VIEW IF EXISTS num_pelis;

CREATE VIEW num_pelis
AS
  (SELECT language_id,
          Sum(rents_por_peli) rents_por_lang
   FROM   (SELECT Sum(rents) rents_por_peli,
                  film_id
           FROM   (SELECT COUNT(rental_id) rents,
                          inventory.inventory_id,
                          film_id
                   FROM   rental,
                          inventory
                   WHERE  rental.inventory_id = inventory.inventory_id
                   GROUP  BY inventory.inventory_id
                   ORDER  BY film_id) T
           GROUP  BY film_id
           ORDER  BY film_id) Q
          natural JOIN film
   GROUP  BY language_id);

SELECT LANGUAGE.name
FROM   LANGUAGE
       natural JOIN num_pelis
WHERE  rents_por_lang = (SELECT Max(rents_por_lang)
                         FROM   num_pelis); 

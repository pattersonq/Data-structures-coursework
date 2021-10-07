SELECT DISTINCT city_id,
                city.city
FROM   city
WHERE  city_id IN (SELECT city_id
                   FROM   address
                   WHERE  address_id IN (SELECT address_id
                                         FROM   customer
                                         WHERE  customer_id IN (SELECT
                                                customer_id
                                                                FROM   rental
                                                                WHERE
                                                inventory_id IN (SELECT
                                                inventory_id
                                                                 FROM
                                                inventory
                                                                 WHERE
                                                film_id IN (SELECT film_id
                                                            FROM   film_actor,
                                                                   actor
                                                            WHERE
                                                film_actor.actor_id =
                                                actor.actor_id
                                                AND first_name = 'Bob'
                                                AND last_name = 'Fawcett')))))
ORDER  BY city.city;   

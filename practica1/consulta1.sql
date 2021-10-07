SELECT Count (*), 
       Extract (year FROM rental_date) 
FROM   film 
       natural JOIN inventory AS inventory(inventory_id, film_id, store_id, 
                    last_upd) 
       natural JOIN rental AS rental(rental_id, rental_date, inventory_id, 
                    customer_id, 
                    return_date, 
                               staff_id 
       , last_up) 
GROUP  BY Extract (year FROM rental_date) 
ORDER  BY 1; 

SELECT Count (*),
       Extract (year FROM rental_date)
FROM   rental
GROUP  BY Extract (year FROM rental_date)
ORDER  BY 1;  

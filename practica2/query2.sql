SELECT film.film_id, title, release_year, length, language.name, fulltext, actor.first_name, actor.last_name
FROM film, language, film_actor, actor
WHERE film.language_id = language.language_id AND film_actor.film_id = film.film_id AND film.title = 'Academy Dinosaur'
GROUP BY film.film_id, language.name, actor.first_name, actor.last_name
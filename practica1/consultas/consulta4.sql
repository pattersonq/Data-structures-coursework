DROP VIEW IF EXISTS pelis_mismo_idioma;

CREATE VIEW pelis_mismo_idioma
AS
  SELECT COUNT(*) AS num_pelis,
         language_id
  FROM   film
  GROUP  BY language_id;

SELECT LANGUAGE.name
FROM   LANGUAGE
WHERE  language_id IN (SELECT language_id
                       FROM   pelis_mismo_idioma
                       WHERE  num_pelis = (SELECT Max(num_pelis)
                                           FROM   pelis_mismo_idioma)); 

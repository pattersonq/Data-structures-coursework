ALTER TABLE public.staff DROP COLUMN store_id CASCADE;

CREATE TABLE staff_in_store(
	staff_id INTEGER NOT NULL,
	store_id INTEGER NOT NULL,
	fecha_entrada DATE NOT NULL,
	fecha_salida DATE NOT NULL,
	last_update timestamp without time zone DEFAULT now(),

	FOREIGN KEY (staff_id) REFERENCES staff(staff_id),
	FOREIGN KEY (store_id) REFERENCES store(store_id),
	PRIMARY KEY (staff_id, store_id));

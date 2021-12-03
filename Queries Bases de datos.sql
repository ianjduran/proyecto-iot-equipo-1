-- Creacion de Tablas	
CREATE TABLE Dueno (
	idDueno INT,
	NombreD VARCHAR(15),
	FechaNacimiento DATE,
	telefono CHAR(10),
	PRIMARY KEY(idDueno)
);

CREATE TABLE Mascota (
	idMascota INT,
    	idDueno INT,
	nombreMascota VARCHAR(15),
	raza VARCHAR(15),
	FechaNacimiento DATE,
	intervaloComida INT,
	PRIMARY KEY(idMascota),
	FOREIGN KEY(idDueno) REFERENCES Dueno(idDueno)
);

CREATE TABLE Intento (
	idIntento INT,
    	idMascota INT,
	tiempo TIMESTAMP,
	puedeAlimentar VARCHAR(15),
	PRIMARY KEY(idIntento),
	FOREIGN KEY(idMascota) REFERENCES Mascota(idMascota)
);

--Inserts de Dueños	
INSERT INTO Dueno VALUES (1, 'David Cantu', '1986-04-05', '8155667788');
INSERT INTO Dueno VALUES (2, 'Bob Esponja', '1990-01-04', '8122334499');

--Inserts de Mascota
INSERT INTO Mascota VALUES (100, 1,'Luna', 'Labrador', '2018-10-18', 6);
INSERT INTO Mascota VALUES (101, 2, 'Rambo', 'Labrador', '2018-10-19', 6);


-- IMPORTACION DE DATOS

-- Eliminacion de primera fila luego del export del csv con los datos
DELETE FROM datos
WHERE idMascota = “idMascota”;

-- Inserción de datos usando casteo
INSERT INTO intento
SELECT convert(idIntento, int), 
idMascota, convert(tiempo,datetime), 
convert(puedeAlimentar, int)
FROM datos;

-- CONSULTAS

-- Consultas de Información
-- Contabilizar las veces que se acercaron las mascotas pero no era su hora de comida.
SELECT nombreMascota as Perrito, count(puedeAlimentar) as 'Me acerqué a comer pero no era mi hora de comida'
FROM intento
JOIN Mascota on Mascota.idMascota = intento.idMascota
WHERE puedeAlimentar=0
GROUP BY nombreMascota;

-- Cuantas veces si comieron en su horario
SELECT nombreMascota as Perrito, count(puedeAlimentar) as 'Me acerqué a comer y sí era mi horario de comida'
from intento
join Mascota on Mascota.idMascota = intento.idMascota
where puedeAlimentar=1
group by nombreMascota;

-- Conteo acerca de cuántos intento cada mascota por día  incluyendo nombre de mes y dia de la semana
SELECT nombreMascota as Puppy,  YEAR(tiempo) as Year, 
	   MONTHNAME(tiempo) as Month, DAY(tiempo) as Day,
	   DAYNAME(tiempo) as DayName, COUNT(idIntento) as intentos 
FROM intento
JOIN mascota ON mascota.idMascota=intento.idMascota
GROUP BY DATE(tiempo), nombreMascota;

-- Día en que se intentó más veces tomando en cuenta la  fecha y el nombre del día.
SELECT DATE(tiempo) as Fecha, DAYNAME(tiempo) as Dia, COUNT(idIntento) as Intentos
FROM intento
GROUP BY DATE(tiempo)
HAVING COUNT(intentos)>=ALL
(
    SELECT COUNT(*)
	FROM intento
	GROUP BY DATE(tiempo)
);

-- Intentos por dia de la semana y mascota
SELECT nombreMascota as Puppy, DAYNAME(tiempo) as "Day of Week", COUNT(idIntento) as intentos 
FROM intento
JOIN mascota ON mascota.idMascota=intento.idMascota
GROUP BY nombreMascota, DAYNAME(tiempo);

--Intentos que se realizan cada día de la semana
SELECT DAYNAME(tiempo) as "Day of Week", COUNT(idIntento) as intentos 
FROM intento
GROUP BY DAYNAME(tiempo);

--Intentos por mes de cada mascota
SELECT nombreMascota as Puppy, MONTHNAME(tiempo) as Month, COUNT(idIntento) as intentos
FROM intento JOIN mascota ON mascota.idMascota=intento.idMascota
GROUP BY nombreMascota, MONTH(tiempo);

-- CREACION PROCEDIMIENTOS Y QUERIES PARA WEB API

-- Insertar registros en la tabla intentos
INSERT INTO intento(idMascota,tiempo,puedeAlimentar)
VALUES('9C E8 76 6E',CURRENT_TIMESTAMP(),1);

-- Creacion Procedimiento (similar a una funcion) para insertar datos
CREATE PROCEDURE SPIntento (IN NUID VARCHAR(11), estado INT)
BEGIN
    INSERT INTO intento (idMascota,tiempo,puedeAlimentar)
    VALUES(NUID, CURRENT_TIMESTAMP(), estado);
END;

-- Insertar dato llamando al procedimiento
-- Parametros -> (idMacota, puedeAlimentar)
CALL SPIntento('1A E8 C2 82', 0);

-- Solicitar el ultimo registro de la tabla Intentos
select * from intento WHERE tiempo = (select MAX(tiempo)from intento);

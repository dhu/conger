CREATE SCHEMA aggregate (time INT,currenttime INT,maxprice DOUBLE )
CREATE SCHEMA packet (time INT,price DOUBLE )
CREATE INPUT STREAM InputStream packet
CREATE OUTPUT STREAM OutputStream aggregate
CREATE WINDOW Dimension1(SIZE 180 ADVANCE 10 ON time)
SELECT max(time) AS currenttime,max(price) AS maxprice FROM InputStream[Dimension1] INTO OutputStream

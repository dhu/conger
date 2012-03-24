CREATE INPUT STREAM Packet PacketTuple
CREATE OUTPUT STREAM Aggregate AggregateTuple
CREATE SCHEMA PacketTuple (time INT,price DOUBLE )
CREATE SCHEMA AggregateTuple(time INT,maxprice DOUBLE ,currenttime INT)
CREATE WINDOW Dimension1(SIZE 180 ADVANCE 10 ON time)
SELECT max(price) AS maxprice, max(time) AS currenttime FROM Packet[Dimension1] INTO Aggregate

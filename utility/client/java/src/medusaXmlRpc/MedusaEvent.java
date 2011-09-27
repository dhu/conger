package medusaXmlRpc;

import java.util.*;

public class MedusaEvent extends Vector{

    public MedusaEvent(Vector columns, Vector medusaIDVector, Vector timeStampVector, String rowID){

	this.add(columns);
	this.add(medusaIDVector);
	this.add(timeStampVector);
	this.add(rowID);
    }

    /* Get the columns from the event
     */
    public Vector getColumns(){
	return (Vector) this.firstElement();
    }

    /* Returns the timestamp
     */
    public Long getTimestamp(){
	String timestamp = (String) ((Vector)this.get(2)).get(0);
	Long timestampLong = Long.valueOf(timestamp);
	return timestampLong;
    }

    /* Returns MedusaIDVector
     */
    public Vector getMedusaIDVector(){
	return (Vector) this.get(1);
    }
}

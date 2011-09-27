package medusaXmlRpc;

import java.util.*;

public class StreamEventVector extends Vector{

    private Vector streamEventSubVector;
    private Vector nameVector;

    public StreamEventVector(String host, String path, Vector events, Vector deletedEvents, Boolean streamClosed, Boolean injectEvent){

	streamEventSubVector = new Vector();
	nameVector = new Vector();

	nameVector.add(host);
	nameVector.add(path);

	streamEventSubVector.add(nameVector);
	streamEventSubVector.add(events);
	streamEventSubVector.add(deletedEvents);
	streamEventSubVector.add(streamClosed);
	streamEventSubVector.add(injectEvent);
	
	//need to encapsulate this info in yet another vector

	this.add(streamEventSubVector);

    }

    
    /* Makes a StreamEventVector out of a vector of the same exact format.  When medusa posts events using XML-RPC, it sends a vector.
     *  This will change it to a StreamEventVector and allow the use of handy methods provided to extract fields from the vector.
     * @param vec a Vector that is in the same exact format as a StreamEventVector.  Usually obtained from a medusa stream.  Will not work if Vector is not in the right format!
     */
    public StreamEventVector(Vector vec){
	
	String host;
	String path;
	Vector events = new Vector();
	Vector deletedEvents;
	Boolean streamClosed;
	Boolean injectEvent;

	Iterator vecIter = vec.iterator();

	///////first, extract the host and the path from the vector//////
	Vector vecStreamName = (Vector) vecIter.next();

	host = (String) vecStreamName.get(0);
	path = (String) vecStreamName.get(1);

	///////next, extract the events/////////
	Vector vecEvents = (Vector) vecIter.next();

	Iterator eventIter = vecEvents.iterator();
	
	//there may be multiple events, try to extract them all
	while (eventIter.hasNext()){
	    Vector vecEvent = (Vector) eventIter.next();
	    
	    Vector columns = (Vector) vecEvent.get(0);
	    Vector medusaIDVector = (Vector) vecEvent.get(1);
	    Vector timeStampVector = (Vector) vecEvent.get(2);
	    //System.out.println("got timeStampVector");
	    String rowID = (String) vecEvent.get(3);

	    MedusaEvent me = new MedusaEvent(columns, medusaIDVector, timeStampVector, rowID);
	    events.add(me);
	}

	// at this point, the events vector is populated

	///////next, take care of the other items//////

	///////deleted tuples//////
	deletedEvents = (Vector) vecIter.next();

	///////stream closed?/////
	streamClosed = (Boolean) vecIter.next();

	//////inject event?//////
	injectEvent = (Boolean) vecIter.next();

	// We now have all the elements to make a StreamEventVector

	streamEventSubVector = new Vector();
	nameVector = new Vector();

	nameVector.add(host);
	nameVector.add(path);

	streamEventSubVector.add(nameVector);
	streamEventSubVector.add(events);
	streamEventSubVector.add(deletedEvents);
	streamEventSubVector.add(streamClosed);
	streamEventSubVector.add(injectEvent);
	
	//need to encapsulate this info in yet another vector

	this.add(streamEventSubVector);
    }


    /* Returns the events in the stream event vector
     *
     */
    public Vector getEvents(){
	return (Vector) streamEventSubVector.get(1);
    }

    public Vector getNames() {
	return (Vector) streamEventSubVector.get(0);
    }
}

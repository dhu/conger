package medusaXmlRpc;

import java.util.*;
import org.apache.xmlrpc.*;

public class MedusaEventHandler {
    
    private Vector medusaEventListeners;

    public MedusaEventHandler () {
	medusaEventListeners = new Vector();
    }
    
    public Boolean handle_event (Vector vec) {

	
	//System.out.println("got event");
	//Decompose the StreamEventVector into its components and print on a line
     
	StreamEventVector sev = new StreamEventVector(vec);

	//debugging//
	//System.out.println("made streameventvector");
	/////////////

	Vector events = sev.getEvents();
	//debugging//
	//System.out.println("got events vector");
	/////////////

	Iterator eventIterator = events.iterator();

	//debugging//
	//if (! eventIterator.hasNext()){
	//    System.out.println("no events");
	//}
	/////////////


	while (eventIterator.hasNext()){
	//debugging//
	//System.out.println("inside eventsIterator");
	/////////////
	    StringBuffer output = new StringBuffer();

	    MedusaEvent event = (MedusaEvent) eventIterator.next();
	    Vector columns = event.getColumns();

	    Iterator columnIterator = columns.iterator();
	    
	    while (columnIterator.hasNext()){
		String elt = (String) columnIterator.next();
		output = output.append(elt + " ");
	    }

	    /*
	    //print the contents of the event to System.out
	    System.out.println(output);
	    */

	    //push the contents of the columns to all registered listeners
	    String outputString = output.toString(); //stringbuffer

	    fireEvents(outputString);
	}
	
       
	return new Boolean(true);
    }
    
    public void addMedusaEventListener(MedusaEventListener mel){
	medusaEventListeners.add(mel);
	System.out.println("MedusaEventHandler: added MedusaEventListener");
    }

    /* Inform all registered listeners about the event
     *
     *
     */
    private void fireEvents(String output){

	System.out.println("fireEvents: " + output);

	Iterator melIter = medusaEventListeners.iterator();
	while (melIter.hasNext()){
	    System.out.println("fireEvents: Inside melIterator");
	    MedusaEventListener mel = (MedusaEventListener) melIter.next();
	    mel.postEvent(output);
	}
    }

    public static void main (String [] args) {

	SubscriptionVector sub = MedusaXmlRpc.makeSubscriptionVector("nms.lcs.mit.edu", "rfidtrack/rfidtags_sighting_stream", "xml", "127.0.0.1:30006", "sample", new Integer(1));

	try {
	    
	    // Invoke me as <http://localhost:8080/RPC2>.
	    WebServer server = new WebServer(30006);
	    server.addHandler("sample", new MedusaEventHandler());
	    
	} catch (Exception exception) {
                System.err.println("MedusaEventHandler: " + exception.toString());
	}

	//try to subscribe
	try {
	    MedusaXmlRpc.subscribe("http://beacon.lcs.mit.edu:10001/RPC2", sub);
	}catch(Exception e){
	    System.err.println("error subscribing: " + e.toString());
	}
    }
}


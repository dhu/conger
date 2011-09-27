package medusaXmlRpc;

import java.util.*;

public class SubscriptionVector extends Vector{

    Vector nameVector;
    Vector subscriptionSubVector;
    Vector destinationVector;

    /* @param host the host [nms.lcs.mit.edu  from medusa://nms.lcs.mit.edu/foobar]
     * @param path the path [foobar   from medusa://nms.lcs.mit.edu/foobar]
     * @param transport [either TCP or XML]
     * @param destination the node to receive events.  ip:port [192.168.0.1:10000]
     * @param objectName the name of the object that the node sending the stream can call.  objectName.handleevent
     * @param addOrRemove indicate whether to be added or removed from subscription
     */
    public SubscriptionVector(String host, String path, String transport, String destination, String objectName, Integer addOrRemove){
	
	nameVector = new Vector();
	nameVector.add(host);
	nameVector.add(path);

	destinationVector = new Vector();
	destinationVector.add(destination);

	subscriptionSubVector = new Vector();
	subscriptionSubVector.add(nameVector);
	subscriptionSubVector.add(transport);
	subscriptionSubVector.add(destinationVector);
	subscriptionSubVector.add(objectName);
	
	this.add(subscriptionSubVector);
	this.add(addOrRemove);

    }

}

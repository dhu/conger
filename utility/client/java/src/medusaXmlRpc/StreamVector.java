package medusaXmlRpc;

import java.util.*;

public class StreamVector extends Vector{

    Vector streamSubVector;
    Vector streamNameVector;
    Vector streamNameSubVector;

    public StreamVector(String host, String streamName, Vector schemaInfoVector, Integer history, Vector partitionVector){

	streamSubVector = new Vector();
	streamNameVector = new Vector();
	streamNameSubVector = new Vector();

	streamNameSubVector.add(host);
	streamNameSubVector.add(streamName);
	streamNameVector.add(streamNameSubVector);

	streamSubVector.add(streamNameVector);
	streamSubVector.add(schemaInfoVector);
	streamSubVector.add(history);
	streamSubVector.add(partitionVector);

	this.add(streamSubVector);

    }


}


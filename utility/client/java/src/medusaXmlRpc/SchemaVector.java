package medusaXmlRpc;

import java.util.*;

public class SchemaVector extends Vector{
    
    private Vector schemaInfoVector;
    private Vector schemaNameVector;
    private Vector schemaNameSubVector;

    public SchemaVector(String host, String schemaName, Vector schemaFieldsVector){

	schemaInfoVector = new Vector();
	schemaNameVector = new Vector();
	schemaNameSubVector = new Vector();

	schemaNameSubVector.add(host);
	schemaNameSubVector.add(schemaName);

	schemaNameVector.add(schemaNameSubVector);

	schemaInfoVector.add(schemaNameVector);
	schemaInfoVector.add(schemaFieldsVector);

	this.add(schemaInfoVector);
    }

    public Vector getSchemaInfoVector(){
	return (Vector) schemaInfoVector.clone();

    }


}

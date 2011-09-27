/**
 * Constance.java
 *
 * Created on July 23, 2005, 1:50 PM
 *
 */

package borgui.generator;

/**
 * A class that record all the constants.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/10/2005
 * @author vivian
 */
public class GenConstants {
    
    //Distribution type (integer)
    public final static int DIST_TYPE_CONSTANT = 0;
    public final static int DIST_TYPE_UNIFORM = 1;
    public final static int DIST_TYPE_NORMAL = 2;
    public final static int DIST_TYPE_EXPONENTIAL = 3;
    public final static int DIST_TYPE_LINEAR = 4;
    public final static int DIST_TYPE_RANDOM_WALK = 5;
    public final static int DIST_TYPE_RANDOMDIR_WALK = 6;
    //public final static int DIST_TYPE_TIMESTAMP = 7;
    
    //The parameter number of each kind of distribution
    public final static int[] DIST_PARAM_NUM = {1,2,2,1,4,5,5,0};
    
    //The distribution name
    public final static String[] DIST_NAME = {"CONSTANT","UNIFORM","NORMAL","EXPONENTIAL","LINEAR","RANDOMWALK","RANDOMDIRECTIONWALK"};
    
    //The total number of all distribution types.
    public final static int TOTAL_DIST_NUM = 7;

    //The parameter name of each distribution
    public final static String[] DIST_CONSTANT_PARAM = {"Constant value"};
    public final static String[] DIST_UNIFORM_PARAM = {"Lower bound","Upper bound"};
    public final static String[] DIST_NORMAL_PARAM = {"Mean","Standard deviation"};
    public final static String[] DIST_EXPONENTIAL_PARAM = {"Mean"};
    public final static String[] DIST_LINEAR_PARAM = {"Initial value","Step","Lower bound","Upper bound"};
    public final static String[] DIST_RANDOM_WALK_PARAM = {"Initial value","Step","Probability","Lower bound","Upper bound"};
    public final static String[] DIST_RANDOMDIR_WALK_PARAM = {"Initial value","Step","Probability","Lower bound","Upper bound"};
    
    //The distribution parameter matrix. 
    public final static String[][] DIST_PARAM_MATRIX = {DIST_CONSTANT_PARAM,
            DIST_UNIFORM_PARAM,
            DIST_NORMAL_PARAM,
            DIST_EXPONENTIAL_PARAM,
            DIST_LINEAR_PARAM,
            DIST_RANDOM_WALK_PARAM,
            DIST_RANDOMDIR_WALK_PARAM
    };
    
    
    public final static String FIELD_NAME = "field";
    public final static String SCHEMA_NAME = "schema";
    public final static String INPUT_NAME = "input";
    
    
    //Xml File
    public final static String DIST_ELEM_NAME = "distribution";
    public final static String DIST_TYPE_ATTR = "type";
    public final static String PARAM_DIST_ELEM_NAME = "param_distribution";
    public final static String DIST_NAME_ATTR = "name";
    public final static String PARAM_ELEM_NAME = "param";
    public final static String PARAM_VALUE_ATTR = "value";    
    public final static String GEN_INFO_ELEM_NAME = "gen_info";
    public final static String GENINFO_STREAM_ID_ATTR = "stream_id";
    public final static String GENINFO_STREAM_NUM_ATTR = "stream_num";
    public final static String GENINFO_TUPLE_NUM_ATTR = "tuple_num";
    public final static String GENINFO_RATE_ATTR = "rate";
    
    //
    public final static int DEFAULT_PORT = 17000;
    public final static String DEFAULT_STREAM_NUM = "1";
    public final static String DEFAULT_TUPLE_NUM = "100";
    
    
    //Rate
    public final static int DEFAULT_RATE_SLIDER_SCALE = 100;
    
    //command file
    public final static String COMM_FILE_NAME = "/tmp/GenCommand.txt";
    public final static String STOP_COMM = "stop";
    public final static String RATE_COMM = "rate";
    public final static String TUPLE_COMM = "tupl";
    
    //tuple file
    public final static String TUPLE_FILE_NAME = "/tmp/GenSaveTuples";
     
    
   /** Creates a new instance of Constance */
    public GenConstants() {
    }
    
}

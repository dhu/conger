/**
 * Distribution.java
 *
 * Created on July 19, 2005, 11:31 AM
 *
 */

package borgui.generator;

/**
 * A class that represents the distribution information of a distribution's parameter.
 *
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/11/2005
 *
 */
public class ParamDistribution {
    
    protected int m_type;
    protected Double m_params[];
    
    /**
     * Creates a new instance of ParamDistribution
     */
    
    public ParamDistribution() {
        //default is constant type
        m_type = 0;
        m_params = new Double[1];
        m_params[0] = 0.0;
    }
    
    /**
     * Create a new instance of ParamDistribution that has the given type
     *
     * @param type the distribution type
     */
    public ParamDistribution(int type) {
        m_type = type;
        int paramsNum = GenConstants.DIST_PARAM_NUM[m_type];
        m_params = new Double[paramsNum];
        for(int i = 0; i<paramsNum; i++) {
            m_params[i] = 0.0;
        }
    }
    
    /**
     * Set the type of the ParamDistribution as the given type
     *
     * @param type the given type
     */
    public void setParamDistType(int type) {
        m_type = type;
        int paramsNum = GenConstants.DIST_PARAM_NUM[m_type];
        m_params = new Double[paramsNum];
        for(int i = 0; i<paramsNum; i++) {
            m_params[i] = 0.0;
        }
        
    }
    
    /**
     * Return the type of the ParamDistribution
     *
     */
    public int getParamDistType() {
        return m_type;
    }
    
    /**
     * Set the parameter of the ParamDistribution
     *
     * @param index the sequence number of the parameter
     * @param paramValue the value of the parameter
     */
    public void setParamDistParam(int index, Double paramValue){
        if(index >= 0 & index < GenConstants.DIST_PARAM_NUM[m_type]) {
            m_params[index] = paramValue;
        } else {
            System.out.println("Error from ParamDistribution: The index of parameter" + index + " is invalid.");
            m_params[index] = -1.0;
        }
    }
    
    /**
     * Return the value of the given parameter
     *
     * @param index the sequence number of the parameter
     */
    public Double getParamDistParamValue(int index) {
        if(index >= 0 & index < GenConstants.DIST_PARAM_NUM[m_type]) {
            return m_params[index];
        } else {
            System.out.println("Error from ParamDistribution: The index of parameter" + index + " is invalid.");
            return -1.0;
        }
    }
    
    /**
     * Return the minimum value of all the parameters in this ParamDistirbution
     */
    public double getMinParamValue() {
        double result = 0.0;
        switch(m_type) {
            case GenConstants.DIST_TYPE_CONSTANT:
                result = m_params[0];
                break;
            case GenConstants.DIST_TYPE_UNIFORM:
                result = m_params[0];
                break;
            case GenConstants.DIST_TYPE_LINEAR:
                result = m_params[2];
                break;
            case GenConstants.DIST_TYPE_RANDOM_WALK:
                result = m_params[3];
                break;
            case GenConstants.DIST_TYPE_RANDOMDIR_WALK:
                result = m_params[3];
                break;
        }
        return result;
    }
    
    /**
     * Return the maximum value of all the parameters in this ParamDistirbution
     */
    public double getMaxParamValue() {
        double result = 0.0;
        switch(m_type) {
            case GenConstants.DIST_TYPE_CONSTANT:
                result = m_params[0];
                break;
            case GenConstants.DIST_TYPE_UNIFORM:
                result = m_params[1];
                break;
            case GenConstants.DIST_TYPE_LINEAR:
                result = m_params[3];
                break;
            case GenConstants.DIST_TYPE_RANDOM_WALK:
                result = m_params[4];
                break;
            case GenConstants.DIST_TYPE_RANDOMDIR_WALK:
                result = m_params[4];
                break;
        }
        return result;
    }
}

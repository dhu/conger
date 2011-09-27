/**
 * Distribution.java
 *
 * Created on August 2, 2005, 11:17 AM
 *
 */

package borgui.generator;

import org.jdom.*;

/**
 * A class that represent the distribution information for a input stream or a field.
 * 
 * @author Wenjuan Xing (vivian@cs.brown.edu)
 * version 1.0 10/10/2005
 */
public class Distribution {
    
    protected int m_type;
    protected ParamDistribution m_paramsDist[];
    
    /** 
     * Creates a new instance of Distribution 
     */
    public Distribution() {
        
        //default is constant type
        m_type = GenConstants.DIST_TYPE_CONSTANT;
        int paramsDistNum = GenConstants.DIST_PARAM_NUM[GenConstants.DIST_TYPE_CONSTANT];
        m_paramsDist = new ParamDistribution[paramsDistNum];
        
        for(int i = 0; i<paramsDistNum; i++) {
            m_paramsDist[i] = new ParamDistribution(GenConstants.DIST_TYPE_CONSTANT);
        }
    }
    
    /**
     * Create a new instance of Distribution with the gvien type
     * @param type distribution type
     */
    public Distribution(int type) {
        
        //default is constant type
        m_type = type;
        int paramsDistNum = GenConstants.DIST_PARAM_NUM[m_type];
        m_paramsDist = new ParamDistribution[paramsDistNum];
        
        for(int i = 0; i<paramsDistNum; i++) {
            m_paramsDist[i] = new ParamDistribution(GenConstants.DIST_TYPE_CONSTANT);
        }
    }
    
    /**
     * Set the distibution type as the given type
     * @param type the given type
     */
    public void setDistType(int type) {
        m_type = type;
        int paramsDistNum = GenConstants.DIST_PARAM_NUM[m_type];
        m_paramsDist = new ParamDistribution[paramsDistNum];
        for(int i = 0; i<paramsDistNum; i++) {
            m_paramsDist[i] = new ParamDistribution(GenConstants.DIST_TYPE_CONSTANT);
        }
    }
    
    /**
     * Return the distribution type
     * @return the distribution type
     */
    public int getDistType() {
        return m_type;
    }
    
    /**
     * Set the ParamDistribution to the "index"th parameter.
     * 
     * @param index the index of the parameter
     * @param paramDist the ParamDistribution that set to the parameter
     */
    public void setParamDist(int index, ParamDistribution paramDist){
        if(index >= 0 & index < GenConstants.DIST_PARAM_NUM[m_type]) {
            m_paramsDist[index] = paramDist;
        } else {
            System.out.println("Error from Distribution: The index of parameter" + index + " is invalid.");
            m_paramsDist[index] = new ParamDistribution(GenConstants.DIST_TYPE_CONSTANT);
        }
    }
    
    /**
     * Return the ParamDistribution of the "index"th parameter.
     * 
     * @param index the index of the parameter
     */    
    public ParamDistribution getParamDist(int index) {
        if(index >= 0 & index < GenConstants.DIST_PARAM_NUM[m_type]) {
            return m_paramsDist[index];
        } else {
            System.out.println("Error from Distribution: The index of parameter" + index + " is invalid.");
            return new ParamDistribution(GenConstants.DIST_TYPE_CONSTANT);
        }
    }
    
    
}

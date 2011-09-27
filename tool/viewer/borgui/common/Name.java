package borgui.common;

/**
 * Representation of a borealis name. 
 */ 
public class Name {

      protected String m_name;

      /**
      * 
      *@param name String representation of the name
      */ 
      public Name(String name) {
	    m_name = name;
      }

      /**
      * 
      *@param name String representation of the name
      */ 
      public void setName(String name) {
	    m_name = name;
      }

      /**
      * 
      *@return String representation of name
      */ 
      public String toString() {
	    return m_name;
      }

      public int hashCode() {
	    return m_name.hashCode();
      }
      
}

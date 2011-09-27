package borgui.common;


////////////////////////////////////////////////////////////////////////////////
//
//  Utilities for debugging.
//
public class  DebugTrace
{
    public static  void  trace() { trace(""); }


    // Print a log message preceeded by the method containing the trace call.
    //
    public static  void  trace(String  message)
    {
        System.out.println(
            Thread.currentThread().getStackTrace()[3].getMethodName()
                + ":  " + message);
    }
}


////////////////////////////  end  DebugTrace.java  ////////////////////////////

package csl.DictSearch;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author uli
 */
public class DictSearch<T> {

    static {
        System.loadLibrary( "JNIDictSearch" );
    }
    
    public DictSearch( String configFile ) {
        native_init(configFile);
    }
    
    /**
     *
     */
    private class ResultReceiver {
        ResultReceiver( List<Interpretation> init_l ) {
            l = init_l;
        }
        void receive( String answerString ) {
            Interpretation i = new Interpretation( answerString );
            l.add( i );
            
        }
        
        private List<Interpretation> l;
    }
    
    public List<Interpretation> query( String q ) {
        List answerList = new ArrayList<Interpretation>();
        long nativeAnswerSet = native_query( q, new ResultReceiver( answerList ) );
        
        return answerList;
    }
    
    
    
    private native void native_init( String config );

    private native long native_query(String q, ResultReceiver answerList );
    
}

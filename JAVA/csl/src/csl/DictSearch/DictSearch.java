package csl.DictSearch;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author uli
 */
public class DictSearch<T> {

    public DictSearch( String configFile ) {
        native_init(configFile);
    }
    
    private class ResultReceiver {
        ResultReceiver( List<Interpretation> init_l ) {
            l = init_l;
        }
        void receive( String answerString ) {
            Interpretation i = new Interpretation( answerString );
            l.add(i);
            
        }
        
        private List<Interpretation> l;
    }
    
    public List<T> query( String q ) {
        List answerList = new ArrayList<T>();
        long nativeAnswerSet = native_query( q, new ResultReceiver( answerList ) );
        
        return new ArrayList<T>();
    }
    
    void fillAnswerList( List<T> l ) {
        
    }
    
    
    private native long native_init( String config );

    private native long native_query(String q, ResultReceiver answerList );
    
}

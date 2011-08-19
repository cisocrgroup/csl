
package csl.DictSearch;

/**
 *
 * @author uli
 */
public class Interpretation {
    public Interpretation( String s ) {
            
        nativeString = new String( s );
        
        int begin = 0;
        int end = nativeString.indexOf(':');
        word = nativeString.substring( begin, end );
        begin = end + 1;
        end = nativeString.indexOf('+');
        modernWord = nativeString.substring( begin, end );
               
    }    
    
    @Override
    public String toString() {
        return nativeString;
    }
    
    private String word;
    private String modernWord;
    private String nativeString;
}

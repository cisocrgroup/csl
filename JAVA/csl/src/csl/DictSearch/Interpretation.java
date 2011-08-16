
package csl.DictSearch;

/**
 *
 * @author uli
 */
class Interpretation {
    public Interpretation( String s ) {
           nativeString = s;
    }    
    
    @Override
    public String toString() {
        return nativeString;
    }
    
    private String word;
    private String modernWord;
    private String nativeString;
}

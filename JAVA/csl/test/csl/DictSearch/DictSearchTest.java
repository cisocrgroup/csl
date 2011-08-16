/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package csl.DictSearch;

import java.util.ArrayList;
import java.util.List;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author uli
 */
public class DictSearchTest {
    
    public DictSearchTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }
    
    @Before
    public void setUp() {
    }
    
    @After
    public void tearDown() {
    }

    /**
     * Test of query method, of class DictSearch.
     */
    @Test
    public void testQuery() {
        System.out.println("query");
        String q = "";
        DictSearch instance = new DictSearch("/mounts/Users/student/uli/implement/OCRC_trunk/dictionaries/ocrc.ini");
        List<Interpretation> expResult = new ArrayList<Interpretation>();
        List result = instance.query(q);
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
    }

}

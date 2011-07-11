package clueweb2trectext;

import java.io.* ;
import java.util.* ;
import java.util.Arrays ;
import java.util.zip.GZIPInputStream;

/**
 * holds a set of clueweb docnos
 * 
 * internally maps docno to unique id using Jimmy Lin's code
 * 
 * keeps the ids in a sorted array
 * 
 * uses binary search to quickly find ids
 * 
 * @author Mark D. Smucker
 */
public class ClueWebDocnoSet
{   
    private ClueWebDocnoMapping mapper ;
    private ArrayList<Integer> hamIds = new ArrayList<Integer>( 50000 ) ; 
    //private int [] hamIds = new int[150955774] ; // cat clueweb09spam.Fusion.ge70.gz | gunzip -c | wc -l
    private Boolean sorted = false ;
       
    public ClueWebDocnoSet(String gzipFileWithDocnos, ClueWebDocnoMapping mapper) 
            throws FileNotFoundException, Exception
    {
        this.mapper = mapper ;
        LoadDocnos( gzipFileWithDocnos ) ;
    }
    
    public void AddDocno( String docno ) throws Exception
    {
        int id = mapper.ConvertToInt( docno );
        hamIds.add(id) ;
        sorted = false ;
    }
    
    public Boolean DocnoExists( String docno ) throws Exception
    {
        if ( ! sorted ) 
        {
             Collections.sort( hamIds );
             sorted = true ;
        }
        int id = mapper.ConvertToInt( docno ) ;
        int index = Collections.binarySearch( hamIds, id ) ;
        //int index = Arrays.binarySearch( hamIds, id ) ;
        return index >= 0 ;       
    }


    private void LoadDocnos( String gzipFileWithDocnos ) throws IOException, Exception
    {
        GZIPInputStream gzInputStream = new GZIPInputStream(
                    new FileInputStream( gzipFileWithDocnos ) );
        BufferedReader inReader = new BufferedReader(
                        new InputStreamReader(gzInputStream));

        String line = null ;
        while ((line = inReader.readLine()) != null) 
        {
            String docno = line ;
            AddDocno( docno );
            //if ( count % 1000000 == 0 )
            //{
            //    System.err.println( count );
           // }
            
//            if ( hamIds.size() % 1000000 == 0 )
//            {
//                System.err.println( hamIds.size() );
//            }
        }
        inReader.close();
    }
    
    
}

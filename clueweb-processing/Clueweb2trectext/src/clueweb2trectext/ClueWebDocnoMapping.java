package clueweb2trectext;

import java.io.* ;
import java.util.* ;

/**
 * Based on Jimmy Lin's ClueWarcDocnoMapping class 
 * from his Cloud9 software.
 * 
 * 
 * @author Mark D. Smucker
 */
public class ClueWebDocnoMapping
{
    private Map<String, Integer> dirFile2offset = new HashMap<String, Integer>() ;

    public ClueWebDocnoMapping( String docnoMapFile ) throws FileNotFoundException, Exception
    {
        LoadMap( docnoMapFile ) ;
    }
       
    private void LoadMap( String docnoMapFile ) throws FileNotFoundException, Exception
    {
        Scanner input = new Scanner( new File( docnoMapFile ) ) ;
        while ( input.hasNext() )
        {
            String dir = input.next() ;
            if ( ! input.hasNextInt() )
            {
                throw new Exception( "malformed docnoMapFile") ;
            }
            int offset = input.nextInt() ;

            dirFile2offset.put( dir, offset ) ;
        }
        input.close();
    }
 
    public int ConvertToInt( String docno ) throws Exception
    {
        // 012345678901234567890123456789
        // clueweb09-en0113-48-27622
        String dirFile = docno.substring( 10, 19 );
        if ( dirFile2offset.containsKey( dirFile ) )
        {
            int offset = dirFile2offset.get( dirFile ) ;
            int docNumberInFile = Integer.parseInt( docno.substring(20, 25), 10 ) ;
            return offset + docNumberInFile ;
        }
        else
        {
            throw new Exception( "bad docno, not in map");          
        }
    }

    // implement later by storing map to allow searching the offsets
    //public String ConvertToDocno( int id )
    //{      
    //
    //}
    
}

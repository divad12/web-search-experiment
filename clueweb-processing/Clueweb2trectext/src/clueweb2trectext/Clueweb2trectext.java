package clueweb2trectext;

import java.io.* ;
import java.util.zip.*;
import net.htmlparser.jericho.*;
import edu.cmu.lemurproject.* ;

// sentence breaking via sentence splitter
import org.wikipedia.miner.util.SentenceSplitter ;
import java.util.Vector ;

// for the sentence breaking via lingpipe (not used)
//import com.aliasi.chunk.Chunk;
//import com.aliasi.chunk.Chunking;
//
//import com.aliasi.sentences.IndoEuropeanSentenceModel ;
//import com.aliasi.sentences.SentenceChunker;
//import com.aliasi.sentences.SentenceModel;
//
//import com.aliasi.tokenizer.IndoEuropeanTokenizerFactory;
//import com.aliasi.tokenizer.TokenizerFactory;
//
//import com.aliasi.util.Files;
//import java.util.Iterator ;
//import java.util.Set ;

import java.io.File;
import java.io.IOException;

/**
 *
 * @author Mark D. Smucker
 */
public class Clueweb2trectext
{
    static final org.wikipedia.miner.util.SentenceSplitter sentenceSpliter = 
            new SentenceSplitter() ;
    //static final TokenizerFactory TOKENIZER_FACTORY = IndoEuropeanTokenizerFactory.INSTANCE;
    //static final SentenceModel SENTENCE_MODEL  = new IndoEuropeanSentenceModel( false, false ) ;
    //static final SentenceChunker SENTENCE_CHUNKER 
    //	= new SentenceChunker(TOKENIZER_FACTORY,SENTENCE_MODEL);
   
    public static void main( String[] args ) throws IOException, FileNotFoundException, Exception
    {
        // Okay, the easiest way to do this is to pass in an input directory and output dir
        if ( args.length != 4 )
        {
            System.err.println( "usage: java Clueweb2trectext inputDir outputDir fusionSpam.ge70.gz docno.mapping.simple");
            return ;
        }
        String inputDirName = args[0] ;
        String outputDirName = args[1] ;
        String hamDocnosFileName = args[2] ;
        String mappingFileName = args[3] ;
        File inputDir = new File( inputDirName ) ;
        if ( ! inputDir.exists() )
        {
            System.err.println( "inputDir does not exist: " + inputDirName );
            return ;
        }
        File outputDir = new File( outputDirName ) ;
        if ( ! outputDir.exists() )
        {
            System.err.println( "outputDir does not exist: " + outputDirName );
            return ;
        }
               
        ClueWebDocnoMapping docnoMapper = new ClueWebDocnoMapping( mappingFileName ) ;
        ClueWebDocnoSet hamDocnos = new ClueWebDocnoSet( hamDocnosFileName, docnoMapper ) ;
        
        // for each warc.gz file in inputDir, convert it and output new file to outputDir
        
        String [] fileNames = inputDir.list() ;
        for ( int fileNameIdx = 0 ; fileNameIdx < fileNames.length ; ++fileNameIdx )
        {
            String currFileName = fileNames[fileNameIdx] ; 
            if ( currFileName.endsWith( "warc.gz" ) )
            {
                ConvertWarcToTrec( inputDir, currFileName, outputDir, hamDocnos ) ;
            }
        }
    }

    public static void ConvertWarcToTrec( File inputDir, String gzipWarcFileName, 
            File outputDir, ClueWebDocnoSet hamDocnos ) throws IOException, Exception
    {               
        int extIdx = gzipWarcFileName.indexOf( ".warc.gz" ) ;
        int dotIdx = gzipWarcFileName.indexOf( "." ) ;
        if ( extIdx != dotIdx )
        {
            throw new Exception("bad file name:" + gzipWarcFileName ) ;
        }
        String baseName = gzipWarcFileName.substring( 0, extIdx) ;
        String gzipTrectextFileName = baseName + ".trectext.gz" ;
        PrintWriter trecFile = new PrintWriter( 
                new BufferedOutputStream(
                new GZIPOutputStream(
                new FileOutputStream( outputDir.getAbsolutePath() + File.separator + gzipTrectextFileName ) ) ) ) ;
        
        String inputWarcFile = inputDir.getAbsolutePath() + File.separator + gzipWarcFileName ;

        // open our gzip input stream
        GZIPInputStream gzInputStream = new GZIPInputStream(
                new FileInputStream( inputWarcFile ) );

        /// cast to a data input stream
        DataInputStream inStream = new DataInputStream( gzInputStream );

        // iterate through our stream
        WarcRecord thisWarcRecord;
        while ( ( thisWarcRecord = WarcRecord.readNextWarcRecord( inStream ) ) != null )
        {
            // see if it's a response record
            if ( thisWarcRecord.getHeaderRecordType().equals( "response" ) )
            {
                // it is - create a WarcHTML record
                WarcHTMLResponseRecord htmlRecord = new WarcHTMLResponseRecord(
                        thisWarcRecord );
                String docno = htmlRecord.getTargetTrecID();
                if ( ! hamDocnos.DocnoExists( docno ) )
                {
                    continue ;
                }

                byte[] contentBytes = thisWarcRecord.getContent();
                StringBuilder doc = new StringBuilder(contentBytes.length);                 
                doc.append( "<DOC>\n<DOCNO>" ) ;
                doc.append( docno ) ;
                doc.append( "</DOCNO>\n<TEXT>\n") ;
                
                ByteArrayInputStream contentStream = new ByteArrayInputStream(
                        contentBytes );
                BufferedReader inReader = new BufferedReader(
                        new InputStreamReader( contentStream ) );

                String url = htmlRecord.getTargetURI();
                
                // forward to the first /n/n

                boolean firstLine = true;
                boolean inHeader = true;
                String line = null;
                while ( inHeader
                        && ( ( line = inReader.readLine() ) != null ) )
                {
                    if ( !firstLine && line.trim().length() == 0 )
                    {
                        inHeader = false;
                    }
                    else
                    {
                        //outHeader.println( line );
                    }

                    if ( firstLine )
                    {
                        firstLine = false;
                    }
                }

                // now we have the rest of the lines
                // read them all into a string buffer
                // to remove all new lines
                StringBuilder html = new StringBuilder( 65536 );
                while ( ( line = inReader.readLine() ) != null )
                {
                    html.append( line );
                }
                Source source = new Source( html );
                source.setLogger( null ); // turn off logging
                source.fullSequentialParse() ; // error message recommends calling this after creation
                String title = getTitle( source ) ;
                Renderer renderer = source.getRenderer();
                renderer.setIncludeHyperlinkURLs( false );               
                String renderedText = renderer.toString();
                //TextExtractor textExtractor = source.getTextExtractor() ;
                //String renderedText = textExtractor.toString() ;

                // identify and mark up sentences
                String sentenceText = renderedText ;
                Vector<String> sentences = sentenceSpliter.getSentences( renderedText, 
                        org.wikipedia.miner.util.SentenceSplitter.MULTIPLE_NEWLINES ) ;
                StringBuilder sbSentenceText = new StringBuilder( (int)(renderedText.length()*1.20) ) ;
                for ( int sIdx = 0 ; sIdx < sentences.size() ; ++sIdx )
                {
                    if ( sIdx > 0 )
                    {
                        sbSentenceText.append( "\n<sentence>") ;
                    }
                    else
                    {
                        sbSentenceText.append( "<sentence>") ;
                    }
                    String sentToAppend = sentences.elementAt( sIdx ) ;
                    sbSentenceText.append( sentToAppend ) ;                            
                    sbSentenceText.append( "</sentence>") ;                   
                }
                sentenceText = sbSentenceText.toString() ;                
//                Chunking chunking 
//                    = SENTENCE_CHUNKER.chunk(renderedText.toCharArray(),0,renderedText.length());
//                Set<Chunk> sentences = chunking.chunkSet();
//                int minSentenceLength = 80 ; // a line of google snippet is around 90 chars
//                if (sentences.size() >= 1) 
//                {
//                    StringBuilder sbSentenceText = new StringBuilder( (int)(renderedText.length()*1.20) ) ;
//                    String slice = chunking.charSequence().toString();
//                    int lengthAdded = 0 ;
//                    for (Iterator<Chunk> it = sentences.iterator(); it.hasNext(); ) 
//                    {
//                        Chunk sentence = it.next();
//                        int start = sentence.start();
//                        int end = sentence.end();
//                        String sentToAppend = slice.substring(start,end);
//                        if ( lengthAdded == 0 )
//                        {
//                            sbSentenceText.append( "<sentence>") ;                            
//                        }
//                        else
//                        {
//                            // make sure there is a space between appended
//                            // sentences.
//                            sbSentenceText.append( " ") ; 
//                        }
//                        sbSentenceText.append( sentToAppend ) ;
//                        lengthAdded += sentToAppend.length() ;
//                        if ( lengthAdded >= minSentenceLength )
//                        {
//                            sbSentenceText.append( "</sentence>" ) ;
//                            lengthAdded = 0 ;
//                        }
//                    }
//                    if ( lengthAdded > 0)
//                    {
//                            sbSentenceText.append( "</sentence>" ) ;
//                            lengthAdded = 0 ;                        
//                    }                
//                    sentenceText = sbSentenceText.toString() ;
//                }    
                
                //String tokenizedurl = TokenizeUrl( url ) ;
                
                doc.append( "<title>" ) ;
                doc.append( title ) ;
                doc.append( "</title>\n<url>" ) ;
                doc.append(  url ) ;
                doc.append( "</url>\n<body>\n" ) ;
                doc.append( sentenceText ) ;
                doc.append( "\n</body>\n</text>\n</DOC>\n" ) ;
                
                trecFile.print( doc.toString() ) ;                                      
            }
         }
        trecFile.flush();
        trecFile.close();
    }

    private static String getTitle( Source source )
    {
        Element titleElement = source.getFirstElement( HTMLElementName.TITLE );

        if ( titleElement == null )
        {
            return "" ;
        }
        return titleElement.getContent().toString() ;
    }

    private static String TokenizeUrl( String url )
    {
        return url ;
    }

}

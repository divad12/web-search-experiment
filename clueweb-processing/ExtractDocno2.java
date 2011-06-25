import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.zip.GZIPInputStream;

import net.htmlparser.jericho.Element;
import net.htmlparser.jericho.Renderer;
import net.htmlparser.jericho.Source;

import org.util.GetAncherText;
import org.util.GetTokenizeURL;

public class ExtractDocno2 {

	
	public static void main(String[] args) throws IOException {
		long time=System.currentTimeMillis();
		String[] keys = { "en0000", "en0001","en0040"};
		
		String[] directories = {
				"/home/aiman/WRP/WarcProject/src/WarcFiles/clueweb1/en0000",
				"/home/aiman/WRP/WarcProject/src/WarcFiles/clueweb1/en0001",
				"/home/aiman/WRP/WarcProject/src/WarcFiles/clueweb1/en0002",
				"/home/aiman/WRP/WarcProject/src/WarcFiles/clueweb2/en0000",
				"/home/aiman/WRP/WarcProject/src/WarcFiles/clueweb2/en0001",
				"/home/aiman/WRP/WarcProject/src/WarcFiles/clueweb2/en0002"};
	
		
		Class c=ExtractDocno2.class;
		
		BufferedReader docnoReader=new BufferedReader(new InputStreamReader(c.getResourceAsStream("clueweb09spam.Fusion.ge70.txt")));
		String docno="";
		 PrintWriter titleFile = new PrintWriter( new BufferedWriter( new FileWriter("/home/aiman/WRP/WarcProject/output/"+"title"+ ".txt" ) ) ) ;	
		while((docno=docnoReader.readLine())!=null){
		// = "clueweb09-en0040-54-00397";// args[0];
		StringBuilder title=new StringBuilder("");
		StringBuilder plaiText=new StringBuilder("");
		
		
		PrintWriter outHeader = new PrintWriter( new BufferedWriter( new FileWriter("/home/aiman/WRP/WarcProject/output/"+docno + ".header" ) ) );
	    PrintWriter outHtml = new PrintWriter( new BufferedWriter( new FileWriter("/home/aiman/WRP/WarcProject/output/"+ docno + ".html" ) ) ) ;
	    PrintWriter outText = new PrintWriter( new BufferedWriter( new FileWriter("/home/aiman/WRP/WarcProject/output/"+ docno + ".txt" ) ) ) ;
	//New code
	    PrintWriter outUrl = new PrintWriter( new BufferedWriter( new FileWriter("/home/aiman/WRP/WarcProject/output/"+ docno + ".url" ) ) ) ;
	   
	    
			// 012345678901234567890123456789
			// clueweb09-en0113-48-27622
			

			String subdir = docno.substring(10,16) ;
		    String zipFile = docno.substring(17,19) + ".warc.gz" ;
		     
		    int i = 0 ;
		    for ( ; i < keys.length ; ++i )
		    {
		        if ( keys[i].compareTo(subdir) == 0 )
		        break ;
		    }
		    String inputWarcFile = directories[i] + "/" + zipFile ;
		  
		    //System.out.println( inputWarcFile ) ;
		  
		    // open our gzip input stream
			GZIPInputStream gzInputStream = new GZIPInputStream(
					new FileInputStream(inputWarcFile));

			/// cast to a data input stream
			DataInputStream inStream = new DataInputStream(gzInputStream);

			// iterate through our stream
			WarcRecord thisWarcRecord;
			while ((thisWarcRecord = WarcRecord.readNextWarcRecord(inStream)) != null) {
				
				// see if it's a response record
				
				
				if (thisWarcRecord.getHeaderRecordType().equals("response")) {
					// it is - create a WarcHTML record
					WarcHTMLResponseRecord htmlRecord = new WarcHTMLResponseRecord(
							thisWarcRecord);
					// get our TREC ID and target URI
					String thisTRECID = htmlRecord.getTargetTrecID();
					//System.out.println("thisTRECID :" +thisTRECID);
					
					if (docno.compareTo(thisTRECID) == 0) {
						byte[] contentBytes = thisWarcRecord.getContent();

						ByteArrayInputStream contentStream = new ByteArrayInputStream(
								contentBytes);
						BufferedReader inReader = new BufferedReader(
								new InputStreamReader(contentStream));

						String thisTargetURI = htmlRecord.getTargetURI();
						outUrl.print(thisTargetURI);
						outUrl.close();
						
						// forward to the first /n/n

						boolean firstLine = true;
						boolean inHeader = true;
						String line = null;
						while (inHeader
								&& ((line = inReader.readLine()) != null)) {
							if (!firstLine && line.trim().length() == 0) {
								inHeader = false;
							} else {
								outHeader.println(line);
							}

							if (firstLine) {
								firstLine = false;
							}
						}
						outHeader.close();

						// now we have the rest of the lines
						// read them all into a string buffer
						// to remove all new lines
						StringBuilder html = new StringBuilder(65536);
						
						while ((line = inReader.readLine()) != null) {
							outHtml.println(line);
							html.append(line);
						}
						outHtml.close();
						Source source = new Source(html);
						Element titleElement = source.getFirstElement("title");
						String tilteContent="";
						if(titleElement!=null)
						 tilteContent = titleElement.getContent().toString();
						else
							tilteContent="\n";
						
						title.append(docno+"\t"+tilteContent+"\n");
						Renderer renderer = source.getRenderer();
						renderer.setIncludeHyperlinkURLs(false);
						String renderedText = renderer.toString();
						plaiText.append(renderedText);
						System.out.println("Tokenize url is");
						System.out.println(GetTokenizeURL.getTokenizeUrl(htmlRecord.getTargetURI()));
						System.out.println("anchor text of docno :-" +docno);
						System.out.println(GetAncherText.getAncherText(source));
						
						break;
					}

				}
			}

			inStream.close();
			outText.print(plaiText);
			outText.close();
			titleFile.print(title.toString());
		}
		
		titleFile.close();
		System.out.println("total time"+(System.currentTimeMillis()-time));
	}
	
}


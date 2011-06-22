import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.zip.GZIPInputStream;

import net.htmlparser.jericho.Element;
import net.htmlparser.jericho.Renderer;
import net.htmlparser.jericho.Source;

public class ExtractDocno2 {

	
	public static void main(String[] args) throws IOException {

		String[] keys = { 
				"en0000", 
				"en0002", 
				"en0004",
				"en0006",
				"en0008",
				"en0010",
				"enwp00",
				"enwp02",
				"en0001",
				"en0003",
				"en0005",
				"en0007",
				"en0009",
				"en0011",
				"enwp01",
				"enwp03",
				"en0012",
				"en0014",
				"en0016",
				"en0018",
				"en0020",
				"en0022",
				"en0024",
				"en0026",
				"en0013",
				"en0015",
				"en0017",
				"en0019",
				"en0021",
				"en0023", 
				"en0025",
				"en0027",
				"en0029",
				"en0031",
				"en0033",
				"en0035",
				"en0037",
				"en0039",
				"en0028",
				"en0030",
				"en0032",
				"en0034",
				"en0036",
				"en0038",
				"en0040",
				"en0041",
				"en0043",
				"en0045",
				"en0047",
				"en0049",
				"en0051",
				"en0053",
				"en0042",
				"en0044",
				"en0046",
				"en0048",
				"en0050",
				"en0052",
				"en0054",
				"en0055",
				"en0057",
				"en0059",
				"en0061",
				"en0063",
				"en0065",
				"en0067",
				"en0056",
				"en0058",
				"en0060",
				"en0062",
				"en0064",
				"en0066",
				"en0068",
				"en0069",
				"en0071",
				"en0073",
				"en0075",
				"en0077",
				"en0079",
				"en0081",
				"en0083",
				"en0070",
				"en0072",
				"en0074",
				"en0076",
				"en0078",
				"en0080",
				"en0082",
				"en0084",
				"en0083",
				"en0085",
				"en0087",
				"en0089",
				"en0091",
				"en0093",
				"en0095",
				"en0097",
				"en0084",
				"en0086",
				"en0088",
				"en0090",
				"en0092",
				"en0094",
				"en0096",
				"en0097",
				"en0099",
				"en0101",
				"en0103",
				"en0105",
				"en0107",
				"en0109",
				"en0098",
				"en0100",
				"en0102",
				"en0104",
				"en0106",
				"en0108",
				"en0110",
				"en0112",
				"en0114",
				"en0116",
				"en0118",
				"en0120",
				"en0122",
				"en0111",
				"en0113",
				"en0115",
				"en0117",
				"en0119",
				"en0121",
				"en0123",
				"en0124",
				"en0125",
				"en0126",
				"en0127",
				"en0128",
				"en0129",
				"en0130",
				"en0131",
				"en0132",
				"en0133"};
		
		String[] directories = {
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0000", 
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0002", 
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0004",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0006",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0008",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0010",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/enwp00",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/enwp02",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0001",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0003",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0005",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0007",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0009",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/en0011",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/enwp01",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_1/enwp03",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0012",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0014",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0016",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0018",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0020",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0022",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0024",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0026",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0013",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0015",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0017",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0019",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0021",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0023", 
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_2/en0025",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0027",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0029",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0031",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0033",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0035",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0037",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0039",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0028",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0030",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0032",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0034",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0036",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0038",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_3/en0040",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0041",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0043",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0045",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0047",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0049",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0051",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0053",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0042",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0044",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0046",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0048",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0050",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0052",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_4/en0054",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0055",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0057",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0059",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0061",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0063",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0065",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0067",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0056",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0058",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0060",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0062",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0064",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0066",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_5/en0068",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0069",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0071",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0073",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0075",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0077",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0079",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0081",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0083",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0070",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0072",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0074",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0076",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0078",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0080",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0082",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_6/en0084",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0083",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0085",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0087",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0089",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0091",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0093",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0095",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0097",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0084",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0086",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0088",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0090",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0092",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0094",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_7/en0096",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0097",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0099",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0101",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0103",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0105",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0107",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0109",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0098",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0100",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0102",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0104",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0106",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_8/en0108",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0110",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0112",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0114",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0116",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0118",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0120",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0122",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0111",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0113",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0115",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0117",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0119",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0121",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_9/en0123",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0124",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0125",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0126",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0127",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0128",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0129",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0130",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0131",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0132",
				"/ClueWeb09/ClueWeb09_1/ClueWeb09_English_10/en0133"};
	
		
		Class c=ExtractDocno2.class;
		
		BufferedReader docnoReader=new BufferedReader(new InputStreamReader(c.getResourceAsStream("clueweb09spam.Fusion")));
		String docno="";
		 PrintWriter titleFile = new PrintWriter( new BufferedWriter( new FileWriter("/home/a2alharb/output/"+"title"+ ".txt" ) ) ) ;	
		while((docno=docnoReader.readLine())!=null){
		// = "clueweb09-en0040-54-00397";// args[0];
		StringBuilder title=new StringBuilder("");
		StringBuilder plaiText=new StringBuilder("");
		StringBuilder headerText=new StringBuilder("");
		
		PrintWriter outHeader = new PrintWriter( new BufferedWriter( new FileWriter("/home/a2alharb/output/"+docno + ".header" ) ) );
	    PrintWriter outHtml = new PrintWriter( new BufferedWriter( new FileWriter("/home/a2alharb/output/"+ docno + ".html" ) ) ) ;
	    PrintWriter outText = new PrintWriter( new BufferedWriter( new FileWriter("/home/a2alharb/output/"+ docno + ".txt" ) ) ) ;
	//New code
	    PrintWriter outUrl = new PrintWriter( new BufferedWriter( new FileWriter("/home/a2alharb/output/"+ docno + ".url" ) ) ) ;
	   
	    
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
		  
		    System.out.println( inputWarcFile ) ;
		  
		    // open our gzip input stream
			GZIPInputStream gzInputStream = new GZIPInputStream(
					new FileInputStream(inputWarcFile));

			/// cast to a data input stream
			DataInputStream inStream = new DataInputStream(gzInputStream);

			// iterate through our stream
			WarcRecord thisWarcRecord;
			while ((thisWarcRecord = WarcRecord.readNextWarcRecord(inStream)) != null) {
				
				// see if it's a response record
				;
				
				if (thisWarcRecord.getHeaderRecordType().equals("response")) {
					// it is - create a WarcHTML record
					WarcHTMLResponseRecord htmlRecord = new WarcHTMLResponseRecord(
							thisWarcRecord);
					// get our TREC ID and target URI
					String thisTRECID = htmlRecord.getTargetTrecID();
					System.out.println("thisTRECID :" +thisTRECID);
					
					if (docno.compareTo(thisTRECID) == 0) {
						byte[] contentBytes = thisWarcRecord.getContent();

						ByteArrayInputStream contentStream = new ByteArrayInputStream(
								contentBytes);
						BufferedReader inReader = new BufferedReader(
								new InputStreamReader(contentStream));

						// TODO: write out url to file alsoq
						String thisTargetURI = htmlRecord.getTargetURI();
						//System.out.println("thisTargetURI::"+thisTargetURI);
						// print our data
						// System.out.println(thisTRECID + " : " +
						// thisTargetURI);
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
									
					}

				}
			}

			inStream.close();
			outText.print(plaiText);
			outText.close();
			titleFile.print(title.toString());
		}
		
		titleFile.close();
	}
		
	}

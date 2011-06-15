//package edu.cmu.lemurproject;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.zip.GZIPInputStream;

public class ExtractWarcFile {
	String title = null;
	  BufferedWriter titlebufferedWriter = null;
	  BufferedWriter contentBufferedWriter = null;
	  DataInputStream inStream = null;
	public ExtractWarcFile(String inputWarcFile, String outputTitlesFile, String textFilesDir) {
		
		try{
			//titlebufferedWriter = new BufferedWriter(new FileWriter(outputTitlesFile));
			
		}catch(Exception e){
			e.printStackTrace();
		}
	}

  public  void readWarcFile(String inputWarcDir,String warcFileName, String textFilesDir, BufferedWriter titlebufferedWriter) throws IOException {
	
    // open our gzip input stream
    GZIPInputStream gzInputStream=new GZIPInputStream(new FileInputStream(inputWarcDir +"/"+warcFileName));
    
    // cast to a data input stream
    inStream=new DataInputStream(gzInputStream);
   try { 
    // iterate through our stream
    WarcRecord thisWarcRecord;
    while ((thisWarcRecord=WarcRecord.readNextWarcRecord(inStream))!=null) {
      // see if it's a response record
      if (thisWarcRecord.getHeaderRecordType().equals("response")) {
        // it is - create a WarcHTML record
        WarcHTMLResponseRecord htmlRecord=new WarcHTMLResponseRecord(thisWarcRecord);
        // get our TREC ID and target URI
        String thisTRECID=htmlRecord.getTargetTrecID();
        System.out.println("recordId::"+thisTRECID);
        String thisTargetURI=htmlRecord.getTargetURI();
        // get the content of the file, HTML page
        WarcRecord raw = htmlRecord.getRawRecord();
        String content = raw.getContentUTF8();
        // print our data
        //System.out.println(thisTRECID + " : " + thisTargetURI);
//        System.out.println(raw.toString());  
 
        
       // System.out.println(content);
      if(thisTargetURI.endsWith(".html") || thisTargetURI.endsWith(".jsp") || thisTargetURI.endsWith(".php") || thisTargetURI.endsWith(".asp")) {
    	  HtmlExtraction htmlExtraction = new HtmlExtraction();
    	  title =   htmlExtraction.titleExtraction(content);
    	  title = title.replaceAll("[^a-zA-Z0-9]+"," ");
    	 content = htmlExtraction.htmlContentExtraction(content);
    	  
    	  String returnTitle = writeContentToAFile(title, content, textFilesDir+"/"+warcFileName.split("\\.")[0]);

    	  writeTitlesToAfile(returnTitle.split("\\.")[0], titlebufferedWriter, warcFileName);
    	      	  
      }
      
      }
    }
    
   }
   catch(Exception e){
	   e.printStackTrace();
	 
	   
   }
  
 }  
  
  
  public void writeTitlesToAfile(String title, BufferedWriter titlebufferedWriter, String warcFileName) {
      
      try {
          //Start writing title to the output stream
    	  titlebufferedWriter.write(title);
    	  titlebufferedWriter.newLine();
    	  titlebufferedWriter.flush();
                   
      } catch (FileNotFoundException ex) {
          ex.printStackTrace();
      } catch (IOException ex) {
          ex.printStackTrace();
      } 
  }
  
  public String writeContentToAFile(String fileName, String content, String textFilesDir) {
	  long timeInMillis = 0;
      try {
    	  
    	 
          //remving the special characters from the title for creating the file with same name
    	  fileName = fileName.replaceAll("[^a-zA-Z0-9]+"," ");
    	  //If the file already exists appending the time in millis
    	  if(fileName == null || fileName.equals("") || fileName.equals(" ")) 
    		  fileName = "Untitled";
    	  String textFile = textFilesDir+"/"+fileName+".txt";
    	  File file = new File(textFile);
    	  if(file.exists()) {
    		  timeInMillis =  System.currentTimeMillis();
    		  textFile = textFilesDir+"/"+fileName+"+"+timeInMillis +".txt";
    		  fileName = fileName + timeInMillis;
    	  }
    	  contentBufferedWriter = new BufferedWriter(new FileWriter(textFile));
    	  contentBufferedWriter.write(content);
    	  contentBufferedWriter.flush();
    	  return fileName;
                   
      } catch (FileNotFoundException ex) {
          ex.printStackTrace();
          return null;
          
          
      } catch (IOException ex) {
          ex.printStackTrace();
          return null;
      } 
	  
  }
  
  public static void main(String[] args) throws IOException {
	 
  
	  String textFilesDir = System.getProperty("user.dir")+"/TextFilesDir";
	  String outputTitlesFile = System.getProperty("user.dir")+"/Titles.txt";
	  String inputWarcFilesDir = System.getProperty("user.dir")+"/WarcFiles";
	  
	  //creating the directory if the directory doesn't exist for all text files representing .html/php/asp
	  File file = new File(textFilesDir);
	  if(file.exists()) {
		  boolean result = deleteDirectory(file);
	  }
	    file.mkdirs();
	  
	    
	  ExtractWarcFile extractWarcFile = new ExtractWarcFile(inputWarcFilesDir,outputTitlesFile, textFilesDir); 
	  extractWarcFile.inputWarcFiles(inputWarcFilesDir, textFilesDir);
      //extractWarcFile.readWarcFile();
	}
  
  
  static public boolean deleteDirectory(File path) {
	    if(path.exists() ) {
	      File[] files = path.listFiles();
	      for(int i=0; i<files.length; i++) {
	         if(files[i].isDirectory()) {
	           deleteDirectory(files[i]);
	         }
	         else {
	           files[i].delete();
	         }
	      }
	    }
	    return( path.delete() );
	  }
  
  
  /**
   * 
   * For getting the WARC files from a directory and passing for extraction
   * @param inputWarcFilesDir, warc file directory
   * @param textFilesDir, text tiles directory for the files of each warc file
   * 
   */
  public void inputWarcFiles(String inputWarcFilesDir, String textFilesDir) {
	  File warcFilesDir = new File(inputWarcFilesDir);
	  if(warcFilesDir.exists()){
		  File[] warcFiles = warcFilesDir.listFiles();
		  try{
		  for(int warcFileCount=0; warcFileCount < warcFiles.length; warcFileCount++){
			   
			// System.out.println("fileName::"+textFilesDir + "\\"+  warcFiles[warcFileCount].getName().split("\\.")[0]);
			  
			  File textFileDir = new File(textFilesDir + "/"+  warcFiles[warcFileCount].getName().split("\\.")[0]);
			 // System.out.println("name:"+textFileDir.getName());
			  if(textFileDir.exists())
				  textFileDir.delete();
				  textFileDir.mkdir();
			  titlebufferedWriter = new BufferedWriter(new FileWriter(textFilesDir +"/"+warcFiles[warcFileCount].getName().split("\\.")[0]+".txt"));
				  
			  readWarcFile(inputWarcFilesDir, warcFiles[warcFileCount].getName(), textFilesDir, titlebufferedWriter);
			  
		  }
	  }
	  
	  catch(Exception e){
		  e.printStackTrace();
	  }
	  finally {
	try{  
				  if (inStream != null)
						inStream.close();
					if (titlebufferedWriter != null) {
						titlebufferedWriter.flush();
						titlebufferedWriter.close();
					}
					if (contentBufferedWriter != null) {
						contentBufferedWriter.flush();
						contentBufferedWriter.close();
					}
	} catch(Exception e){
			   e.printStackTrace();
		   }

	   }
	  
	  
  }
  

  }
}
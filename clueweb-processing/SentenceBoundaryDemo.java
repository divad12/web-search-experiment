import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.aliasi.sentences.MedlineSentenceModel;
import com.aliasi.sentences.SentenceModel;
import com.aliasi.tokenizer.IndoEuropeanTokenizerFactory;
import com.aliasi.tokenizer.Tokenizer;
import com.aliasi.tokenizer.TokenizerFactory;
import com.aliasi.util.Files;

/** Use SentenceModel to find sentence boundaries in text */
public class SentenceBoundaryDemo {

    static final TokenizerFactory TOKENIZER_FACTORY = IndoEuropeanTokenizerFactory.INSTANCE;
    static final SentenceModel SENTENCE_MODEL  = new MedlineSentenceModel();

    public static void main(String[] args) throws IOException {
	File file = new File("/home/aiman/WRP/WarcProject/output/plainText.txt");//new File(args[0]);
	BufferedWriter  outPutFile=new BufferedWriter(new FileWriter(new File("/home/aiman/WRP/WarcProject/output/formatText.txt")));
	String text = Files.readFromFile(file,"ISO-8859-1");
	//System.out.println("INPUT TEXT: ");
	//System.out.println(text);

	List<String> tokenList = new ArrayList<String>();
	List<String> whiteList = new ArrayList<String>();
	Tokenizer tokenizer = TOKENIZER_FACTORY.tokenizer(text.toCharArray(),0,text.length());
	tokenizer.tokenize(tokenList,whiteList);

	//System.out.println(tokenList.size() + " TOKENS");
	//System.out.println(whiteList.size() + " WHITESPACES");

	String[] tokens = new String[tokenList.size()];
	String[] whites = new String[whiteList.size()];
	tokenList.toArray(tokens);
	whiteList.toArray(whites);
	int[] sentenceBoundaries = SENTENCE_MODEL.boundaryIndices(tokens,whites);

	/*System.out.println(sentenceBoundaries.length 
			   + " SENTENCE END TOKEN OFFSETS");
	*/	
	if (sentenceBoundaries.length < 1) {
	    System.out.println("No sentence boundaries found.");
	    return;
	}
	int sentStartTok = 0;
	int sentEndTok = 0;
	for (int i = 0; i < sentenceBoundaries.length; ++i) {
	    sentEndTok = sentenceBoundaries[i];
	    System.out.print("<SENTENCE>");
	    outPutFile.write("<SENTENCE>");
	    for (int j=sentStartTok; j<=sentEndTok; j++) {
		System.out.print(tokens[j]+whites[j+1]);
		outPutFile.write(tokens[j]+whites[j+1]);
	    }
	    System.out.print("</SENTENCE>\n");
	    outPutFile.write("</SENTENCE>\n");
	    sentStartTok = sentEndTok+1;
	}
	outPutFile.close();
    }
}


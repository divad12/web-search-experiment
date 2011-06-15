import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
public class HtmlExtraction {
	public String  titleExtraction(String htmlContent) {
		Document doc = Jsoup.parse(htmlContent);
		return doc.title();
		
	}
	public String  htmlContentExtraction(String htmlContent) {
		Document doc = Jsoup.parse(htmlContent);
		return doc.html();
		
	}
	
}

package org.util;

public class GetTokenizeURL {
	
	public static String getTokenizeUrl(String url){
		String tokenUrl="";
		String protocol=url.substring(0,url.indexOf("//"));
		String words="";
		
		if(url.indexOf("?")==-1)
		words=url.substring(url.indexOf("/",url.indexOf("//")+2));
		else
		words=url.substring(url.indexOf("/",url.indexOf("//")+2),url.indexOf("?"));	
		tokenUrl="<tokenizedurl>url broken into tokens with protocol ("+
		protocol+"// stripped and then broken into words on . "+words+"" +
				" and truncated at the prescence of ? </tokenizedurl>";
		return tokenUrl;
	}
	

}


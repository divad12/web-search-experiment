package org.util;

import java.util.List;

import net.htmlparser.jericho.Element;
import net.htmlparser.jericho.Source;

public class GetAncherText {
	public static String getAncherText(Source source){
		String text="";
		List<Element> aEList=source.getAllElements("a");
		for(Element element:aEList){
			//System.out.println("the url is  "+element.getAttributeValue("href"));
			while (element.getAllElements().size()>1) {
				List<Element> inerElemnt=element.getAllElements();
				element=inerElemnt.get(inerElemnt.size()-1);
			}	
			//System.out.println("THe content is "+element.getContent());
			text+="<anchortext>"+element.getContent().toString().trim()+"</anchortext>"+"\n";
		}
		return text;
	}
}


package com;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.TreeMap;


public class RuleEngine {

	public static void execute(List<Product> cases, HashMap<String, TreeMap<Float, Double>> attrScoreTable, List<AttrImpPair> attrImpPairsList) throws SQLException
	{
		List<String> selectedAttributes = new ArrayList<String>();
		List<String> nonSelectedAttributes = DataAccess.getAllAttributesName();
		
		Iterator<AttrImpPair> attrImpPairsListIterator = attrImpPairsList.iterator();
		while(attrImpPairsListIterator.hasNext())
		{
			selectedAttributes.add(attrImpPairsListIterator.next().getAttrName());
		}
		
		nonSelectedAttributes.removeAll(selectedAttributes);
		
		Iterator<Product> casesIterator = cases.iterator();
		while(casesIterator.hasNext())
		{
			// The Rule Engine is to run on each selected Product/Case
			Product eachProduct = casesIterator.next();
			
			// Iterating through all the Non-Selected Attributes.
			Iterator<String> nonSelectedAttributeIterator = nonSelectedAttributes.iterator();
			
			while(nonSelectedAttributeIterator.hasNext())
			{
				String eachNonSelectedAttribute = nonSelectedAttributeIterator.next();
				
				// Iterate through all the Selected Attributes for every Non-Selected attribute and try to find a MATCH.
				attrImpPairsListIterator = attrImpPairsList.iterator();
				
				selectedAttributeIteration:
				while(attrImpPairsListIterator.hasNext())
				{
					AttrImpPair eachAttrImpPair = attrImpPairsListIterator.next();
					Rule rule;
					
					if( (rule = DataAccess.getRule(eachNonSelectedAttribute, DataAccess.getAttributeRange(eachNonSelectedAttribute, eachProduct
							.getValue(eachNonSelectedAttribute)), eachAttrImpPair)) == null )
						continue selectedAttributeIteration; // MATCH NOT FOUND
					
					// MATCH FOUND
					if(rule.getEffect() == Rule.EFFECT.POSITIVE)
					{
						// Contributing towards a Positive Effect increase the Relative Score
						eachProduct.insertStrongFeature(rule.getRemarks());
						eachProduct.setScore( eachProduct.getScore() + (attrScoreTable.get(eachAttrImpPair.getAttrName())
								.get(eachProduct.getValue(eachAttrImpPair.getAttrName()))/100) );
						
					}else if(rule.getEffect() == Rule.EFFECT.NEGETIVE)
					{
						// Contributing towards a Negative Effect decrease the Relative Score
						eachProduct.insertWeakFeature(rule.getRemarks());
						eachProduct.setScore( eachProduct.getScore() - (attrScoreTable.get(eachAttrImpPair.getAttrName())
								.get(eachProduct.getValue(eachAttrImpPair.getAttrName()))/100) );
					}
				}
			}
		}
	}
}

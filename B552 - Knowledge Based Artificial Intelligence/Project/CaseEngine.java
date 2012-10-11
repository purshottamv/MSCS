package com;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Set;
import java.util.TreeMap;


public class CaseEngine {

	public static enum discardColumnGroup {PRODUCT_ID, PRDOCUT_TYPE, MODEL_NO, BRAND, PROCESSOR_BRAND, OPERATING_SYSTEM};
	private static final int totalAttributeScope = 1000;
	private static final int nonSelectedAttributeContributionPercentage = 1;
	private static HashMap<String, Integer> perctWaitageMap = new HashMap<String, Integer>();
	
	static{
		perctWaitageMap.put("HIGH", 30);
        perctWaitageMap.put("MEDIUM", 20);
        perctWaitageMap.put("LOW", 10);
	}
	
	/**
	 * Returns the Initial Upper Bound Table
	 * @return
	 * @throws SQLException
	 */
	public static HashMap<String, Double> getAttributeUpperBoundTable() throws SQLException
	{
		int totalNoOfAttributes;
		int initEachAttrScore;
		
		// Create a MAP representing the Table
		HashMap<String, Double> table = new HashMap<String, Double>();
		
		// Get all the Attribute Names
		List<String> attributeNames = DataAccess.getAllAttributesName();
		
		// Set the Total Number Of Attributes
		totalNoOfAttributes = attributeNames.size();
		
		// Calculate the initial Average Score Of Each Attribute
		initEachAttrScore = totalAttributeScope/totalNoOfAttributes;
		
		// Populate the Table
		Iterator<String> attributeNamesIterator = attributeNames.iterator();
		while(attributeNamesIterator.hasNext())
		{
			table.put(attributeNamesIterator.next(), ((Integer)initEachAttrScore).doubleValue());
		}
		
		return table;
	}
	
	/**
	 * Perform the score distribution over a normalized Table.
	 * @param selectedAttrImpPairsList
	 * @param table
	 */
	public static void performScoreDistribution(List<AttrImpPair> selectedAttrImpPairsList, HashMap<String, Double> table)
	{
		int totalNoOfSelectedAttributes = selectedAttrImpPairsList.size();
		int totalNoOfAttributes = table.size();
		int totalNoOfNonSelectedAttributes = totalNoOfAttributes - totalNoOfSelectedAttributes;
		double eachNonSelectedAttrContibution;
		double totalNonSelectedAttrContibution;
		
		// Obtain the Contribution of Each Non Selected Attribute
		eachNonSelectedAttrContibution = ( ((Double [])table.values().toArray(new Double [table.values().size()]))[0] / 100 ) * nonSelectedAttributeContributionPercentage;
		
		// Obtain the Contribution of Non Selected Attributes Altogether
		totalNonSelectedAttrContibution = eachNonSelectedAttrContibution * totalNoOfNonSelectedAttributes;
		
		// Reduce the Non-Selected Attributes Scores (To reflect their contribution towards Selected Attributes).
		List<String> selectedAttributeList = new ArrayList<String>();
		Iterator<AttrImpPair> selectedAttrImpPairsIterator = selectedAttrImpPairsList.iterator();
		Iterator<String> tableKeySetIte = table.keySet().iterator();
		
		while(selectedAttrImpPairsIterator.hasNext())
		{
			selectedAttributeList.add(selectedAttrImpPairsIterator.next().getAttrName());
		}
		
		while(tableKeySetIte.hasNext())
		{
			String eachAttribute = tableKeySetIte.next();
			if(selectedAttributeList.contains(eachAttribute))
				continue;
			else
				table.put(eachAttribute, table.get(eachAttribute) - eachNonSelectedAttrContibution);
		}
		
		// Contribute the score extracted from the Non-Selected Attributes towards Selected Attributes
		// With a dynamic distribution scale calculated below. 
		double sumOfPW = 0;
		HashMap<String, Double> localPercWaitageMap = new HashMap<String, Double>();
		selectedAttrImpPairsIterator = selectedAttrImpPairsList.iterator();
		tableKeySetIte = table.keySet().iterator();
		
		while(selectedAttrImpPairsIterator.hasNext())
		{
			sumOfPW += perctWaitageMap.get(selectedAttrImpPairsIterator.next().getImp());
		}
		
		Iterator<String> perctWaitageKeyIte = perctWaitageMap.keySet().iterator();
		while(perctWaitageKeyIte.hasNext())
		{
			String type = perctWaitageKeyIte.next();
			localPercWaitageMap.put( type, ((Double)( (100.00 / sumOfPW) *  perctWaitageMap.get(type))).doubleValue() );
		}
		
		while(tableKeySetIte.hasNext())
		{
			String eachAttribute = tableKeySetIte.next();
			double initialValue = table.get(eachAttribute);
			String importanceLevel = null;
			if( (importanceLevel = getImportance(eachAttribute, selectedAttrImpPairsList)) == null)
				continue;
			double perctIncrement = localPercWaitageMap.get(importanceLevel);
			double increment = (totalNonSelectedAttrContibution / 100) * perctIncrement;
			
			if(selectedAttributeList.contains(eachAttribute))
				table.put(eachAttribute, initialValue + increment );
		}
		
	}
	
	/**
	 * Returns the user selected Importance level of a Selected Attribute.
	 * @param attributeName
	 * @param selectedAttrImpPairsList
	 * @return
	 */
	private static String getImportance(String attributeName, List<AttrImpPair> selectedAttrImpPairsList)
	{
		ListIterator<AttrImpPair> selectedAttrImpPairsListIte = selectedAttrImpPairsList.listIterator();
		while(selectedAttrImpPairsListIte.hasNext())
		{
			if(attributeName.equalsIgnoreCase(selectedAttrImpPairsListIte.next().getAttrName()))
				return selectedAttrImpPairsList.get(selectedAttrImpPairsListIte.previousIndex()).getImp();
		}
		
		return null;
	}
	
	/**
	 * Calculates the required query to extract all the required cases relatively subjective to the user requirements and fetches them.
	 * @param attrImpPairsList
	 * @return
	 * @throws SQLException
	 */
	public static List<Product> fetchCases(List<AttrImpPair> attrImpPairsList) throws SQLException
	{
		String fetchQuery = "SELECT * FROM [SalesAgent].dbo.PRODUCT WHERE ";
		List<AttrRangePair> attrRangePairList = new ArrayList<AttrRangePair>();
		Iterator<AttrImpPair> attrImpPairsListIterator = attrImpPairsList.iterator();
		while(attrImpPairsListIterator.hasNext())
		{
			attrRangePairList.add(DataAccess.getRange(attrImpPairsListIterator.next()));
		}
		
		// Construct The Query
		Iterator<AttrRangePair> attrRangePairListIte = attrRangePairList.iterator();
		while(attrRangePairListIte.hasNext())
		{
			AttrRangePair eachAttrRangePair = attrRangePairListIte.next();
			fetchQuery += eachAttrRangePair.getAttrName()+" >= "+eachAttrRangePair.getRange();
			
			if(attrRangePairListIte.hasNext())
				fetchQuery += " AND ";
		}
		
		System.out.println("QUERY -> "+fetchQuery);
		
		return DataAccess.getResults(fetchQuery);
	}
	
	/**
	 * Generates the Attribute Score table.
	 * @param attrUpperBoundTable
	 * @return
	 * @throws SQLException
	 */
	public static HashMap<String, TreeMap<Float, Double>> generateScoreAttributeTable(HashMap<String, Double> attrUpperBoundTable) throws SQLException
	{
		HashMap<String, TreeMap<Float, Double>> attrScoreTable = new HashMap<String, TreeMap<Float, Double>>();
		
		Set<String> attributeNameSet = attrUpperBoundTable.keySet();
		Iterator<String> attributeNameSetIte = attributeNameSet.iterator();
		
		while(attributeNameSetIte.hasNext())
		{
			String eachAttribute = attributeNameSetIte.next();
			
			// Fetch the Unique vales for a given Attribute.
			TreeMap<Float, Double> uniqueScoreMap = DataAccess.getUniqueAttributeValues(eachAttribute);
			Iterator<Float> uniqueScoreMapKeySetIterator = uniqueScoreMap.keySet().iterator();
			
			int index = 0;
			while(uniqueScoreMapKeySetIterator.hasNext())
			{
				// Calculate the Score for a given Unique Value of an Attribute
				uniqueScoreMap.put(uniqueScoreMapKeySetIterator.next(), (attrUpperBoundTable.get(eachAttribute)/uniqueScoreMap.keySet().size())*(index+1));
				index++;
			}
			
			// Hold them together in a DataStructure.
			attrScoreTable.put(eachAttribute, uniqueScoreMap);
		}
		
		return attrScoreTable;
	}
	
	/**
	 * Score all the cases provided according to the Attribute Score Table provided.
	 * @param cases
	 * @param attrScoreTable
	 */
	public static void scoreCases(List<Product> cases, HashMap<String, TreeMap<Float, Double>> attrScoreTable)
	{
		Iterator<Product> casesIterator = cases.iterator();
		while(casesIterator.hasNext())
		{
			double score = 0;
			Product eachProduct = casesIterator.next();
			
			Iterator<String> computationalAttributesIterator = attrScoreTable.keySet().iterator();
			while(computationalAttributesIterator.hasNext())
			{
				String eachCompAttr = computationalAttributesIterator.next();
				score += attrScoreTable.get(eachCompAttr).get(eachProduct.getValue(eachCompAttr));
			}
			
			eachProduct.setScore(score);
		}
	}
}

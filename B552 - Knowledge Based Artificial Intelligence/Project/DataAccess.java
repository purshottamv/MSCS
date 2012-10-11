package com;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;


public class DataAccess {

	/**
	 * Returns all the available Computational Attributes name.
	 * @return
	 * @throws SQLException
	 */
	public static List<String> getAllAttributesName() throws SQLException
	{
		String query = "select COLUMN_NAME from [SalesAgent].INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = 'PRODUCT'";
		Connection connection = MSDataBaseAccess.getInstance().getConnection();
		Statement statement = connection.createStatement();
		ResultSet resultSet = statement.executeQuery(query);
		List<String> result = new ArrayList<String>();
		
		while(resultSet.next())
		{
			String eachColumnName = resultSet.getString(1);
			try
			{
				CaseEngine.discardColumnGroup.valueOf(eachColumnName);
			}catch(IllegalArgumentException e)
			{
				result.add(eachColumnName);
			}
		}
		
		return result;
	}
	
	
	/**
	 * Returns the range of an Attribute.
	 * @param attrImpPair
	 * @return
	 * @throws SQLException
	 */
	public static AttrRangePair getRange(AttrImpPair attrImpPair) throws SQLException
	{
		String query = "SELECT LOW_RANGE FROM [SalesAgent].dbo.PRODUCT_RANGE WHERE ATTRIBUTE = '"+attrImpPair.getAttrName()+"' and IMPORTANCE = '"+attrImpPair.getImp()+"'";
		Connection connection = MSDataBaseAccess.getInstance().getConnection();
		Statement statement = connection.createStatement();
		ResultSet resultSet = statement.executeQuery(query);
		if(resultSet.next())
			return new AttrRangePair(attrImpPair.getAttrName(), resultSet.getString(1));

		return null;
	}
	
	/**
	 * Returns a set of Cases relatively subjective to the user requirements. 
	 * @param query
	 * @return
	 * @throws SQLException
	 */
	public static List<Product> getResults(String query) throws SQLException
	{
		List<Product> productList = new ArrayList<Product>();
		Connection connection = MSDataBaseAccess.getInstance().getConnection();
		Statement statement = connection.createStatement();
		ResultSet resultSet = statement.executeQuery(query);
		
		while(resultSet.next())
		{
			productList.add(new Product(resultSet.getInt(1), resultSet.getString(2), resultSet.getString(3), resultSet.getString(4), resultSet.getFloat(5), resultSet.getFloat(6),
					resultSet.getFloat(7), resultSet.getString(8), resultSet.getFloat(9), resultSet.getFloat(10), resultSet.getFloat(11), resultSet.getFloat(12), resultSet.getFloat(13),
					resultSet.getFloat(14), resultSet.getString(15), resultSet.getFloat(16), resultSet.getFloat(17), resultSet.getFloat(18), resultSet.getFloat(19)));
		}
		
		return productList;
	}
	
	/**
	 * Returns the Unique values for a given Attribute. 
	 * @param attributeName
	 * @return
	 * @throws SQLException
	 */
	public static TreeMap<Float, Double> getUniqueAttributeValues(String attributeName) throws SQLException
	{
		String query = "SELECT DISTINCT "+attributeName+" FROM [SalesAgent].dbo.PRODUCT";
		TreeMap<Float, Double> uniqueScoreMap = new TreeMap<Float, Double>();
		Connection connection = MSDataBaseAccess.getInstance().getConnection();
		Statement statement = connection.createStatement();
		ResultSet resultSet = statement.executeQuery(query);
		
		while(resultSet.next())
		{
			uniqueScoreMap.put(resultSet.getFloat(1), null);
		}
		
		return uniqueScoreMap;
	}
	
	/**
	 * Returns the Range of an Attribute for a given Value.
	 * @param attribute
	 * @param value
	 * @return
	 * @throws SQLException
	 */
	public static Integer getAttributeRange(String attribute, double value) throws SQLException
	{
		String query = "SELECT LOW_RANGE FROM [SalesAgent].dbo.PRODUCT_RANGE WHERE ATTRIBUTE = '"+attribute+"' AND (IMPORTANCE = 'HIGH' OR IMPORTANCE = 'MEDIUM')" +
				" ORDER BY LOW_RANGE";
		
		Connection connection = MSDataBaseAccess.getInstance().getConnection();
		Statement statement = connection.createStatement();
		ResultSet resultSet = statement.executeQuery(query);
		
		if( resultSet.next() && (value < Double.parseDouble(resultSet.getString(1))) )
			return 0;
		else if( resultSet.next() && (value >= Double.parseDouble(resultSet.getString(1))) )
			return 1;
		else
			return null;
	}
	
	/**
	 * Returns all the Rules that could be matched with a given sequence.
	 * @param nonSelectedAttribute
	 * @param importance
	 * @param selectedAttrImpPair
	 * @return
	 * @throws SQLException
	 */
	public static Rule getRule(String nonSelectedAttribute, Integer importance, AttrImpPair selectedAttrImpPair) throws SQLException
	{
		if(importance == null)
			return null;
		
		String query = "SELECT EFFECT, REMARKS FROM [SalesAgent].dbo.RULES WHERE NON_SELECTED_ATTRIBUTE = '"+nonSelectedAttribute+"' AND " +
				"SELECTED_ATTRIBUTE = '"+selectedAttrImpPair.getAttrName()+"' AND MAGNITUDE = "+importance;
		
		Connection connection = MSDataBaseAccess.getInstance().getConnection();
		Statement statement = connection.createStatement();
		ResultSet resultSet = statement.executeQuery(query);
		
		if(resultSet.next())
			return new Rule(resultSet.getInt(1), resultSet.getString(2));
		return null;
	}
	
}

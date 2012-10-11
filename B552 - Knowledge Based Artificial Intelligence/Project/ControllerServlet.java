package com;

import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.TreeMap;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class ControllerServlet
 */

/**
 * This class behaves as a Controller, which receives the request from the client and propagates the same to the following entities as mentioned below as part of the business
 * logic:
 *  1. Initially once the controller receives a request it forwards the same to the Case Engine (In Parts).
 *  2. Once it receives the results back from the Case Engine it forwards the same to the Rule Engine.  
 * 	3. Once it receives the results back from the Rule Engine it packs them up and sends them back to the requesting client.
 *
 */
public class ControllerServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
	private static final String attrIdentifier = "attribute";
	private static final String impIdentifier = "importance";
	
	private RequestDispatcher requestDispatcher;

    /**
     * Default constructor. 
     */
    public ControllerServlet() {
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
    /**
     * This is a Overridden method to handle HTTP GET Requests from client
     */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		int count = 1;
		String attr = null;
		String imp = null;
		List<AttrImpPair> attrImpPairList = new ArrayList<AttrImpPair>();
		
		// Populating the Selected Attribute list from the client
		while( (attr = request.getParameter(attrIdentifier+count))!=null && (imp = request.getParameter(impIdentifier+count))!=null )
		{
			attrImpPairList.add(new AttrImpPair(attr, imp));
			attr = null;
			imp = null;
			count++;
		}
		
		try
		{
			//Calling the Business Logic and packing the Response up.
			request.setAttribute("Results", callBusinessLogic(attrImpPairList));
	        requestDispatcher = request.getRequestDispatcher("results.jsp");
	        requestDispatcher.forward(request, response);
		}catch (Exception e) {
			// TODO: handle exception
			request.setAttribute("Error", e);
	        requestDispatcher = request.getRequestDispatcher("error.jsp");
	        requestDispatcher.forward(request, response);
		}
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	/**
	 * This is a Overridden method to handle HTTP POST Requests from client
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		int count = 1;
		String attr = null;
		String imp = null;
		List<AttrImpPair> attrImpPairList = new ArrayList<AttrImpPair>();
		
		// Populating the Selected Attribute list from the client
		while( (attr = request.getParameter(attrIdentifier+count))!=null && (imp = request.getParameter(impIdentifier+count))!=null )
		{
			attrImpPairList.add(new AttrImpPair(attr, imp));
			attr = null;
			imp = null;
			count++;
		}
		
		try
		{
			//Calling the Business Logic and packing the Response up.
			request.setAttribute("Results", callBusinessLogic(attrImpPairList));
	        requestDispatcher = request.getRequestDispatcher("results.jsp");
	        requestDispatcher.forward(request, response);
		}catch (Exception e) {
			// TODO: handle exception
			request.setAttribute("Error", e);
	        requestDispatcher = request.getRequestDispatcher("error.jsp");
	        requestDispatcher.forward(request, response);
		}
	}
	
	/**
	 * @throws SQLException 
	 * The Business Logic Flow is implemented here.
	 */
	private Results callBusinessLogic(List<AttrImpPair> attrImpPairsList) throws SQLException
	{
		HashMap<String, Double> attrUpperBoundTable;
		HashMap<String, TreeMap<Float, Double>> attrScoreTable;
		List<Product> cases;
		
		// Get the Initial Attribute Upper Bound Table 
		attrUpperBoundTable = CaseEngine.getAttributeUpperBoundTable();
		
		// Perform Score Distribution:- Obtain some definitive scores from Non-Selected Attributes and Distribute them among Selected Attributes.
		CaseEngine.performScoreDistribution(attrImpPairsList, attrUpperBoundTable);
		
		// Fetch the relevant cases from the Knowledge Base based on their {Attributes, Importance} level.
		cases = CaseEngine.fetchCases(attrImpPairsList);
		
		// Generate the Attribute Score Table considering the Attribute Upper-bound Table generated prior to this.
		attrScoreTable = CaseEngine.generateScoreAttributeTable(attrUpperBoundTable);
		
		// Score each of the retrieved CASES based on the Attribute Score Table generated prior to this.
		CaseEngine.scoreCases(cases, attrScoreTable);
		
		// Printing the cases after Scoring (Primarily for Debugging Benefits)
		System.out.println(" ==== Results After Scoring Cases ==== ");
		printResults(cases);
		
		// Run the RULE ENGINE over the Cases
		RuleEngine.execute(cases, attrScoreTable, attrImpPairsList);
		
		// Printing the cases after executing the Rule Engine over them (Primarily for Debugging Benefits)
		System.out.println(" ==== Results After Running Rule Engine ==== ");
		printResultsWithRules(cases);
		
		// Sort the Results considering the Scores.
		Product products[] = cases.toArray(new Product[cases.size()]);
		Arrays.sort(products);
		cases = Arrays.asList(products);
		
		// Printing the cases after Sorting (Primarily for Debugging Benefits)
		System.out.println(" ==== Results After Sorting The Results ==== ");
		printResultsWithRules(cases);
		
		// Pack the Results up to be sent back to the requesting Client.
		return new Results(cases);
	}
	
	/**
	 * Iterates through all the cases and prints them (Primarily used for Server side Debugging purposes)
	 * @param cases
	 */
	public static void printResults(List<Product> cases)
	{
		Iterator<Product> casesIterator = cases.iterator();
		while(casesIterator.hasNext())
		{
			Product eachProduct = casesIterator.next();
			System.out.println(eachProduct.getPRODUCT_ID() +" -> "+ eachProduct.getScore());
		}
		System.out.println(" ==================================================================================================== ");
	}
	
	/**
	 * Iterates through all the cases and prints them (Primarily used for Server side Debugging purposes)
	 * @param cases
	 */
	public static void printResultsWithRules(List<Product> cases)
	{
		Iterator<Product> casesIterator = cases.iterator();
		while(casesIterator.hasNext())
		{
			Product eachProduct = casesIterator.next();
			System.out.println(eachProduct.getPRODUCT_ID() +" -> "+ eachProduct.getScore());
			
			System.out.println("Strong Features:");
			for(String eachStrongFeature:eachProduct.getStrongFeatures())
			{
				System.out.println(eachStrongFeature);
			}
					
			System.out.println("Weak Features:");
			for(String eachWeakFeature:eachProduct.getWeakFeatures())
			{
				System.out.println(eachWeakFeature);
			}
			
			System.out.println();
			System.out.println(" -------------- ******** -------------- ");
			System.out.println();
		}
		System.out.println(" ==================================================================================================== ");
	}

}

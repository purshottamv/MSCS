package com;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;


public class Results {

	private List<Product> recommendedProducts;
	private List<Product> discardedProducts;
	
	private static final int threshHold = 3;
	
	/**
	 * Constructor used for splitting up the cases and Wrap them up.
	 * @param cases
	 */
	public Results(List<Product> cases)
	{
		recommendedProducts = new ArrayList<Product>();
		discardedProducts = new ArrayList<Product>();
		
		split(cases);
		
		displayResults();
	}
	
	public List<Product> getRecommendedProducts() {
		return recommendedProducts;
	}

	public List<Product> getDiscardedProducts() {
		return discardedProducts;
	}
	
	private void split(List<Product> cases)
	{
		ListIterator<Product> casesIte = cases.listIterator();
		
		while(casesIte.hasNext())
		{
			if(casesIte.nextIndex() < threshHold)
				recommendedProducts.add(casesIte.next());
			else
				discardedProducts.add(casesIte.next());
		}
	}
	
	/**
	 * Method only used for Server side Debugging purpose.
	 */
	private void displayResults()
	{
		System.out.println(" ==== RECOMMENDED PRODUCTS ==== ");
		Iterator<Product> recommendedProductsIte = recommendedProducts.iterator();
		while(recommendedProductsIte.hasNext())
		{
			Product eachProduct = recommendedProductsIte.next();
			System.out.println(eachProduct.getPRODUCT_ID() +" -> "+ eachProduct.getScore());
		}
		
		System.out.println("");
		System.out.println(" ==== DISCARDED PRODUCTS ==== ");
		Iterator<Product> discardedProductsIte = discardedProducts.iterator();
		while(discardedProductsIte.hasNext())
		{
			Product eachProduct = discardedProductsIte.next();
			System.out.println(eachProduct.getPRODUCT_ID() +" -> "+ eachProduct.getScore());
		}
	}
}

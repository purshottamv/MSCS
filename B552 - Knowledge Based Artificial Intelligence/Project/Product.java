package com;

import java.util.ArrayList;
import java.util.List;


public class Product implements Comparable<Product>{
	
	private int PRODUCT_ID;
	private String PRDOCUT_TYPE;
	private String MODEL_NO;
	private String BRAND;
	private float DISPLAY_SIZE;
	private float PROCESSOR_SPEED;
	private float NO_OF_CORES;
	private String PROCESSOR_BRAND;
	private float RAM_SIZE;
	private float HDD_SIZE; 
	private float PRICE;
	private float BATTERY_LIFE;
	private float CACHE_SIZE;
	private float GRAPHICS_CARD_MEM_SIZE;
	private String OPERATING_SYSTEM;
	private float OPERATING_SYSTEM_RANK;
	private float WEIGHT_OF_PRODUCT;
	private float WARRANTY;
	private float NO_OF_USB_PORTS;
	
	private static enum computationalField{DISPLAY_SIZE, PROCESSOR_SPEED, NO_OF_CORES, RAM_SIZE, HDD_SIZE, PRICE, BATTERY_LIFE, CACHE_SIZE, GRAPHICS_CARD_MEM_SIZE, 
		OPERATING_SYSTEM_RANK, WEIGHT_OF_PRODUCT, WARRANTY, NO_OF_USB_PORTS}
	
	private double score;
	
	private List<String> strongFeatures;
	private List<String> weakFeatures;
	
	public Product(int PRODUCT_ID, String PRDOCUT_TYPE, String MODEL_NO, String BRAND, float DISPLAY_SIZE, float PROCESSOR_SPEED, float NO_OF_CORES, String PROCESSOR_BRAND,
			float RAM_SIZE, float HDD_SIZE, float PRICE, float BATTERY_LIFE, float CACHE_SIZE, float GRAPHICS_CARD_MEM_SIZE, String OPERATING_SYSTEM, float OPERATING_SYSTEM_RANK, 
			float WEIGHT_OF_PRODUCT, float WARRANTY, float NO_OF_USB_PORTS)
	{
		this.PRODUCT_ID = PRODUCT_ID;
		this.PRDOCUT_TYPE = PRDOCUT_TYPE;
		this.MODEL_NO = MODEL_NO;
		this.BRAND = BRAND;
		this.DISPLAY_SIZE = DISPLAY_SIZE;
		this.PROCESSOR_SPEED = PROCESSOR_SPEED;
		this.NO_OF_CORES = NO_OF_CORES;
		this.PROCESSOR_BRAND = PROCESSOR_BRAND;
		this.RAM_SIZE = RAM_SIZE;
		this.HDD_SIZE = HDD_SIZE; 
		this.PRICE = PRICE;
		this.BATTERY_LIFE = BATTERY_LIFE;
		this.CACHE_SIZE = CACHE_SIZE;
		this.GRAPHICS_CARD_MEM_SIZE = GRAPHICS_CARD_MEM_SIZE;
		this.OPERATING_SYSTEM = OPERATING_SYSTEM;
		this.OPERATING_SYSTEM_RANK = OPERATING_SYSTEM_RANK; 
		this.WEIGHT_OF_PRODUCT = WEIGHT_OF_PRODUCT;
		this.WARRANTY = WARRANTY;
		this.NO_OF_USB_PORTS = NO_OF_USB_PORTS;
		
		strongFeatures = new ArrayList<String>();
		weakFeatures = new ArrayList<String>();
	}
	
	public void insertStrongFeature(String strongFeature)
	{
		strongFeatures.add(strongFeature);
	}
	
	public void insertWeakFeature(String weakFeature)
	{
		weakFeatures.add(weakFeature);
	}
	
	public List<String> getStrongFeatures() {
		return strongFeatures;
	}

	public List<String> getWeakFeatures() {
		return weakFeatures;
	}
	
	public Float getValue(String attributeName)
	{
		switch(computationalField.valueOf(attributeName))
		{
			case BATTERY_LIFE:
				return getBATTERY_LIFE();
			case CACHE_SIZE:
				return getCACHE_SIZE();
			case DISPLAY_SIZE:
				return getDISPLAY_SIZE();
			case GRAPHICS_CARD_MEM_SIZE:
				return getGRAPHICS_CARD_MEM_SIZE();
			case HDD_SIZE:
				return getHDD_SIZE();
			case NO_OF_CORES:
				return getNO_OF_CORES();
			case NO_OF_USB_PORTS:
				return getNO_OF_USB_PORTS();
			case OPERATING_SYSTEM_RANK:
				return getOPERATING_SYSTEM_RANK();
			case PRICE:
				return getPRICE();
			case PROCESSOR_SPEED:
				return getPROCESSOR_SPEED();
			case RAM_SIZE:
				return getRAM_SIZE();
			case WARRANTY:
				return getWARRANTY();
			case WEIGHT_OF_PRODUCT:
				return getWEIGHT_OF_PRODUCT();
			default:
				return null;
		}
		
	}
	
	public int getPRODUCT_ID() {
		return PRODUCT_ID;
	}

	public String getPRDOCUT_TYPE() {
		return PRDOCUT_TYPE;
	}

	public String getMODEL_NO() {
		return MODEL_NO;
	}

	public String getBRAND() {
		return BRAND;
	}

	public float getDISPLAY_SIZE() {
		return DISPLAY_SIZE;
	}

	public float getPROCESSOR_SPEED() {
		return PROCESSOR_SPEED;
	}

	public float getNO_OF_CORES() {
		return NO_OF_CORES;
	}

	public String getPROCESSOR_BRAND() {
		return PROCESSOR_BRAND;
	}

	public float getRAM_SIZE() {
		return RAM_SIZE;
	}

	public float getHDD_SIZE() {
		return HDD_SIZE;
	}

	public float getPRICE() {
		return PRICE;
	}

	public float getBATTERY_LIFE() {
		return BATTERY_LIFE;
	}

	public float getCACHE_SIZE() {
		return CACHE_SIZE;
	}

	public float getGRAPHICS_CARD_MEM_SIZE() {
		return GRAPHICS_CARD_MEM_SIZE;
	}

	public String getOPERATING_SYSTEM() {
		return OPERATING_SYSTEM;
	}
	
	public float getOPERATING_SYSTEM_RANK() {
		return OPERATING_SYSTEM_RANK;
	}

	public float getWEIGHT_OF_PRODUCT() {
		return WEIGHT_OF_PRODUCT;
	}

	public float getWARRANTY() {
		return WARRANTY;
	}

	public float getNO_OF_USB_PORTS() {
		return NO_OF_USB_PORTS;
	}
	
	public double getScore() {
		return score;
	}

	public void setScore(double score) {
		this.score = score;
	}

	@Override
	public int compareTo(Product product) {
		// TODO Auto-generated method stub
		if(this.score < product.getScore())
			return 1;
		else if(this.score > product.getScore())
			return -1;
		return 0;
	}

}

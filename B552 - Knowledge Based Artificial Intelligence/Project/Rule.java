package com;

import java.util.HashMap;


public class Rule {

	public static enum EFFECT{POSITIVE, NEGETIVE}
	public static enum MAGNITUDE{HIGH, LOW}
	public static HashMap<Integer, Rule.EFFECT> effectMap= new HashMap<Integer, Rule.EFFECT>();
	public static HashMap<Rule.MAGNITUDE, Integer> magnitudeMap= new HashMap<Rule.MAGNITUDE, Integer>();
	
	private Rule.EFFECT effect;
	private String remarks;
	
	static{
		effectMap.put(1, Rule.EFFECT.POSITIVE);
		effectMap.put(0, Rule.EFFECT.NEGETIVE);
		
		magnitudeMap.put(Rule.MAGNITUDE.HIGH, 1);
		magnitudeMap.put(Rule.MAGNITUDE.LOW, 0);
	}
	
	public Rule(int effect, String remarks) {
		// TODO Auto-generated constructor stub
		this.effect = effectMap.get(effect);
		this.remarks = remarks;
	}
	
	public Rule.EFFECT getEffect() {
		return effect;
	}

	public String getRemarks() {
		return remarks;
	}
	
	public static int getMagnitude(Rule.MAGNITUDE magnitude)
	{
		return magnitudeMap.get(magnitude);
	}
}

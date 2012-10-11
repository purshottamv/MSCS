import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class run_ps {
	
	public static boolean success_state_antec = false;
	public static int antecs_cycle = 0;
	public static int antecs_cycle_round = 0;
	public static int originalWMSize;

	public static void main(String args[])
	{
		try {
			Class wm1 = (ClassLoader.getSystemClassLoader().loadClass(args[0]));
			Object wm = wm1.newInstance();
			ArrayList facts = ((wm)wm).facts;
			Class rules1 = ClassLoader.getSystemClassLoader().loadClass(args[1]);
			Object rules = rules1.newInstance();
			ArrayList rulesList = ((rules)rules).rules;
			
			originalWMSize = facts.size();
			
			System.out.println("CYCLE 1\n");
			printWorkingMemory(facts);
			ArrayList newPattern = match_rules(rulesList,facts);
			// append the working memory with new patterns
			for(Object z:newPattern)
			{
				ArrayList fact = new ArrayList();
				fact.add(new ArrayList(Arrays.asList(z.toString())));
				facts.add(fact);
			}
			int cycleCount = 2;
			while(!newPattern.isEmpty())
			{
				System.out.println("\nCYCLE " + (cycleCount++));
				printWorkingMemory(facts);
				newPattern = match_rules(rulesList,facts);
				// append the working memory with new patterns
				for(Object z:newPattern)
				{
					ArrayList fact = new ArrayList();
					fact.add(new ArrayList(Arrays.asList(z.toString())));
					facts.add(fact);
				}	
			}
			System.out.println("\nFollowing Assertions were added after applying the Production Rules : ");
			int i=0;
			for(Object x:facts)
			{
				if(i >= originalWMSize)
				{
					ArrayList fact = (ArrayList)x;
					System.out.println(fact.get(0).toString());
				}
				i++;
			}
		System.out.println("");
			
		}catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	private static void printWorkingMemory(ArrayList facts)
	{
		System.out.println("Current WM :");
		for(Object x:facts)
		{
			ArrayList fact = (ArrayList)x;
			System.out.println(fact.get(0).toString());
		}
		System.out.println("");
	}
	
	private static ArrayList match_rules(ArrayList rulesList, ArrayList facts)
	{
		ArrayList newPatterns = new ArrayList();
		for(Object x:rulesList)
		{
			ArrayList y = (ArrayList)x;
			ArrayList newPatterns1 = match_rule(y,facts);
			// Add the newPattern into ArrayList to be returned
			newPatterns.addAll(newPatterns1);
		}
		return newPatterns;
	}

	private static boolean isVariable(String obj)
	{
		if(obj.contains("%"))
			return true;
		else
			return false;
	}
	
	private static ArrayList match_rule(ArrayList rule,ArrayList facts)
	{
		// Match-rule returns the list of new patterns to be added to working memory as a result of matching the rule. 
		// The list will be empty if either the rule fails to be matched or 
		// all of the instantiated consequents which result are already in the working memory. 
		ArrayList antecedentList = (ArrayList)rule.get(1);
		ArrayList queue = new ArrayList();
		ArrayList state1 = new ArrayList();
		
		state1.add(antecedentList);
		state1.add(new ArrayList()); // This is the list containing substitutions
		
		queue.add(state1);
		
		System.out.print("Attempting to match rule "+ (String)rule.get(0));
		if(mr_helper(queue, facts))
		{
			//  execute takes a substitution, the right-hand side of a rule (one or more consequents), and a working memory.
			//  Execute applies the substitution to each of the consequents in the right-hand side 
			ArrayList newPatterns = execute((ArrayList)(state1.get(1)),(ArrayList)(rule.get(2)),facts);
			return newPatterns;
		}
		else
		{
			// No Substitutions were generated
			System.out.println(" : Failed");
		}
		return (new ArrayList());
	}
	
	private static ArrayList execute(ArrayList subs,ArrayList consequents, ArrayList facts)
	{
		boolean factExists = false;
		ArrayList patterns = new ArrayList();
		for(Object cons:consequents)
		{
			String pattern = "";
			pattern = (String)cons;
			for(Object x:subs)
			{			
				ArrayList sub = (ArrayList)x;
				pattern = substitute(sub,pattern);
			}
			// Check if the pattern already exists in the working memory. and if it is not, adds it to an accumulated list of new patterns
			for(Object x:facts)
			{
				ArrayList fact = (ArrayList)x;
				if((((ArrayList)fact.get(0)).get(0)).equals(pattern))
				{
					factExists = true;
					break;
				}
			}
			if(factExists == false)
			{
				System.out.println(" : Match Succeeds\nAdding assertions to WM: "+pattern.toString());
				patterns.add(pattern);
			}
			else
			{
				System.out.println(" : Assertion already exists\n"+pattern.toString());
			}
			
		}
		return patterns;
	}
	private static String substitute(ArrayList sub,String pattern)
	{
		// takes a substitution and a pattern and returns the pattern with the variables from the substitution substituted into it
		String vName = (String)sub.get(0);
		String vValue = (String)sub.get(1);
		pattern = pattern.replaceAll(vName, vValue);
		return pattern;
	}
	private static boolean mr_helper(ArrayList queue, ArrayList facts)
	{
		ArrayList s1;
		if(queue.isEmpty())
		{
			return false;
		}
		else
		{
			s1 = (ArrayList)queue.get(0);
			ArrayList antecedents = (ArrayList)s1.get(0);
			if(antecedents.isEmpty())
			{
				// Goal State. Call execute on the rest of the queue.
				// Remove the first state from queue and 
				// add the assertion returned by execute into working memory
				//	mr_helper(queue,facts);
				return false;
			}
			else // state1 has antecedents
			{
				//  if state1 does have antecedents, use
	            //   "match-antecedent" on them, along with wm(facts) and the substitutions in state1.
				ArrayList subs = (ArrayList)s1.get(1);
				match_antecedent(antecedents,subs, facts); // continue				
			}
		}
		// Check if any substitutions were returned from match_atecedent. If yes return true else return false.
		if(!((ArrayList)s1.get(1)).isEmpty())
		{
			return true;
		}
		else 
			return false;
	}

	private static void match_antecedent(ArrayList antecedents,ArrayList subs,ArrayList facts)
	{
		// antecedents contains all the antecedents of a single rule.
		ArrayList workingMemoryLeft = new ArrayList();
		//workingMemoryLeft.addAll(facts);
		ArrayList temporary_ant_list = new ArrayList();
		temporary_ant_list.addAll(antecedents);
		//ArrayList antec = (ArrayList)antecedents.get(0);
		do{
			success_state_antec = false;
			workingMemoryLeft.addAll(facts);
			ma_helper(temporary_ant_list, subs,workingMemoryLeft);
			if(success_state_antec)
			{
				// success_state_antec is set to true if any of the facts matches with one antecedent in a single rule involving multiple antecedents
				temporary_ant_list.remove(0);
			}else{
				// If a particular antecedent doesn't match then remove from the head of the queue 
				// and place it in the end of the queue. This is because it is possible that
				// the antecedent might match after some later antecedents are matched.
				temporary_ant_list.add(temporary_ant_list.get(0));
				temporary_ant_list.remove(0);
			}
		}while(!temporary_ant_list.isEmpty() && doContinue(temporary_ant_list.size(), subs));
	}
	
	private static boolean doContinue(int size, ArrayList subs)
	{
		// Checks if any iteration between multiple antecedents in a single state is a success.
		// If it is then it moves to a new state.
		// A State comprises of antecedents of a rule and the substitution list
		// This function avoids infinite loop.
		if(size == antecs_cycle && antecs_cycle_round != 0)
		{
			antecs_cycle_round--;
			return true;
		}
		else if(size == antecs_cycle && antecs_cycle_round == 0)
		{
			subs.clear();
			antecs_cycle = 0;
			return false;
		}
		else{
			antecs_cycle = size;
			antecs_cycle_round = size;
			return true;
		}
	}
	
	private static void ma_helper(ArrayList antecedents, ArrayList subs, ArrayList workingMemoryLeft)
	{
		// This is a recursive function. 
		// It calls the unify function for every antecedent in the antecedent list and
		// with every fact in the working memory.
		// The working memory set reduces in each recursive call until no fact is pending in the working memory set.
		String antec = (String)antecedents.get(0);
		
		if(workingMemoryLeft.isEmpty())
			return;
		else
		{
			if(unify(antec, (ArrayList)workingMemoryLeft.get(0), subs) != null)
			{
				// Unification Succeeded
				// If the unification succeeds, call ma-helper with the new state condensed onto states and the rest of wm-left
				// success_state_antec is set to true if any of the facts matches with one antecedent in a single rule involving multiple antecedents
				success_state_antec = true;	// set the flag success_state_antec to true if the antecedent matches with the fact 
			}
			// Whether unification succeeded or not,  call ma_helper recursively until no fact is pending in the working memory set. 
			workingMemoryLeft.remove(0);
			ma_helper(antecedents,subs,workingMemoryLeft);
		}
	}
	
	private static ArrayList unify(String antec, ArrayList fact, ArrayList subs)
	{
		// Initially subs is blank.
		String factString = (String)((ArrayList)fact.get(0)).get(0);
		if(fact.size() == 1 && (antec).equals(factString))
		{
			// If pat1 and pat2 are equal, return substitution
			return subs; // return substitution. 
		}
		
		if((Arrays.asList((antec).split(" "))).size() == 1 ||
				(Arrays.asList((factString).split(" "))).size() == 1	)
				return null;
				
		// Check if antec contains any variable
		String ant;
		if(isVariable(antec))
		{
			// Apply the substitution if available
			ant = antec;
			if(!subs.isEmpty())
			{
				for(Object x:subs)	// Iterating through each substitution and substituting ( if any) in the antec
				{
					ArrayList y = (ArrayList)x;
					String vName = (String)y.get(0);
					String vValue = (String)y.get(1);

					if(ant.contains(vName))
					{
						ant = ant.replaceAll(vName, vValue/*.trim()*/);
					}
				}
				
				if((ant).equals(factString))
				{
					// If pat1 and pat2 are equal, return substitution
					return subs; // return substitution. 
				}
			}
			// Unification between the modified antec and fact start here to create new subs if any
			
			List<String> antList = Arrays.asList(ant.split(" "));
			List<String> factList = Arrays.asList((factString).split(" "));
			
			if(compare(antList, factList))
			{
				if(antList.size() == factList.size())
				{
					ArrayList temporarySubstitution = new ArrayList();
					ArrayList temp;
					for(int i=0;i < antList.size();i++)
					{
						if( isVariable((String)antList.get(i)) )
						{
							// Add the variables to the substitution List
							temp = new ArrayList();
							temp.add((String)antList.get(i));
							temp.add((String)factList.get(i));
							temporarySubstitution.add(temp);
						}
						else if(!((String)antList.get(i)).equalsIgnoreCase((String)factList.get(i))) // Compare the non-variables
						{
							// if the non-variables in the antecedent and the fact don't match, return null.
							temporarySubstitution.clear();
							return null;
						}
					}
					subs.addAll(temporarySubstitution);
					return subs;
				}
			}
		}
		return null;
	}
	
	private static boolean compare(List antList, List factList)
	{
		// compare the two lists word by word
		if(antList.size() == factList.size())
		{
			for(int i=0;i < antList.size();i++)
			{
				if(((String)antList.get(i)).equals((String)factList.get(i)))
					return true;
			}
		}
		
		return false;
	}
	
	public static void main1(String args[])
	{
		ArrayList antec = new ArrayList();
		ArrayList fact = new ArrayList();
		ArrayList subs = new ArrayList();
/*		antec.add("drop %x %y");
		
		fact.add("drop arnold_class_csci 552");
		ArrayList tmpSubs = new ArrayList();
		tmpSubs.add("%x");
		tmpSubs.add("%z");
		subs.add(tmpSubs);
		ArrayList x = unify(antec, fact, subs);
		for(Object a:x)
		{
			System.out.println("varname = "+((ArrayList)a).get(0));
			System.out.println("varval = "+((ArrayList)a).get(1));
		}*/
		wm objwm = new wm();
		rules objrules = new rules();
		
		match_antecedent((ArrayList)((ArrayList)(objrules.rules.get(6))).get(1),subs,objwm.facts);
		for(Object o:subs)
		{
			ArrayList oo = (ArrayList)o;
			System.out.println("varname = "+(oo).get(0));
			System.out.println("varval = "+(oo).get(1));
			System.out.println(" ========================================== ");
		}
		//match_antecedent(ArrayList antecedents,ArrayList subs,ArrayList facts);		
	}
	
}
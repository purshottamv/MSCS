import java.util.*;

/*
 * Note : Please note that the variable names start with '%' as opposed to '?' because I was 
 * finding problem with replacing variables (strings) containing '?'  
 * */
public class rules {
	
	ArrayList rules;
	rules()
	{
		rules = new ArrayList();
		ArrayList rule1 = new ArrayList();
		ArrayList rule2 = new ArrayList();
		ArrayList rule3 = new ArrayList();
		ArrayList rule4 = new ArrayList();
		ArrayList rule5 = new ArrayList();
		ArrayList rule6 = new ArrayList();
		ArrayList rule7 = new ArrayList();
		ArrayList rule8 = new ArrayList();
		ArrayList rule9 = new ArrayList();
		ArrayList rule10 = new ArrayList();
		ArrayList rule11 = new ArrayList();
		ArrayList rule12 = new ArrayList();
		ArrayList rule13 = new ArrayList();
		
		//If a patient has a very high fever, the patient has a high fever.
		rule1.add("1");
		rule1.add(new ArrayList(Arrays.asList("%x very_high fever")));
		rule1.add(new ArrayList(Arrays.asList("%x high_fever")));
		rules.add(rule1);
		
		// If a patient has whooping cough, the patient has a cough.
		rule2.add("2");
		rule2.add(new ArrayList(Arrays.asList("%x has whooping_cough")));
		rule2.add(new ArrayList(Arrays.asList(("%x cough"))));
		rules.add(rule2);
		
		//If a patient has poison ivy, the patient has a rash.
		rule3.add("3");
		rule3.add(new ArrayList(Arrays.asList(("%x poison_ivy"))));
		rule3.add(new ArrayList(Arrays.asList(("%x rash"))));
		rules.add(rule3);
		
		// If a patient has a high fever and congestion, the patient has the flu.
		rule4.add("4");
		rule4.add(new ArrayList(Arrays.asList("%x high_fever","%x congestion")));
		rule4.add(new ArrayList(Arrays.asList(("%x flu"))));
		rules.add(rule4);
		
		//If a patient has a rash and no high fever, the patient has poison ivy.
		rule5.add("5");
		rule5.add(new ArrayList(Arrays.asList("%x rash","%x no_high_fever")));
		rule5.add(new ArrayList(Arrays.asList(("%x poison_ivy"))));
		rules.add(rule5);
		
		//If a patient has a cough and a very high fever, the patient has whooping cough.
		rule6.add("6");
		rule6.add(new ArrayList(Arrays.asList("%x cough","%x very_high fever")));
		rule6.add(new ArrayList(Arrays.asList(("%x has whooping_cough"))));
		rules.add(rule6);		
		
		//If a patient has no fever, no cough, and no rash, the patient is healthy.
		rule7.add("7");
		rule7.add(new ArrayList(Arrays.asList("%x no_fever","%x no_cough","%x no_rash")));
		rule7.add(new ArrayList(Arrays.asList(("%x healthy"))));
		rules.add(rule7);		
		
		//If one patient has a particular disease which is contagious and that patient contacts another patient, then the other patient has the disease.
		rule8.add("8");
		rule8.add(new ArrayList(Arrays.asList("%x has %y","%y contagious","%x contacts %z")));
		rule8.add(new ArrayList(Arrays.asList(("%z has %y"))));
		rules.add(rule8);
		
		// If a doctor says that a patient has a particular disease or is healthy, then what the doctor says is true.
		rule9.add("9");
		rule9.add(new ArrayList(Arrays.asList("%x says %y %z","%x doctor")));
		rule9.add(new ArrayList(Arrays.asList(("%x true"))));
		rules.add(rule9);				
		
		rule10.add("10");
		rule10.add(new ArrayList(Arrays.asList("%x says %y healthy","%x doctor")));
		rule10.add(new ArrayList(Arrays.asList(("%x true"))));
		rules.add(rule10);				
		
		//If a person says that a patient has a particular disease or is healthy and that is not true of the patient, then that person is not a doctor.
		rule11.add("11");
		rule11.add(new ArrayList(Arrays.asList("%x says %y disease","%y no_disease")));
		rule11.add(new ArrayList(Arrays.asList(("%x not_doctor"))));
		rules.add(rule11);
		
		rule12.add("12");
		rule12.add(new ArrayList(Arrays.asList("%x says %y healthy","%y not_healthy")));
		rule12.add(new ArrayList(Arrays.asList(("%x not_doctor"))));
		rules.add(rule12);
		
		// Additional Rules 
/*		rule13.add("13");
		rule13.add(new ArrayList(Arrays.asList("%x disease","%y true")));
		rule13.add(new ArrayList(Arrays.asList(("%x not_doctor"))));
		rules.add(rule13);			*/
	}
}
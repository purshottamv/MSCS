import java.util.ArrayList;
import java.util.Arrays;

public class wm {

	ArrayList facts;
	wm()
	{
		facts = new ArrayList();
		ArrayList fact1 = new ArrayList();
		ArrayList fact2 = new ArrayList();
		ArrayList fact3 = new ArrayList();
		ArrayList fact4 = new ArrayList();
		ArrayList fact5 = new ArrayList();
		ArrayList fact6 = new ArrayList();
		ArrayList fact7 = new ArrayList();
		ArrayList fact8 = new ArrayList();
		ArrayList fact9 = new ArrayList();
		ArrayList fact10 = new ArrayList();
		
		fact1.add(new ArrayList(Arrays.asList("Ed very_high fever")));
		facts.add(fact1);
		
		fact2.add(new ArrayList(Arrays.asList(("Ed cough"))));
		facts.add(fact2);
		
		fact3.add(new ArrayList(Arrays.asList(("Alice no_poison_ivy"))));
		facts.add(fact3);
		
		fact4.add(new ArrayList(Arrays.asList("Max says Alice poison_ivy")));
		facts.add(fact4);
		
		fact5.add(new ArrayList(Arrays.asList("Grace says Don healthy")));
		facts.add(fact5);
		
		fact6.add(new ArrayList(Arrays.asList("Grace doctor")));
		facts.add(fact6);		
		
		fact7.add(new ArrayList(Arrays.asList("whooping_cough contagious")));
		facts.add(fact7);		
		
		fact8.add(new ArrayList(Arrays.asList("Ed contacts Alice")));
		facts.add(fact8);

		// Additional facts
/*		fact9.add(new ArrayList(Arrays.asList("Ed poison_ivy contagious")));
		facts.add(fact9);
		fact10.add(new ArrayList(Arrays.asList("Ed poison_ivy contagious")));
		facts.add(fact10);
*/
	}	
}
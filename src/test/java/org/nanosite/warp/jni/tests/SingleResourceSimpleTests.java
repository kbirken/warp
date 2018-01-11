package org.nanosite.warp.jni.tests;

import org.junit.Test;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpFunctionBlock;
import org.nanosite.warp.jni.WarpStep;

public class SingleResourceSimpleTests extends SingleResourceTestBase {

	@Test
	public void testResourceOneUserNoCPU() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		WarpStep s1 = addStep(bhvr1, "s1", 1000, 0);
		bhvr1.setInitial();
	
		warp.simulate("output/ResourceOneUserNoCPU_dot.txt");
		
		check(s1, 0, 0, 1000);
	}

	@Test
	public void testResourceOneUserWithCPU1() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		
		// less CPU load than resource load (never mind, both are working in parallel)
		WarpStep s1 = addStep(bhvr1, "s1", 500, 1000, 0);
		bhvr1.setInitial();
	
		warp.simulate("output/ResourceOneUserWithCPU1_dot.txt");
		
		check(s1, 0, 0, 1000);
	}

	@Test
	public void testResourceOneUserWithCPU2() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);

		// less resource load than CPU load (never mind, both are working in parallel)
		WarpStep s1 = addStep(bhvr1, "s1", 1000, 500, 0);
		bhvr1.setInitial();
	
		warp.simulate("output/ResourceOneUserWithCPU2_dot.txt");
		
		check(s1, 0, 0, 1000);
	}

	@Test
	public void testResourceTwoUsersNoCPU() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0, 0);

		// both behaviors are using the same amount of CPU
		// we have to take into account 10% CST penalty 
		WarpStep s1 = addStep(bhvr1, "s1", 500, 0);
		WarpStep s2 = addStep(bhvr2, "s2", 500, 0);
		bhvr1.setInitial();
		bhvr2.setInitial();
	
		warp.simulate("output/ResourceTwoUsersNoCPU_dot.txt");
		
		check(s1, 0, 0, 1100);
		check(s2, 0, 0, 1100);
	}


}

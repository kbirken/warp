package org.nanosite.warp.jni.tests;

import org.junit.Test;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpFunctionBlock;
import org.nanosite.warp.jni.WarpStep;

public class SingleCPUWaitTests extends WarpSingleCPUTestBase {

	@Test
	public void testSingleBehaviorWaiting() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		WarpStep s1 = addWaitStep(bhvr1, "s1", 2000, 0);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorWaiting_dot.txt");
		
		check(s1, 0, 0, 2000);
	}

	@Test
	public void testSingleBehaviorWaitLoad1() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		
		// CPU load << wait time: wait time dominates
		WarpStep s1 = addWaitStep(bhvr1, "s1", 2000, 100);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorWaitLoad1_dot.txt");
		
		check(s1, 0, 0, 2000);
	}

	@Test
	public void testSingleBehaviorWaitLoad2() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		
		// CPU load == wait time: wait time can be accomplished while computing
		WarpStep s1 = addWaitStep(bhvr1, "s1", 2000, 2000);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorWaitLoad2_dot.txt");
		
		check(s1, 0, 0, 2000);
	}
	
	@Test
	public void testTwoBehaviorsParallelWait1() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		
		// s1 gets only 50% CPU, therefore 1 sec wait time is not enough for completing this step
		WarpStep s1 = addWaitStep(bhvr1, "s1", 1000, 800);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0, 0);
		WarpStep s5 = addStep(bhvr2, "s5", 2000);
		bhvr2.setInitial();
		
		warp.simulate("output/TwoBehaviorsParallelWait1_dot.txt");
		
		check(s1, 0, 0, 1000+600);
		check(s5, 0, 0, 2*500+2*300+1200);
	}

	@Test
	public void testTwoBehaviorsParallelWait2() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		
		// although s1 only gets 50% CPU, 1 sec wait time is enough for completing this step
		WarpStep s1 = addWaitStep(bhvr1, "s1", 1000, 200);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0, 0);
		WarpStep s5 = addStep(bhvr2, "s5", 2000);
		bhvr2.setInitial();
		
		warp.simulate("output/TwoBehaviorsParallelWait2_dot.txt");
		
		check(s1, 0, 0, 1000);
		check(s5, 0, 0, 2*200+1800);
	}

}

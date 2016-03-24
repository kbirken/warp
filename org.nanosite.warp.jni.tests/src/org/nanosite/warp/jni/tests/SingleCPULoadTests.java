package org.nanosite.warp.jni.tests;

import org.junit.Test;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpFunctionBlock;
import org.nanosite.warp.jni.WarpStep;

public class SingleCPULoadTests extends WarpSingleCPUTestBase {

	@Test
	public void testSingleBehaviorOneStep() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = addStep(bhvr1, "s1", 3000);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorOneStep_dot.txt");
		
		check(s1, 0, 0, 3000);
	}

	@Test
	public void testSingleBehaviorTwoSteps() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = addStep(bhvr1, "s1", 2000);
		WarpStep s2 = addStep(bhvr1, "s2", 3000);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorTwoSteps_dot.txt");
		
		check(s1, 0, 0, 2000);
		check(s2, 2000, 2000, 5000);
	}
	
	@Test
	public void testTwoBehaviorsSequential() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = addStep(bhvr1, "s1", 1000);
		WarpStep s2 = addStep(bhvr1, "s2", 2000);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0);
		WarpStep s5 = addStep(bhvr2, "s5", 500);
		WarpStep s6 = addStep(bhvr2, "s6", 600);
	
		bhvr1.addSendTrigger(bhvr2);
		
		warp.simulate("output/TwoBehaviorsSequential_dot.txt");
		
		check(s1, 0, 0, 1000);
		check(s2, 1000, 1000, 3000);
		check(s5, 3000, 3000, 3500);
		check(s6, 3500, 3500, 4100);
	}

	
	@Test
	public void testTwoBehaviorsParallel() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = addStep(bhvr1, "s1", 1000);
		WarpStep s2 = addStep(bhvr1, "s2", 2000);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0);
		WarpStep s5 = addStep(bhvr2, "s5", 2000);
		bhvr2.setInitial();
		
		warp.simulate("output/TwoBehaviorsParallel_dot.txt");
		
		check(s1, 0, 0, 2000);
		check(s2, 2000, 2000, 5000);
		check(s5, 0, 0, 4000);
	}

	@Test
	public void testTwoBehaviorsParallelPrecondition() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = addStep(bhvr1, "s1", 1000);
		WarpStep s2 = addStep(bhvr1, "s2", 2000);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0);
		WarpStep s5 = addStep(bhvr2, "s5", 2000);
		bhvr2.setInitial();
		
		s5.addPrecondition(s2);
		
		warp.simulate("output/TwoBehaviorsParallelPrecondition_dot.txt");
		
		check(s1, 0, 0, 1000);
		check(s2, 1000, 1000, 3000);
		check(s5, 3000, 3000, 5000);  // TODO: why isn't ready time of s5 at 0? what's the difference of ready and running?
	}
	
}

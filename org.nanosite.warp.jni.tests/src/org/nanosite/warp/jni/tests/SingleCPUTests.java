package org.nanosite.warp.jni.tests;

import static org.junit.Assert.assertEquals;

import org.junit.BeforeClass;
import org.junit.Test;
import org.nanosite.warp.jni.Warp;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpFunctionBlock;
import org.nanosite.warp.jni.WarpStep;

public class SingleCPUTests {

	/**
	 * The timebase for warp is one microseconds, thus we define a millisec as 1000 microseconds.
	 */
	private final int MS = 1000;
	
	private final int verbose = 0;

	private final Warp warp = new Warp(verbose);
	
	@BeforeClass
	public static void setup() {
		Warp.initJNI();
	}

	@Test
	public void testSingleBehaviorOneStep() {
		warp.addResource("CPU1", 0);

		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = bhvr1.addStep("s1", 5000*MS);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorOneStep_dot.txt");
		
		check(s1, 0, 0, 5000);
	}

	@Test
	public void testSingleBehaviorTwoSteps() {
		warp.addResource("CPU1", 0);

		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = bhvr1.addStep("s1", 2000*MS);
		WarpStep s2 = bhvr1.addStep("s2", 3000*MS);
		bhvr1.setInitial();
	
		warp.simulate("output/SingleBehaviorTwoSteps_dot.txt");
		
		check(s1, 0, 0, 2000);
		check(s2, 2000, 2000, 5000);
	}
	
	@Test
	public void testTwoBehaviorsSequential() {
		warp.addResource("CPU1", 0);

		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = bhvr1.addStep("s1", 1000*MS);
		WarpStep s2 = bhvr1.addStep("s2", 2000*MS);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0);
		WarpStep s5 = bhvr2.addStep("s5", 500*MS);
		WarpStep s6 = bhvr2.addStep("s6", 600*MS);
	
		bhvr1.addSendTrigger(bhvr2);
		
		warp.simulate("output/TwoBehaviorsSequential_dot.txt");
		
		check(s1, 0, 0, 1000);
		check(s2, 1000, 1000, 3000);
		check(s5, 3000, 3000, 3500);
		check(s6, 3500, 3500, 4100);
	}

	
	@Test
	public void testTwoBehaviorsParallel() {
		warp.addResource("CPU1", 0);

		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = bhvr1.addStep("s1", 1000*MS);
		WarpStep s2 = bhvr1.addStep("s2", 2000*MS);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0);
		WarpStep s5 = bhvr2.addStep("s5", 2000*MS);
		bhvr2.setInitial();
		
		warp.simulate("output/TwoBehaviorsParallel_dot.txt");
		
		check(s1, 0, 0, 2000);
		check(s2, 2000, 2000, 5000);
		check(s5, 0, 0, 4000);
	}

	@Test
	public void testTwoBehaviorsParallelPrecondition() {
		warp.addResource("CPU1", 0);

		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0);
		WarpStep s1 = bhvr1.addStep("s1", 1000*MS);
		WarpStep s2 = bhvr1.addStep("s2", 2000*MS);
		bhvr1.setInitial();

		WarpBehavior bhvr2 = fb1.addBehavior("Bhvr2", 0);
		WarpStep s5 = bhvr2.addStep("s5", 2000*MS);
		bhvr2.setInitial();
		
		s5.addPrecondition(s2);
		
		warp.simulate("output/TwoBehaviorsParallelPrecondition_dot.txt");
		
		check(s1, 0, 0, 1000);
		check(s2, 1000, 1000, 3000);
		check(s5, 3000, 3000, 5000);  // TODO: why isn't ready time of s5 at 0? what's the difference of ready and running?
	}

	
	private void check(WarpStep step, int tReadyExpected, int tRunningExpected, int tDoneExpected) {
		assertEquals(tReadyExpected, step.getReady()/MS);
		assertEquals(tRunningExpected, step.getRunning()/MS);
		assertEquals(tDoneExpected, step.getDone()/MS);
	}
}

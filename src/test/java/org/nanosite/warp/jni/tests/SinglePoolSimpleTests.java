package org.nanosite.warp.jni.tests;

import java.util.ArrayList;
import java.util.List;
import org.junit.Test;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpFunctionBlock;
import org.nanosite.warp.jni.WarpStep;

public class SinglePoolSimpleTests extends WarpSingleCPUTestBase {

	@Test
	public void testPoolAlloc_Reject() {
		// add alloc/free pool with REJECT_AND_CONTINUE on overflow
		warp.addPool("Pool1", 1000, 0, 0);

		List<WarpStep> steps = threeStepsWithOverflowInSecondStep();
		warp.simulate("output/PoolAlloc1_dot.txt");
		
		check(steps.get(2), 20, 20, 30);

		checkPool(steps.get(0), 0, 100, false, false);
		checkPool(steps.get(1), 0, 100,  true, false);
		checkPool(steps.get(2), 0,  50,  true, false);
	}

	@Test
	public void testPoolAlloc_Execute() {
		// add alloc/free pool with EXECUTE_AND_CONTINUE on overflow
		warp.addPool("Pool1", 1000, 1, 0);

		List<WarpStep> steps = threeStepsWithOverflowInSecondStep();
		warp.simulate("output/PoolAlloc2_dot.txt");
		
		check(steps.get(2), 20, 20, 30);

		checkPool(steps.get(0), 0,  100, false, false);
		checkPool(steps.get(1), 0, 2100,  true, false);
		checkPool(steps.get(2), 0, 2050,  true, false);
	}

	@Test
	public void testPoolAlloc_Stop() {
		// add alloc/free pool with STOP_WORKING on overflow
		warp.addPool("Pool1", 1000, 2, 0);

		List<WarpStep> steps = threeStepsWithOverflowInSecondStep();
		warp.simulate("output/PoolAlloc3_dot.txt");
		
		check(steps.get(2), 20, 20, 30);

		checkPool(steps.get(0), 0,  100, false, false);
		checkPool(steps.get(1), 0,  100,  true, false);
		checkPool(steps.get(2), 0,  100,  true, false);
	}


	private List<WarpStep> threeStepsWithOverflowInSecondStep() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);

		List<WarpStep> steps = new ArrayList<WarpStep>();
		steps.add(addPoolStep(bhvr1, "s1", 100));
		steps.add(addPoolStep(bhvr1, "s2", 2000));
		steps.add(addPoolStep(bhvr1, "s3", -50));
		bhvr1.setInitial();

		return steps;
	}
}

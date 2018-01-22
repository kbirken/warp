package org.nanosite.warp.jni.tests;

import org.junit.Test;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpFunctionBlock;
import org.nanosite.warp.jni.WarpStep;

public class SinglePoolSimpleTests extends SinglePoolTestBase {

	@Test
	public void testPoolAlloc1() {
		WarpFunctionBlock fb1 = warp.addFunctionBlock("FB1", 1, 1);
		WarpBehavior bhvr1 = fb1.addBehavior("Bhvr1", 0, 0);
		WarpStep s1 = addPoolStep(bhvr1, "s1", 100);
		WarpStep s2 = addPoolStep(bhvr1, "s2", 2000);
		WarpStep s3 = addPoolStep(bhvr1, "s3", -50);
		bhvr1.setInitial();
	
		warp.simulate("output/PoolAlloc1_dot.txt");
		
		check(s3, 20, 20, 30);
	}


}

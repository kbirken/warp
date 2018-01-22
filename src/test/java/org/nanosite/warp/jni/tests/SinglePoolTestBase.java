package org.nanosite.warp.jni.tests;

import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpStep;

public class SinglePoolTestBase extends WarpSingleCPUTestBase {

	@Override
	protected void initializeHardware() {
		// we need one CPU
		warp.addCPU("CPU1", 0);
		
		// and an alloc/free pool
		warp.addPool("Pool1", 1000, 0, 0);
	}

}

package org.nanosite.warp.jni.tests;

import org.junit.Before;
import org.nanosite.warp.jni.Warp;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpStep;

public class WarpSingleCPUTestBase extends WarpTestBase {

	@Override
	protected void initializeHardware() {
		warp.addCPU("CPU1", 0);
	}
	
	
	protected WarpStep addStep(WarpBehavior bhvr, String name, int loadCPU1) {
		return addWaitStep(bhvr, name, 0, loadCPU1);
	}
	
	protected WarpStep addWaitStep(WarpBehavior bhvr, String name, int waitTime, int loadCPU1) {
		long[] loads = new long[2];
		long[] poolVals = new long[1];
		loads[0] = ((long)waitTime) * MS;
		loads[1] = ((long)loadCPU1) * MS;
		return bhvr.addStep(name, loads, poolVals);
	}

}

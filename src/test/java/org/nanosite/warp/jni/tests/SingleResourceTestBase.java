package org.nanosite.warp.jni.tests;

import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpStep;

public class SingleResourceTestBase extends WarpTestBase {

	final static int PERCENT = 10;
	
	@Override
	protected void initializeHardware() {
		// we need one CPU
		warp.addCPU("CPU1", 0);
		
		// and a bandwidth-limited CPU with two interfaces
		int[] cst = { 10*PERCENT, 20*PERCENT };
		warp.addResource("Res1", cst);
	}

	protected WarpStep addStep(WarpBehavior bhvr, String name, int loadRes1IF1, int loadRes1IF2) {
		return addWaitStep(bhvr, name, 0, 0, loadRes1IF1, loadRes1IF2);
	}
	
	protected WarpStep addStep(WarpBehavior bhvr, String name, int loadCPU1, int loadRes1IF1, int loadRes1IF2) {
		return addWaitStep(bhvr, name, 0, loadCPU1, loadRes1IF1, loadRes1IF2);
	}
	
	protected WarpStep addWaitStep(WarpBehavior bhvr, String name, int waitTime, int loadCPU1, int loadRes1IF1, int loadRes1IF2) {
		long[] loads = new long[4];
		long[] poolVals = new long[1];
		loads[0] = ((long)waitTime) * MS;
		loads[1] = ((long)loadCPU1) * MS;
		loads[2] = ((long)loadRes1IF1) * MS;
		loads[3] = ((long)loadRes1IF2) * MS;
		return bhvr.addStep(name, loads, poolVals);
	}

}

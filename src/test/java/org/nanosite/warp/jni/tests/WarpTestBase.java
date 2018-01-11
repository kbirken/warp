package org.nanosite.warp.jni.tests;

import static org.junit.Assert.assertEquals;

import org.junit.Before;
import org.junit.BeforeClass;
import org.nanosite.warp.jni.Warp;
import org.nanosite.warp.jni.WarpBehavior;
import org.nanosite.warp.jni.WarpStep;

abstract public class WarpTestBase {

	/**
	 * The timebase for warp is one microsecond, thus we define a millisec as 1000 microseconds.
	 */
	protected final int MS = 1000;

	/**
	 * The verbose level controlling console output of the warp simulator.
	 */
	private final int verbose = 5;
	
	/**
	 * The instance of the warp simulator under test. 
	 */
	protected Warp warp;

	@BeforeClass
	public static void setupClass() {
		Warp.initJNI();
	}

	@Before
	public void setup() {
		this.warp = new Warp(verbose);
		initializeHardware();
	}
	
	abstract protected void initializeHardware();
	
	protected void check(WarpStep step, int tReadyExpected, int tRunningExpected, int tDoneExpected) {
		assertEquals(tReadyExpected, step.getReady()/MS);
		assertEquals(tRunningExpected, step.getRunning()/MS);
		assertEquals(tDoneExpected, step.getDone()/MS);
	}
}

package org.nanosite.warp.jni;

public class WarpStep {
	final private Warp api;
	final private long handle;
	final private int id;
	
	private static int nextId = 0;
	
	public static void reset() {
		nextId = 0;
	}
	
	public WarpStep(Warp api, long handle) {
		this.api = api;
		this.handle = handle;
		this.id = nextId++;
	}

	public int getId() {
		return id;
	}

	public void addPrecondition(WarpStep precondition) {
		addPrecondition(handle, precondition.handle);
	}

	public long getReady() {
		return getReady(handle, api.getHandle());
	}

	public long getRunning() {
		return getRunning(handle, api.getHandle());
	}

	public long getDone() {
		return getDone(handle, api.getHandle());
	}
	
	public int getPoolUsage(int poolIndex) {
		return getPoolUsage(handle, poolIndex);
	}

	public boolean getPoolOverflow(int poolIndex) {
		return getPoolOverflow(handle, poolIndex);
	}

	public boolean getPoolUnderflow(int poolIndex) {
		return getPoolUnderflow(handle, poolIndex);
	}

	/**
	 * Get internal handle for this step.</p>
	 * 
	 *  This method should be used only internally (e.g., by WarpBehavior).
	 */
	public long getHandle() {
		return handle;
	}
	
	static private native void addPrecondition(long handle, long preconditionStepHandle);
	static private native long getReady(long handle, long simhandle);
	static private native long getRunning(long handle, long simhandle);
	static private native long getDone(long handle, long simhandle);
	static private native int getPoolUsage(long handle, int poolIndex);
	static private native boolean getPoolOverflow(long handle, int poolIndex);
	static private native boolean getPoolUnderflow(long handle, int poolIndex);

}

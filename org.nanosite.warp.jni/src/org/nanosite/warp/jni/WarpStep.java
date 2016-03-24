package org.nanosite.warp.jni;

public class WarpStep {
	final private Warp api;
	final private long handle;
	
	public WarpStep(Warp api, long handle) {
		this.api = api;
		this.handle = handle;
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

	static private native void addPrecondition(long handle, long preconditionStepHandle);
	static private native long getReady(long handle, long simhandle);
	static private native long getRunning(long handle, long simhandle);
	static private native long getDone(long handle, long simhandle);

}

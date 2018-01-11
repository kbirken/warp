package org.nanosite.warp.jni;

public class WarpBehavior {
	final private Warp api;
	final private long handle;
	
	public WarpBehavior(Warp api, long handle) {
		this.api = api;
		this.handle = handle;
	}
	
	public void setInitial() {
		setInitial(handle, api.getHandle());
	}

	public WarpStep addStep(String name, long[] loads) {
		long stepHandle = addStep(handle, api.getHandle(), name, loads);
		WarpStep step = new WarpStep(api, stepHandle);
		api.getSteps().addStep(step);
		return step;
	}
	
	public void addSendTrigger(WarpBehavior receiver) {
		addSendTrigger(handle, receiver.handle);
	}

	public void addRepeatUnless(WarpStep terminationStep) {
		addRepeatUnless(handle, terminationStep.getHandle());
	}

	static private native void setInitial(long handle, long simhandle);
	static private native long addStep(long handle, long simhandle, String name, long[] loads);
	static private native void addSendTrigger(long handle, long receiverHandle);
	static private native void addRepeatUnless(long handle, long unlessStepHandle);

}

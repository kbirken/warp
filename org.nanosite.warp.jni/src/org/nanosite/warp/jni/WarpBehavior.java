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

	public WarpStep addStep(String name, long use) {
		long step = addStep(handle, api.getHandle(), name, use);
		return new WarpStep(api, step);
	}
	
	public void addSendTrigger(WarpBehavior receiver) {
		addSendTrigger(handle, receiver.handle);
	}

	static private native void setInitial(long handle, long simhandle);
	static private native long addStep(long handle, long simhandle, String name, long use);
	static private native void addSendTrigger(long handle, long receiverHandle);

}

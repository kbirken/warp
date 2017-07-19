package org.nanosite.warp.jni;

public class WarpFunctionBlock {
	final private Warp api;
	final private long handle;
	
	public WarpFunctionBlock(Warp api, long handle) {
		this.api = api;
		this.handle = handle;
	}
	
	public WarpBehavior addBehavior(String name, int loopType, int loopParam) {
		long bhvr = addBehavior(handle, api.getHandle(), name, loopType, loopParam);
		return new WarpBehavior(api, bhvr);
	}

	static private native long addBehavior(
		long handle,
		long warpHandle,
		String name,
		int loopType,
		int loopParam
	);

}

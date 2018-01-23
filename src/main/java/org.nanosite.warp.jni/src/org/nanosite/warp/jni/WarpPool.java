package org.nanosite.warp.jni;

public class WarpPool {
	final private Warp api;
	final private long handle;
	
	public WarpPool(Warp api, long handle) {
		this.api = api;
		this.handle = handle;
	}

	public long getAllocated() {
		return getAllocated(handle);
	}

	public int getNOverflows() {
		return getNOverflows(handle);
	}

	public int getNUnderflows() {
		return getNUnderflows(handle);
	}

	/**
	 * Get internal handle for this step.</p>
	 * 
	 *  This method should be used only internally.
	 */
	public long getHandle() {
		return handle;
	}
	
	static private native long getAllocated(long handle);
	static private native int getNOverflows(long handle);
	static private native int getNUnderflows(long handle);

}

package org.nanosite.warp.jni;

public class Warp {

	/**
	 * Initialize native object which represents the simulation.
	 * 
	 * @return a handle for the native object
	 */
	private native long createSimulation(int verbose);
	
	/**
	 * Add a resource to the simulation model.
	 * 
	 * @param simhandle the native object handle 
	 * @param name
	 */
	private native void addResource(long simhandle, String name, int scheduling);

	/**
	 * Add a function block to the simulation model.
	 * 
	 * @param simhandle the native object handle 
	 * @param name the name of the function block

	 * @return a handle for the native function block object
	 */
	private native long addFunctionBlock(long simhandle, String name, int cpu, int partition);

	
	/**
	 * Start simulation.
	 * 
	 * @param simhandle the native object handle 
	 */
	private native void simulate(long simhandle, String dotfile);


	final private long simhandle;
	
	/**
	 * Load the JNI library.
	 * 
	 * This has to be done explicitly because some clients want to do this differently
	 * (i.e., without using this method).
	 */
	public static void initJNI() { 
		System.loadLibrary("warp_jni");
	}
	
	public Warp(int verbose) {
		simhandle = createSimulation(verbose);
	}
	
	public long getHandle() {
		return simhandle;
	}

	public void addResource(String name, int scheduling) {
		addResource(simhandle, name, scheduling);
	}

	public WarpFunctionBlock addFunctionBlock(String name, int cpu, int partition) {
		long handle = addFunctionBlock(simhandle, name, cpu, partition);
		return new WarpFunctionBlock(this, handle);
	}

	public void simulate(String dotfile) {
		simulate(simhandle, dotfile);
	}
}

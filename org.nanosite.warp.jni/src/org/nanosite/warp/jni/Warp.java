package org.nanosite.warp.jni;

import org.nanosite.warp.jni.results.SimulationResult;

public class Warp {

	/**
	 * Initialize native object which represents the simulation.
	 * 
	 * @return a handle for the native object
	 */
	private native long createSimulation(int verbose);
	
	/**
	 * Add a CPU to the simulation model.
	 * 
	 * @param simhandle the native object handle 
	 * @param name the name of the CPU
	 * @param scheduling the type of scheduling
	 */
	private native void addCPU(long simhandle, String name, int scheduling);

	/**
	 * Add a bandwidth-limited resource to the simulation model.
	 * 
	 * @param simhandle the native object handle 
	 * @param name the name of the resource
	 * @param cst the array of context switching times for the resource's interfaces
	 */
	private native void addResource(long simhandle, String name, int[] cst);

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

	/**
	 * Write detail result file.
	 * 
	 * @param simhandle the native object handle 
	 */
	private native void writeResultFile(long simhandle, String outfile);

	/**
	 * Get number of iterations of last simulation.
	 * 
	 * @param simhandle the native object handle 
	 * @return number of iterations.
	 */
	private native int getNIterations(long simhandle);

	/**
	 * Get number of remaining behaviors of last simulation.</p>
	 * 
	 * This should be zero if the simulation was successful.
	 * 
	 * @param simhandle the native object handle 
	 * @return number of behaviors which have not been executed completely at least once.
	 */
	private native int getNRemainingBehaviors(long simhandle);
	

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
	
	private StepDirectory steps = new StepDirectory();
	
	public Warp(int verbose) {
		simhandle = createSimulation(verbose);

		// reset global id counter for WarpStep objects
		WarpStep.reset();
	}
	
	public long getHandle() {
		return simhandle;
	}

	public StepDirectory getSteps() {
		return this.steps;
	}
	
	public void addCPU(String name, int scheduling) {
		addCPU(simhandle, name, scheduling);
	}

	public void addResource(String name, int[] cst) {
		addResource(simhandle, name, cst);
	}

	public WarpFunctionBlock addFunctionBlock(String name, int cpu, int partition) {
		long handle = addFunctionBlock(simhandle, name, cpu, partition);
		return new WarpFunctionBlock(this, handle);
	}

	public void simulate(String dotfile) {
		simulate(simhandle, dotfile);
	}
	
	public int getNIterations() {
		return getNIterations(simhandle);
	}
	
	public int getNRemainingBehaviors() {
		return getNRemainingBehaviors(simhandle);
	}
	
	public SimulationResult getSimResult(String tempfile) {
		writeResultFile(simhandle, tempfile);
		SimulationResult res = new SimulationResult();
		res.readFromFile(tempfile, steps);
		return res;
	}
}

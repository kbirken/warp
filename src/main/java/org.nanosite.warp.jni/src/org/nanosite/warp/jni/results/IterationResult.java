package org.nanosite.warp.jni.results;

import java.util.ArrayList;
import java.util.List;

import org.nanosite.warp.jni.WarpStep;

public class IterationResult {
	
	private int dt = 0;
	private List<List<WarpStep>> stepsPerResource = new ArrayList<List<WarpStep>>();
	
	public void setDT(int dt) {
		this.dt = dt;
	}
	
	public void setResourceUsage(List<List<WarpStep>> resUsage) {
		this.stepsPerResource = resUsage;
	}
	
	public int getDT() {
		return this.dt;
	}
	
	public List<List<WarpStep>> getResourceUsage() {
		return this.stepsPerResource;
	}
}

package org.nanosite.warp.jni;

import java.util.HashMap;
import java.util.Map;

public class StepDirectory {

	private Map<Integer, WarpStep> id2step = new HashMap<Integer, WarpStep>();
	
	public void addStep(WarpStep step) {
		id2step.put(step.getId(), step);
	}
	
	public WarpStep getStep(int id) {
		return id2step.get(id);
	}
}

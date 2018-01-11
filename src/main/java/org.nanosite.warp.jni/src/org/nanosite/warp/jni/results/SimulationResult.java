package org.nanosite.warp.jni.results;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.nanosite.warp.jni.StepDirectory;
import org.nanosite.warp.jni.WarpStep;

public class SimulationResult {

	private static final String TAG_PREAMBLE = "WARP SIM RESULT VERSION";
	private static final String TAG_ITERATION = "I";
	private static final String TAG_DELTA_T = "DT";
	private static final String TAG_RES_USAGE = "RU";
	private static final String TAG_STEP_PER_RES = "ST";
	
	private List<IterationResult> iterations = new ArrayList<IterationResult>();

	public Collection<IterationResult> getIterationResults() {
		return this.iterations;
	}
	
	private int parseState = 0;
	private IterationResult currentIteration = null;
	private StepDirectory stepDirectory = null;
	
	public boolean readFromFile(String resfile, StepDirectory steps) {
		this.iterations.clear();
		this.parseState = 0;
		this.stepDirectory = steps;

		try {
			BufferedReader reader = new BufferedReader(new FileReader(resfile));
			try {
				int lineno = 1;
				String line = reader.readLine();
				while (line!=null) {
					if (! parse(line)) {
						System.err.println("ERROR: Invalid content while reading result file '" + resfile + "' in line " + lineno + ".");
						return false;
					};

					line = reader.readLine();
					lineno++;
				}
				
				// add final iteration (if any)
				if (currentIteration!=null)
					iterations.add(currentIteration);
				
			} catch (IOException e) {
				System.err.println("ERROR: Cannot read from result file '" + resfile + "' (" + e.getMessage() + ")");
				return false;
			} finally {
				try {
					reader.close();
				} catch (IOException e) {
					System.err.println("ERROR: Cannot close result file '" + resfile + "' (" + e.getMessage() + ")");
					return false;
				}
			}
		} catch (FileNotFoundException e) {
			System.err.println("ERROR: Result file '" + resfile + "' not found (" + e.getMessage() + ")");
			return false;
		}
		return true;
	}
	
	private boolean parse(String line) {
		switch (parseState) {
		case 0:
			// expecting preamble
			if (line.startsWith(TAG_PREAMBLE)) {
				// TODO: check version number
				parseState = 1;
				return true;
			} else {
				return false;
			}
		case 1:
			// expecting start of iteration
			if (line.startsWith(TAG_ITERATION)) {
				int nIter = Integer.valueOf(values(line, TAG_ITERATION));

				// save current iteration
				if (currentIteration!=null) {
					iterations.add(currentIteration);
				}
				
				// create next iteration
				currentIteration = new IterationResult();
					
				parseState = 2;
				return true;
			} else {
				return false;
			}
		case 2:
			// expecting data for iteration
			if (line.startsWith(TAG_ITERATION)) {
				parseState = 1;
				return parse(line);
			} else if (line.startsWith(TAG_DELTA_T)) {
				int dt = Integer.valueOf(values(line, TAG_DELTA_T));
				currentIteration.setDT(dt);
				return true;
			} else if (line.startsWith(TAG_RES_USAGE)) {
				// ignore this line, was replaced by TAG_STEP_PER_RES
				return true;
			} else if (line.startsWith(TAG_STEP_PER_RES)) {
				String data = values(line, TAG_STEP_PER_RES);
				String resourceLoads[] = data.split(";");
				List<List<WarpStep>> stepsPerResource = new ArrayList<List<WarpStep>>();
				
				// for each resource, get the behavior steps using it
				for(String rl : resourceLoads) {
					List<WarpStep> stepsUsing = new ArrayList<WarpStep>();
					String steps[] = rl.trim().split(" ");
					for(String s : steps) {
						if (! s.trim().isEmpty()) {
							int stepId = Integer.valueOf(s.trim());
							WarpStep step = stepDirectory.getStep(stepId);
							if (step==null) {
								System.err.println("ERROR: Cannot find WarpStep with id " + stepId + "!");
								return false;
							}
							stepsUsing.add(step);
						}
					}
					stepsPerResource.add(stepsUsing);
				}
				currentIteration.setResourceUsage(stepsPerResource);
				return true;
			} else {
				return false;
			}
		default:
			return false;
		}
	}

	private String values(String line, String tag) {
		return line.substring(tag.length()+1).trim();
	}
}

/*
 * warp_jni.cpp
 *
 * Implementation of the JNI API for warp.
 *
 * Author:  Klaus Birken (itemis AG)
 * Created: 2016-03-12
 */

#include <jni.h>
#include <unistd.h> // for getcwd()

#include <iostream>

#include "model/CSimModel.h"
#include "model/Resource.h"
#include "model/CFunctionBlock.h"
#include "model/CBehavior.h"
#include "model/Pool.h"
#include "sim/PoolSim.h"
#include "simulation/CSimCore.h"
#include "simulation/CIntAccuracy.h"

#include "org_nanosite_warp_jni_Warp.h"
#include "org_nanosite_warp_jni_WarpFunctionBlock.h"
#include "org_nanosite_warp_jni_WarpBehavior.h"
#include "org_nanosite_warp_jni_WarpStep.h"


using namespace std;
using namespace warp::model;

class WarpJNI {
public:
	WarpJNI(int verbose);
	virtual ~WarpJNI();

	void addResource(const char* name, vector<int> cst, int scheduling);
	void addPool(const char* name, int maxAmount, int onOverflow, int onUnderflow);
	warp::CFunctionBlock* addFunctionBlock(const char* name, int cpu, int partition);
	warp::CBehavior* addBehavior(warp::CFunctionBlock* fb, const char* name, int loopType, int loopParam);
	warp::CStep* addStep(warp::CBehavior* bhvr, const char* name, vector<long> loads, vector<long> poolVals);
	void addInitial(warp::CBehavior* bhvr);

	void simulate(const char* dotfile);
	void writeResultFile(const char* outfile);

	int getNIterations();
	int getNRemnants();

	warp::CSimulatorCore* getSimulator() {
		return _simulator;
	}

private:
	int _verbose;
	int _stepId;
	bool _phase2started;
	CSimModel _model;

	warp::CBehavior* _bhvrLast;
	warp::CStep* _previousStep;

	warp::CSimulatorCore* _simulator;
};

WarpJNI::WarpJNI(int verbose) :
	_verbose(verbose),
	_stepId(0),
	_phase2started(false),
	_bhvrLast(NULL),
	_previousStep(NULL),
	_simulator(NULL)
{
	_model.init();
}

WarpJNI::~WarpJNI() {
	delete _simulator;
}

void WarpJNI::addResource(const char* name, vector<int> cst, int scheduling) {
//    cout << "WarpJNI::addResource(" << name << ")\n";
    shared_ptr<Resource> res(new Resource(name, cst, (Resource::Scheduling)scheduling));
    _model.addResource(res);
}

void WarpJNI::addPool(const char* name, int maxAmount, int onOverflow, int onUnderflow) {
//    cout << "WarpJNI::addPool(Pool << name << ")\n";
    Pool* pool = new Pool(name, maxAmount,
    		static_cast<Pool::ErrorAction>(onOverflow),
			static_cast<Pool::ErrorAction>(onUnderflow));
    _model.addPool(pool);
}

warp::CFunctionBlock* WarpJNI::addFunctionBlock(const char* name, int cpu, int partition) {
//    cout << "WarpJNI::addFunctionBlock(" << name << ", " << cpu << ")\n";

    if (!_phase2started) {
    	_phase2started = true;
    	_model.phase2();
    }

    // partition counting in input file starts with 1, we start with 0 internally
	warp::CFunctionBlock* fb = new warp::CFunctionBlock(name, cpu, partition-1);
	_model.addFunctionBlock(fb);
	return fb;
}

warp::CBehavior* WarpJNI::addBehavior(warp::CFunctionBlock* fb, const char* name, int loopType, int loopParam) {
//    cout << "WarpJNI::addBehavior(" << fb->getName() << "::" << name << ", " << type << ")\n";
	warp::CBehavior* bhvr = new warp::CBehavior(*fb, name, loopType, loopParam, false);
	fb->addBehavior(bhvr);
	return bhvr;
}

warp::CStep* WarpJNI::addStep(warp::CBehavior* bhvr, const char* name, vector<long> loads, vector<long> poolVals) {
//    cout << "WarpJNI::addStep(" << bhvr->getQualifiedName() << ", " << name << ")\n";

	// create CStep and corresponding CResourceVector
	vector<int> values;
	vector<int> averageCSTs;
	int loadsIdx = 0;
	for(int r=0; r<_model.getNResources(); r++) {
		const shared_ptr<Resource> res = _model.getResources()[r];
		//printf("  reading resource %d / %s\n", r, res.getName());

		int rv=0, cst=0;
		int n = res->getNSlots();
		int nRIs = res->getNInterfaces();
		//printf("    n=%d nRIs=%d\n", n, nRIs);
		for(int ri=0; ri<n; ri++) {
			int v = loads[loadsIdx++];

			rv += v;
//			printf("    ri=%d rv=%d v=%d\n", ri, rv, v);
			if (nRIs>0) {
				int cstRI = v * res->getCST(ri);
//				printf("     res.getCST(ri)=%d cstRI=%d\n", res->getCST(ri), cstRI);
				cst += cstRI;
//				printf("     cst=%d\n", cst);
//				if (cst>0) {
//					printf("  r=%d ri=%d n=%d v=%d rv=%d getCST=%d cst=%d\n",
//							r, ri, n, v, rv, res->getCST(ri), cst);
//				}
				// check for overflow
				if (cstRI<0 || cst<0) {
					cerr << "overflow error while computing average CST";
					//return false;
				}
			}
		}
		values.push_back(CIntAccuracy::toCalc(rv));

		// CST is averaged across all IOActivities on one resource
		int averageCST = rv>0 ? (cst / rv) : 0;
		/*
		if (averageCST>0) {
			printf("averageCST(%s):\t%d\n", _resources[r]->getName(), averageCST);
		}
		*/
		averageCSTs.push_back(averageCST);
	}

	warp::sim::PoolSimVector::Values poolValsVector;
	int np = _model.getPools().getNPools();
	if (np > poolVals.size()) {
		cerr << "Not enough pool values for " << np << " pools in step " << name << "!";
	} else {
		for(int i=0; i<np; i++) {
			// TODO: here we implicitly convert long values to int
			poolValsVector.push_back(poolVals[i]);
		}
	}

	const warp::CFunctionBlock& fb = bhvr->getFunctionBlock();
	warp::CStep* step =
			new warp::CStep(_stepId++, *bhvr, name, 0, fb.getCPU(), fb.getPartition(),
					new CResourceVector(values),
					new CResourceVector(averageCSTs),
					poolValsVector
			);
	if (_verbose>1) {
		step->print();
	}
	_model.addStep(step);
	bhvr->addStep(step);

	// connect with previous step (if same FB and behavior)
	if (_previousStep!=NULL) {
		if (bhvr==_bhvrLast) {
			_previousStep->addSuccessor(step);
		} else {
			_previousStep->setIsLast();
			step->setIsFirst();
		}
	}

	_previousStep = step;
	_bhvrLast = bhvr;
	return step;
}

void WarpJNI::addInitial(warp::CBehavior* bhvr) {
	_model.addInitial(bhvr);
}


void WarpJNI::simulate(const char* dotfile) {
	cout << "WarpJNI::simulate()\n";

	// do final initialization steps
	if (_previousStep!=NULL) {
		_previousStep->setIsLast();
	}
	_model.finalize();

	_simulator = new warp::CSimulatorCore(_verbose);

	// inject initial steps
	int verbose = 3;
	_model.addInitials(*_simulator, *_simulator);
	_simulator->print();

	// run simulation
	bool withLoadfile = (verbose>2);
	_simulator->run(
			_model.getResources(),
			_model.getResourceSlots(),
			_model.getPools(),
			withLoadfile,
			dotfile);

	int remnants = _model.checkRemnants();
	if (remnants > 0) {
		cout << "WarpJNI::simulate: Simulation could not be finished (" << remnants << " remnants).\n";
	} else {
		cout << "WarpJNI::simulate: Simulation finished.\n";
	}
}

void WarpJNI::writeResultFile(const char* outfile) {
	const int CWDMAX = 256;
	char cwd[CWDMAX];
	getcwd(cwd, CWDMAX-1);
	cout << "CWD: " << cwd << "\n";

	if (_simulator!=NULL) {
		cout << "WarpJNI::simulate: Writing result file " << outfile << ".\n";
		_simulator->writeDetailedResults(outfile);
	}
}

int WarpJNI::getNIterations() {
	return _simulator!=NULL ? _simulator->getNIterations() : 0;
}

int WarpJNI::getNRemnants() {
	return _model.checkRemnants();
}


JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_Warp_createSimulation(JNIEnv * env, jobject obj, jint verbose) {
    WarpJNI* warp = new WarpJNI(verbose);
    return (jlong)warp;
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_Warp_addCPU(JNIEnv *env, jobject obj, jlong handle, jstring name, jint scheduling) {
	WarpJNI* warp = (WarpJNI*)handle;

	const char *str= env->GetStringUTFChars(name, 0);
	vector<int> cst; // not used for CPU resources
	warp->addResource(str, cst, 0);

    env->ReleaseStringUTFChars(name, str);
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_Warp_addResource(JNIEnv *env, jobject obj, jlong handle, jstring name, jintArray cst) {
	WarpJNI* warp = (WarpJNI*)handle;

	const char *str= env->GetStringUTFChars(name, 0);
	int n = env->GetArrayLength(cst);
	jint* cstArray = env->GetIntArrayElements(cst, 0);
	vector<int> cstData;
	for(int i=0; i<n; i++)
		cstData.push_back(cstArray[i]);
	warp->addResource(str, cstData, 0);

    env->ReleaseStringUTFChars(name, str);
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_Warp_addPool(
	JNIEnv *env, jobject obj, jlong handle,
	jstring name,
	jint maxAmount,
	jint onOverflow,
	jint onUnderflow
) {
	WarpJNI* warp = (WarpJNI*)handle;

	const char *str= env->GetStringUTFChars(name, 0);
	warp->addPool(str, maxAmount, onOverflow, onUnderflow);

    env->ReleaseStringUTFChars(name, str);
}


JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_Warp_addFunctionBlock(JNIEnv *env, jobject, jlong handle, jstring name, jint cpu, jint partition) {
	const char *str= env->GetStringUTFChars(name, 0);

	WarpJNI* warp = (WarpJNI*)handle;
	warp::CFunctionBlock* fb = warp->addFunctionBlock(str, cpu, partition);

    env->ReleaseStringUTFChars(name, str);
    return (jlong)fb;
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_Warp_simulate(JNIEnv *env, jobject obj, jlong handle, jstring dotfile) {
	const char *dotfileStr = env->GetStringUTFChars(dotfile, 0);

	WarpJNI* warp = (WarpJNI*)handle;
	warp->simulate(dotfileStr);

	env->ReleaseStringUTFChars(dotfile, dotfileStr);
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_Warp_writeResultFile(JNIEnv *env, jobject obj, jlong handle, jstring resultfile) {
	const char *resultfileStr = env->GetStringUTFChars(resultfile, 0);

	WarpJNI* warp = (WarpJNI*)handle;
	warp->writeResultFile(resultfileStr);

	env->ReleaseStringUTFChars(resultfile, resultfileStr);
}


JNIEXPORT jint JNICALL Java_org_nanosite_warp_jni_Warp_getNIterations(JNIEnv *, jobject, jlong handle) {
	WarpJNI* warp = (WarpJNI*)handle;
	return warp->getNIterations();
}

JNIEXPORT jint JNICALL Java_org_nanosite_warp_jni_Warp_getNRemainingBehaviors(JNIEnv *, jobject, jlong handle) {
	WarpJNI* warp = (WarpJNI*)handle;
	return warp->getNRemnants();
}


/* ******************** JNI API for FunctionBlock ******************** */

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpFunctionBlock_addBehavior(
	JNIEnv *env, jclass,
	jlong handle,
	jlong warpHandle,
	jstring name,
	jint loopType,
	jint loopParam
) {
	warp::CFunctionBlock* fb = (warp::CFunctionBlock*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	const char *str= env->GetStringUTFChars(name, 0);
	warp::CBehavior* bhvr = warp->addBehavior(fb, str, loopType, loopParam);

    env->ReleaseStringUTFChars(name, str);
    return (jlong)bhvr;
}


/* ******************** JNI API for Behavior ******************** */

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_WarpBehavior_setInitial(JNIEnv *, jclass, jlong handle, jlong warpHandle) {
	warp::CBehavior* bhvr = (warp::CBehavior*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;
	warp->addInitial(bhvr);
}

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpBehavior_addStep(
		JNIEnv *env, jclass, jlong handle,
		jlong warpHandle,
		jstring name,
		jlongArray loads,
		jlongArray poolVals
) {
	warp::CBehavior* bhvr = (warp::CBehavior*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	const char *str= env->GetStringUTFChars(name, 0);

	int n = env->GetArrayLength(loads);
	jlong* loadsArray = env->GetLongArrayElements(loads, 0);
	vector<long> loadsData;
	for(int i=0; i<n; i++)
		loadsData.push_back(loadsArray[i]);

	int np = env->GetArrayLength(poolVals);
	jlong* poolValsArray = env->GetLongArrayElements(poolVals, 0);
	vector<long> poolValsData;
	for(int i=0; i<np; i++)
		poolValsData.push_back(poolValsArray[i]);

	warp::CStep* step = warp->addStep(bhvr, str, loadsData, poolValsData);

    env->ReleaseStringUTFChars(name, str);
    return (jlong)step;
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_WarpBehavior_addSendTrigger(JNIEnv *env, jclass, jlong handle, jlong receiverHandle) {
	warp::CBehavior* sender = (warp::CBehavior*)handle;
	warp::CBehavior* receiver = (warp::CBehavior*)receiverHandle;
	sender->addSendTrigger(receiver);
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_WarpBehavior_addRepeatUnless(
	JNIEnv *,
	jclass,
	jlong handle,
	jlong unlessStepHandle
) {
	warp::CBehavior* bhvr = (warp::CBehavior*)handle;
	warp::CStep* unlessEvent = (warp::CStep*)unlessStepHandle;
	bhvr->setUnlessCondition(unlessEvent);
}


/* ******************** JNI API for Step ******************** */

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_WarpStep_addPrecondition(JNIEnv *, jclass, jlong handle, jlong preconditionStepHandle) {
	warp::CStep* step = (warp::CStep*)handle;
	warp::CStep* precondition = (warp::CStep*)preconditionStepHandle;
	precondition->addSuccessor(step);
}

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpStep_getReady(JNIEnv *, jclass, jlong handle, jlong warpHandle) {
	warp::CStep* step = (warp::CStep*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	warp::CSimulatorCore* sim = warp->getSimulator();
	if (sim==NULL) {
		return 0;
	} else {
		return sim->getTimeReady(step);
	}
}

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpStep_getRunning(JNIEnv *, jclass, jlong handle, jlong warpHandle) {
	warp::CStep* step = (warp::CStep*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	warp::CSimulatorCore* sim = warp->getSimulator();
	if (sim==NULL) {
		return 0;
	} else {
		return sim->getTimeRunning(step);
	}
}

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpStep_getDone(JNIEnv *, jclass, jlong handle, jlong warpHandle) {
	warp::CStep* step = (warp::CStep*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	warp::CSimulatorCore* sim = warp->getSimulator();
	if (sim==NULL) {
		return 0;
	} else {
		return sim->getTimeDone(step);
	}
}

JNIEXPORT jint JNICALL Java_org_nanosite_warp_jni_WarpStep_getPoolUsage(JNIEnv *, jclass, jlong handle, jint poolIndex) {
	warp::CStep* step = (warp::CStep*)handle;
	return step->getPoolUsage(poolIndex);
}

JNIEXPORT jboolean JNICALL Java_org_nanosite_warp_jni_WarpStep_getPoolOverflow(JNIEnv *, jclass, jlong handle, jint poolIndex) {
	warp::CStep* step = (warp::CStep*)handle;
	return step->getPoolOverflow(poolIndex);
}

JNIEXPORT jboolean JNICALL Java_org_nanosite_warp_jni_WarpStep_getPoolUnderflow(JNIEnv *, jclass, jlong handle, jint poolIndex) {
	warp::CStep* step = (warp::CStep*)handle;
	return step->getPoolUnderflow(poolIndex);
}



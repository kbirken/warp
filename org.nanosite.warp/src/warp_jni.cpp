/*
 * warp_jni.cpp
 *
 * Implementation of the JNI API for warp.
 *
 * Author:  Klaus Birken (itemis AG)
 * Created: 2016-03-12
 */

#include <jni.h>

#include <iostream>

#include "model/CSimModel.h"
#include "model/Resource.h"
#include "model/CFunctionBlock.h"
#include "model/CBehavior.h"
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
	warp::CFunctionBlock* addFunctionBlock(const char* name, int cpu, int partition);
	warp::CBehavior* addBehavior(warp::CFunctionBlock* fb, const char* name, int type);
	warp::CStep* addStep(warp::CBehavior* bhvr, const char* name, vector<long> loads);
	void addInitial(warp::CBehavior* bhvr);

	void simulate(const char* dotfile);

	warp::CSimulatorCore* getSimulator() {
		return _simulator;
	}

private:
	int _verbose;
	bool _phase2started;
	CSimModel _model;

	warp::CBehavior* _bhvrLast;
	warp::CStep* _previousStep;

	warp::CSimulatorCore* _simulator;
};

WarpJNI::WarpJNI(int verbose) :
	_verbose(verbose),
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
    cout << "WarpJNI::addResource(" << name << ")\n";
    shared_ptr<Resource> res(new Resource(name, cst, (Resource::Scheduling)scheduling));
    _model.addResource(res);
}

warp::CFunctionBlock* WarpJNI::addFunctionBlock(const char* name, int cpu, int partition) {
    cout << "WarpJNI::addFunctionBlock(" << name << ", " << cpu << ")\n";

    if (!_phase2started) {
    	_phase2started = true;
    	_model.phase2();
    }

    // partition counting in input file starts with 1, we start with 0 internally
	warp::CFunctionBlock* fb = new warp::CFunctionBlock(name, cpu, partition-1);
	_model.addFunctionBlock(fb);
	return fb;
}

warp::CBehavior* WarpJNI::addBehavior(warp::CFunctionBlock* fb, const char* name, int type) {
    cout << "WarpJNI::addBehavior(" << fb->getName() << "::" << name << ", " << type << ")\n";
	warp::CBehavior* bhvr = new warp::CBehavior(*fb, name, type, 0, false);
	fb->addBehavior(bhvr);
	return bhvr;
}

warp::CStep* WarpJNI::addStep(warp::CBehavior* bhvr, const char* name, vector<long> loads) {
    cout << "WarpJNI::addStep(" << bhvr->getQualifiedName() << ", " << name << ")\n";

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
			printf("    ri=%d rv=%d v=%d\n", ri, rv, v);
			if (nRIs>0) {
				int cstRI = v * res->getCST(ri);
				printf("     res.getCST(ri)=%d cstRI=%d\n", res->getCST(ri), cstRI);
				cst += cstRI;
				printf("     cst=%d\n", cst);
				if (cst>0) {
					printf("  r=%d ri=%d n=%d v=%d rv=%d getCST=%d cst=%d\n",
							r, ri, n, v, rv, res->getCST(ri), cst);
				}
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
	warp::sim::PoolSimVector::Values poolVals; // TODO
	const warp::CFunctionBlock& fb = bhvr->getFunctionBlock();
	warp::CStep* step =
			new warp::CStep(1, *bhvr, name, 0, fb.getCPU(), fb.getPartition(),
					new CResourceVector(values),
					new CResourceVector(averageCSTs),
					poolVals
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


/* ******************** JNI API for FunctionBlock ******************** */

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpFunctionBlock_addBehavior(JNIEnv *env, jclass, jlong handle, jlong warpHandle, jstring name, jint type) {
	warp::CFunctionBlock* fb = (warp::CFunctionBlock*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	const char *str= env->GetStringUTFChars(name, 0);
	warp::CBehavior* bhvr = warp->addBehavior(fb, str, type);

    env->ReleaseStringUTFChars(name, str);
    return (jlong)bhvr;
}


/* ******************** JNI API for Behavior ******************** */

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_WarpBehavior_setInitial(JNIEnv *, jclass, jlong handle, jlong warpHandle) {
	warp::CBehavior* bhvr = (warp::CBehavior*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;
	warp->addInitial(bhvr);
}

JNIEXPORT jlong JNICALL Java_org_nanosite_warp_jni_WarpBehavior_addStep(JNIEnv *env, jclass, jlong handle, jlong warpHandle, jstring name, jlongArray loads) {
	warp::CBehavior* bhvr = (warp::CBehavior*)handle;
	WarpJNI* warp = (WarpJNI*)warpHandle;

	const char *str= env->GetStringUTFChars(name, 0);
	int n = env->GetArrayLength(loads);
	jlong* loadsArray = env->GetLongArrayElements(loads, 0);
	vector<long> loadsData;
	for(int i=0; i<n; i++)
		loadsData.push_back(loadsArray[i]);
	warp::CStep* step = warp->addStep(bhvr, str, loadsData);

    env->ReleaseStringUTFChars(name, str);
    return (jlong)step;
}

JNIEXPORT void JNICALL Java_org_nanosite_warp_jni_WarpBehavior_addSendTrigger(JNIEnv *env, jclass, jlong handle, jlong receiverHandle) {
	warp::CBehavior* sender = (warp::CBehavior*)handle;
	warp::CBehavior* receiver = (warp::CBehavior*)receiverHandle;
	sender->addSendTrigger(receiver);
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


// warp.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <unistd.h> // for getopt()

#include <string>
using namespace std;


#include "model/CSimModel.h"
#include "simulation/CSimCore.h"
#include "simulation/CTokenFactory.h"
#include "simulation/CTimeMonitor.h"


static void printUsage (void)
{
	fprintf(stderr, "USAGE: warp.exe [-v <verbose_level>] [-g <dot_file>] <warp_file>\n");
}


int main(int argc, char* argv[])
{
	CTimeMonitor mon;

	// options and parameters
	int verbose = 0;
	string dotFile = "";
	string warpFile = "";

	int c;
	int errflg = 0;
	extern char *optarg;
	extern int optind, optopt;
	while ((c = getopt(argc, argv, "v:g:")) != -1) {
		switch (c) {
		case 'v':
			verbose = atoi(optarg);
			break;
		case 'g':
			dotFile = optarg;
			break;
		case '?':
			fprintf(stderr, "unknown option -%c!\n", optopt);
			errflg++;
			break;
		default:
			fprintf(stderr, "internal getopt error (returned 0%o)!\n", c);
			errflg++;
			break;
		}
	}

	if (optind < argc) {
		warpFile = argv[optind];
		optind++;
	} else {
		fprintf(stderr, "missing warp_file parameter!\n");
		errflg++;
	}

	// no further parameters expected
	if (optind < argc) {
		fprintf(stderr, "too many parameters!\n");
		errflg++;
	}

	// check if any errors occurred during parameter parsing
	if (errflg>0) {
		printUsage();
		return 0;
	}

	// override verbose flag for debugging (ensure this is commented out for production code)
	//verbose = 3;

	if (verbose>1) {
		CTokenFactory::instance()->setPrintInfos();
	}

	printf("warp is reading file %s\n", warpFile.c_str());
	warp::model::CSimModel model;
	if (! model.readFile(warpFile.c_str(), verbose>0)) {
		printf("file could not be read due to previous errors!\n");
		return 100;
	}

	mon.done("init");

	// do simulation
	{
		warp::CSimulatorCore core(verbose);

		// inject initial steps
		model.addInitials(core, core);
		//core.print();

		// run simulation
		bool withLoadfile = (verbose>2);
		core.run(
				model.getResources(),
				model.getResourceSlots(),
				model.getPools(),
				withLoadfile,
				dotFile);
	}

	mon.done("simulate");

	int retval = 0;

	if (model.checkRemnants() > 0) {
		retval = 200;
	}
	mon.report();

	return retval;
}


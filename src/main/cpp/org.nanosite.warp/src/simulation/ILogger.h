/*
 * ILogger.h
 *
 *  Created on: 13.08.2009
 *      Author: kbirken
 */

#ifndef ILOGGER_H_
#define ILOGGER_H_

class ILogger {
public:
	virtual void fatal (const char* fmt, ...) = 0;
	virtual void log (const char* action, const char* fmt, ...) = 0;
	virtual int verbose() const = 0;
};

#endif /* ILOGGER_H_ */

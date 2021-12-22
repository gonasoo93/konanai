/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#pragma once

#include "../reporter.h"
#include "../utils/utils.h"

class AbaloneReporter : public Reporter {
public:
	AbaloneReporter();
	virtual ~AbaloneReporter();

protected:
	virtual KBool m_visualizeEnd(void* pAux, KString sName, KString sTimestamp, KaiDict xs, KaiDict ys, KaiDict os, KaiDict vs);

protected:
	int m_version;
	static int ms_checkCode;

protected:
};

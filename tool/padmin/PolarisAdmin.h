/*
 * PolarisAdmin.h
 *
 *  Created on: Oct 7, 2011
 *      Author: jj
 */

#ifndef POLARISADMIN_H_
#define POLARISADMIN_H_

#include "common.h"
#include "NMSTL/debug"
#include "NMSTL/ptr"
#include "MedusaClient.h"

BOREALIS_NAMESPACE_BEGIN

class PolarisAdmin
{
public:
	PolarisAdmin();
	void transferSQLToXML(string sqlFilename, string xmlFilename);
	void start(string sqlFilename);
	virtual ~PolarisAdmin();
};

BOREALIS_NAMESPACE_END

#endif /* POLARISADMIN_H_ */

/*
 * info.h
 *
 *  Created on: 7 paź 2014
 *      Author: abby
 */

#ifndef INFO_H_
#define INFO_H_

#include "libs.h"

class info {
public:
	info();
	virtual ~info();

	static std::string GetCurrWorkspace();
	static std::string GetCurrWin();
};

#endif /* INFO_H_ */

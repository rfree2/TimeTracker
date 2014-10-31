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

	static std::string Command(char *cmd); // executes bash command and return >>output<<

	static std::string GetCurrWorkspace() noexcept; ///< uses wmctrl and awk
	static std::string GetCurrWin() noexcept; ///< FIXME: For some application (e.g xchat) c-string is NULL (works for e.g. chromium)

	static void Cut(std::string &s);

	protected:
		static std::string GetCurrWorkspace_Detail(); // uses wmctrl and awk
		static std::string GetCurrWin_Detail(); // FIXME: For some application (e.g xchat) c-string is NULL (works for e.g. chromium)

};

#endif /* INFO_H_ */

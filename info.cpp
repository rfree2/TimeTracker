/*
 * info.cpp
 *
 *  Created on: 7 paź 2014
 *      Author: abby
 */

#include "info.h"

using namespace std;

info::info() {
	// TODO Auto-generated constructor stub

}

info::~info() {
	// TODO Auto-generated destructor stub
}

string info::GetCurrWorkspace() {
	const auto w = system(" wmctrl -d | grep \"*\" | awk \'{print $10 }\'");
	ostringstream output;
	output << w;

	return output.str();
}

std::string info::GetCurrWin() {
	Display *display;
	Window focus;
	char *window_name;
	int revert;

	display = XOpenDisplay(NULL);
	XGetInputFocus(display, &focus, &revert);
	XFetchName(display, focus, &window_name);

	cout << "tittle: " << window_name << endl;

	if (window_name) {

		const string win_name(window_name);

		printf("%s\n", window_name);

		return win_name;
	} else
		return "";
}

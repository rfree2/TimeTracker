/*
 * analyser.cpp
 *
 *  Created on: 10 paź 2014
 *      Author: abby
 */

#include "analyser.h"

analyser::analyser(const std::string& filename) :
		displayAll_(false), logging(false) {
	_info(filename);
	if (ControlFile(filename))
		Timesheet(filename);
}

analyser::~analyser() {
	// TODO Auto-generated destructor stub
}

void analyser::Timesheet(const std::string& filename) {
	using namespace std;
	fstream file;
	file.open(filename.c_str(), ios::in | ios::out);
	if (!file.is_open()) {
		_erro("Can't open file " << filename);
		return;
	}

	while (!file.eof()) {
		string a;
		getline(file, a);

		if (a == "")
		break;
		TimesheetProcess(a);
	}
	Correct();
	PrintTable();
}

std::shared_ptr<task> analyser::LineToTask(const std::string &line) {
	using namespace boost::posix_time;
	const auto splited = nOT::nUtils::SplitString(line);
	assert(splited.size() == 4);
	//	for (auto tmp : splited)
	//		_dbg3(tmp);

	// format:
	//     [0]             [1]      [2]   [3]
	//	2014-Oct-10		16:19:43	C	task

	auto new_task = std::make_shared<task>(splited.at(0) + " " + splited.at(1),
			splited.at(2), splited.at(3));

	// only for test - is creating task the same as line?
	std::ostringstream oss;
	oss << *new_task;

	assert(line == oss.str());
	return new_task;
}

void analyser::TimesheetProcess(const std::string& line) {
	auto new_task = LineToTask(line);
	const auto type = new_task->getType();

	if (type == task::state::B) { // if new task has type "begin" create new info struct
		Correct();
		auto task_info = std::make_shared<taskInfo>();
		task_info->name = new_task->name_;
		task_info->ttask = new_task;
		task_info->start_time = new_task->day_time_;
		task_info->has_end = false;
		taskInformations_.push_back(task_info);
	}
	else if (type == task::state::E) { // end - save ending time to struct
		const auto stru = taskInformations_.back();
		const auto ttask = stru->ttask;
		if (*new_task != *ttask) {
			_erro(
					"Critical! End task without beginning!: end_task " << ttask->name_ << "; new_task: " << new_task->name_);
			return;
		}
		// all ok
		taskInformations_.back()->end_time = new_task->day_time_;
		taskInformations_.back()->has_end = true;
		taskInformations_.back()->total_time =
				taskInformations_.back()->end_time
						- taskInformations_.back()->start_time;

//		PrintOne(taskInformations_.back());

	}
	ltask = new_task;
}

void analyser::PrintOne(const std::shared_ptr<taskInfo> top) const {
	_mark(*top->ttask);
	_dbg1(top->total_time);
}

void analyser::PrintTable() const {
	using namespace std;
	using namespace boost::posix_time;
	for (auto ttask : taskInformations_) {
		const auto st = to_iso_extended_string(ttask->start_time).substr(11, 18);
		const auto et = to_iso_extended_string(ttask->end_time).substr(11, 18);

		cout << setw(10) << ttask->name << " | " << st << " " << et << " " << ttask->total_time << " \n";
	}
}

void analyser::AddMap(const std::shared_ptr<task> task_, std::map<std::string,
		taskInfo> &map) {
	const auto name = task_->name_;
	taskInfo tstruct;
	tstruct.name = name;
	tstruct.start_time = task_->day_time_;

	map.insert(std::pair<std::string, taskInfo>(name, tstruct));
}

void analyser::PrintMaps() const {
	using namespace std;
	const string sep = "+-----------------------------------";

	for (auto &it1 : Map_) {
		cout << "\t" << it1.first << endl;
		for (auto& it : it1.second) {
			cout << sep << endl;
			cout << "| Name: | " << it.first << endl;
			cout << "| Time: | " << it.second.total_time << endl;
		}
		cout << sep << endl;
		cout << endl;

	}
}

void analyser::Display() {
	if (!displayAll_) {
		PrintMaps();
		return;
	}
	using namespace std;
	Merge();
	const string sep = "+-----------------------------------";

	for (auto& it : mergedMap_) {
		cout << sep << endl;
		cout << "| Name: | " << it.first << endl;
		cout << "| Time: | " << it.second.total_time << endl;
	}
	cout << sep << endl;
	cout << endl;

}

void analyser::Merge() {
	using namespace std;
	assert(!Map_.empty());

	for (auto &it1 : Map_) {
		for (auto& it : it1.second) {
			auto found = mergedMap_.find(it.first);

			if (found == mergedMap_.end())
			mergedMap_.insert(pair<string, taskInfo>(it.first, it.second));
			else {
				mergedMap_.at(it.first).total_time += it.second.total_time;
			}
		}
	}
}

void analyser::Correct() {
	if (taskInformations_.empty()) return;
	if (taskInformations_.back()->has_end) return;

	_info("correcting");
	taskInformations_.back()->end_time = ltask->day_time_;
	taskInformations_.back()->total_time = taskInformations_.back()->end_time
			- taskInformations_.back()->start_time;
	taskInformations_.back()->has_end = true;

}

analyser::analyser(std::vector<std::string> fnames, bool da) :
		displayAll_(da), logging(false) {
	bool ok = false;
	for (auto fn : fnames)
		if (ControlFile(fn)) ok = true;

	assert(ok);
	SummaryGetFromFile(fnames);
	Display();
}

void analyser::SummaryGetFromFile(std::vector<std::string>& fnames) {
	using namespace std;
	sort(fnames.begin(), fnames.end());

	for (auto fname : fnames) {
		_dbg2(fname);
		fstream file;
		file.open(fname.c_str(), ios::in | ios::out);

		if (!file.is_open()) // can't open file, nothing to do
		_warn("Can't open file " << fname);

		else {
			//_mark();
			taskInfo tstruct;
			map<string, taskInfo> dMap;
			Map_.insert(std::pair<std::string, std::map<std::string, taskInfo> >(fname, dMap));

			while (!file.eof()) {
				string a = "";
				getline(file, a);
				if (a == "") break;
				SummaryProcessTask(LineToTask(a), Map_.at(fname));
			}
		}
	}
}

void analyser::SummaryProcessTask(const std::shared_ptr<task> task_, std::map<
		std::string, taskInfo> &taskMap) {
	const auto tname = task_->name_;

	auto it = taskMap.find(tname); // exist element with key tname?
	if (it == taskMap.end()) { // not found, create them, nothing to do
		AddMap(task_, taskMap);
		return;
	}

	auto &tinfo = it->second;
	auto &start_time = tinfo.start_time;
	auto &curr_time = task_->day_time_;

	tinfo.total_time = curr_time - start_time;

}

analyser::analyser(std::vector<std::string> fnames) : displayAll_(true), logging(true) {
	const std::string dir = "data/";
	bool ok = false;
	for (auto fn : fnames)
		if (ControlFile(fn)) ok = true;

	assert(ok);
	SummaryGetFromFile(fnames);

	using namespace std;
	const auto tdate = boost::posix_time::second_clock::local_time();
	ostringstream oss;
	oss << "OUT-" << tdate << ".txt";
	string logname = dir + oss.str();
	logname.replace(logname.find(" "), 1, "-");
	size_t control = string::npos;

	while(control != string::npos){
		control = logname.find(":");
		logname.replace(control, 1, " ");
	}

	assert(logname.find(" ")==string::npos);

	_mark(logname);

	if(!boost::filesystem::exists(dir))
		assert(boost::filesystem::create_directory(dir));

	assert(!boost::filesystem::exists(logname));

	// redirecting cout stream
	// http://coliru.stacked-crooked.com/a/9c4729dd796c5daf
	ofstream logFile;
	logFile.open(logname.c_str());
	ostream tmp(cout.rdbuf());
	TeeDevice outputDevice(tmp, logFile); // <----
	TeeStream logger(outputDevice);

	cout.rdbuf(logger.rdbuf());

	PrintMaps();
	cout << "\t == ALL ==" << endl;
	Display();

	logger.close();
}

bool analyser::ControlFile(const std::string& name) const {
	bool e = boost::filesystem::exists(name);
	if(!e) _erro("File [" << name <<"] doesn't exist!" );

	if(name.find(".task")==std::string::npos)
		_warn("File: [" << name << "] has wrong extension, trying to read them");

	return e;
}

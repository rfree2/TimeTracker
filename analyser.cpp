/*
 * analyser.cpp
 *
 *  Created on: 10 paź 2014
 *      Author: abby
 */

#include "analyser.h"

analyser::analyser() : displayAll_(false) {
	_dbg3("constructor");
}

analyser::~analyser() {
	// TODO Auto-generated destructor stub
}

std::shared_ptr<task> analyser::LineToTask(const std::string &line) {
	using namespace boost::posix_time;
	const auto splited = nOT::nUtils::SplitString(line);
	assert(splited.size() == 4);
	//	for (auto tmp : splited)
	//		_dbg3(tmp);

	// format:
	//     [0]             [1]      [2]   [3]
	//	2014-Oct-10		16:19:43	C	  task

	// splited.at(2) must be type of task
	assert(splited.at(2) == "B" || splited.at(2) == "C" || splited.at(2) == "E");

	auto new_task = std::make_shared<task>(splited.at(0) + " " + splited.at(1),
			splited.at(2), splited.at(3));

	// only for test - is creating task the same as line?
	std::ostringstream oss;
	oss << *new_task;

	assert(line == oss.str());
	return new_task;
}

void analyser::PrintOne(const std::shared_ptr<taskInfo> top) const {
	_mark(*top->ttask);
	_dbg1(top->total_time);
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

analyser::analyser(std::vector<std::string> fnames, bool da) :
		displayAll_(da) {
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

		else { // file is open
			string task_date = "";

			// reading file
			while (!file.eof()) {
				string a = "";
				getline(file, a);
				if (a == "") break; // no data = brak loop

				auto tmp_task = LineToTask(a);
				assert(tmp_task->dateToString() != "");

				// if current task is first or has different date than previous, create new map for this task
				if (task_date != tmp_task->dateToString() || task_date == "") {
					task_date = tmp_task->dateToString();
					taskInfo tstruct;
					map<string, taskInfo> dMap;
					Map_.insert(std::pair<std::string, std::map<std::string, taskInfo> >(task_date, dMap));
				} SummaryProcessTask(tmp_task, Map_.at(task_date));
			}
		}
	}
}

void analyser::SummaryProcessTask(const std::shared_ptr<task> task_, std::map<std::string, taskInfo> &taskMap) {
	const auto tname = task_->name_;

	auto it = taskMap.find(tname); // exist element with key tname?
	if (it == taskMap.end()) { // not found, create them, nothing to do
		AddMap(task_, taskMap);
		return;
	}

	// saving informations
	auto &tinfo = it->second;
	auto &start_time = tinfo.start_time;
	auto &curr_time = task_->day_time_;
	tinfo.total_time = curr_time - start_time;
}

analyser::analyser(std::vector<std::string> fnames) : displayAll_(true) {
	const std::string dir = "data/";
	bool ok = false;
	for (auto fn : fnames)
		if (ControlFile(fn)) ok = true;

	assert(ok);
	SummaryGetFromFile(fnames);

	// preparing logname
	using namespace std;
	const auto tdate = boost::posix_time::second_clock::local_time();
	ostringstream oss;
	oss << "OUT-" << tdate << ".txt";
	string logname = dir + oss.str();

	// no spaces and :
	logname.replace(logname.find(" "), 1, "-");
	size_t control = string::npos;
	while(control != string::npos){
		control = logname.find(":");
		logname.replace(control, 1, " ");
	}
	assert(logname.find(" ")==string::npos);
	_mark(logname);

	// directory for logs
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

	// printing
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

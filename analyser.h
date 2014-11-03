/*
 * analyser.h
 *
 *  Created on: 10 paź 2014
 *      Author: abby
 */

#ifndef ANALYSER_H_
#define ANALYSER_H_

#include "libs.h"
#include "task.h"

//namespace bprinter {
//	class TablePrinter;
//}

class analyser {
public:
	analyser(const std::string &filename);
	analyser(std::vector <std::string> fnames, bool da);

	virtual ~analyser();
	void Timesheet(const std::string &filename);

private:
	std::shared_ptr <task> LineToTask(const std::string &line);

	void SummaryGetFromFile(std::vector <std::string> &fnames);
	void TimesheetProcess(const std::string &line); // 2014-Oct-10 10:14:42 B ot

	std::vector <task>tasks_;


	struct taskInfo {
		std::shared_ptr <task> ttask;
		std::string name;
		boost::posix_time::ptime start_time;
		boost::posix_time::ptime end_time;
		boost::posix_time::time_duration total_time;
		bool has_end;
		std::string memo;
	};

	void SummaryProcessTask(const std::shared_ptr<task> task_, std::map <std::string, taskInfo> &taskMap ); // adding to taskMap_

	std::map <std::string, taskInfo> mergedMap_;

	void Merge();


	std::map <std::string, std::map <std::string, taskInfo> > Map_; //constains date (as key) and map for every day
	std::vector <std::shared_ptr<taskInfo>> taskInformations_;
	std::shared_ptr <task> ltask;
	const bool displayAll_;


	void AddMap(const std::shared_ptr<task> task_, std::map <std::string, taskInfo> &map);
	void PrintOne(const std::shared_ptr<taskInfo> top) const;
	void PrintTable() const;
	void PrintMaps() const;
	void Display();
	void Correct();
};

#endif /* ANALYSER_H_ */

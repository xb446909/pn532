#ifndef __FILE_LOGGER_H
#define __FILE_LOGGER_H

enum {
		System = -1,
		Error = 0,
		Warning = 1,
		Debug = 2,
		Info = 3
	};


extern void AddLogLine(int verbosity, const char* fmt, ...);
extern void SetVerbosity(int verbosity);

#define LOG_SYSTEM(...)				AddLogLine(System, ##__VA_ARGS__)
#define LOG_ERROR(...)				AddLogLine(Error, ##__VA_ARGS__)
#define LOG_WARNING(...)			AddLogLine(Warning, ##__VA_ARGS__)
#define LOG_DEBUG(...)				AddLogLine(Debug, ##__VA_ARGS__)
#define LOG_INFO(...)				AddLogLine(Info, ##__VA_ARGS__)

#endif /*__FILE_LOGGER_H*/

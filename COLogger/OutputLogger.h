#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <any>

namespace OutLog
{
	enum OLoggerLevel
	{
		Verbose,
		Log,
		Warning,
		Error,
		Critical
	};

	class OutputLogger
	{
		public:
			OutputLogger(bool useConsole, bool logToFile);
			~OutputLogger();

			void LogMessage(OLoggerLevel logLevel, const std::string& message);
			void ClearOutput();

			template<typename T, typename... TArgs>
			std::string FormatMsg(const std::string& message, T firstArg, TArgs&&... args)
			{
				std::string oldMessage = std::string(message);
				std::string newMessage;

				int currentVarIndex = 0;
				bool onVarFormat = false, appendedArg = false, hexPrint = false;
				for (int i = 0; i < oldMessage.length(); i++)
				{
					if (!appendedArg && oldMessage[i] == '{')
					{
						onVarFormat = true;
						continue;
					}
					
					if (appendedArg && onVarFormat && oldMessage[i] == '}')
					{
						onVarFormat = false;
						continue;
					}

					if (!onVarFormat)
					{
						newMessage += oldMessage[i];
						hexPrint = false;	
						continue;
					}

					if (oldMessage[i] == '#')
					{
						hexPrint = true;
						continue;
					}

					if (appendedArg) continue;
					currentVarIndex = oldMessage[i] - 0x30;
					newMessage.append(CastToType(firstArg, hexPrint));

					appendedArg = true;
					hexPrint = false;
				}

				return FormatMsg(newMessage, args...);
			}

			static void InitLog(bool useConsole, bool logToFile);
			inline static std::shared_ptr<OutputLogger>& GetLoggerInstance() { return logger; }

		private:
			static std::shared_ptr<OutputLogger> logger;

			bool usingConsole, usingFile;
			void* outConsoleHandle;
			std::fstream* pLogFileStream;
			std::string fileName = "output.log";

			bool InitConsoleLog();
			bool InitFileLog();
			std::string ParseLevelToColor(OLoggerLevel level) const;
			std::string ParseLevelToString(OLoggerLevel level) const;

			std::string CastToType(std::any typeToCast, bool hexPrint) const;

			template<typename T>
			std::string ToHex(T value) const;

			std::string FormatMsg(const std::string& message) { return message; }
	};
}

#define InitLogSystem(...) OutLog::OutputLogger::InitLog(__VA_ARGS__)
#define OLOG(...) OutLog::OutputLogger::GetLoggerInstance()->LogMessage(__VA_ARGS__)
#define OLOG_CLEAR() OutLog::OutputLogger::GetLoggerInstance()->ClearOutput()

#define OLOG_V(x) OLOG(OutLog::OLoggerLevel::Verbose, x)
#define OLOG_L(x) OLOG(OutLog::OLoggerLevel::Log, x)
#define OLOG_W(x) OLOG(OutLog::OLoggerLevel::Warning, x)
#define OLOG_E(x) OLOG(OutLog::OLoggerLevel::Error, x)
#define OLOG_C(x) OLOG(OutLog::OLoggerLevel::Critical, x)

#define OFORMAT(...) OutLog::OutputLogger::GetLoggerInstance()->FormatMsg(__VA_ARGS__)

#define OLOG_VF(...) OLOG(OutLog::OLoggerLevel::Verbose, OFORMAT(__VA_ARGS__))
#define OLOG_LF(...) OLOG(OutLog::OLoggerLevel::Log, OFORMAT(__VA_ARGS__))
#define OLOG_WF(...) OLOG(OutLog::OLoggerLevel::Warning, OFORMAT(__VA_ARGS__))
#define OLOG_EF(...) OLOG(OutLog::OLoggerLevel::Error, OFORMAT(__VA_ARGS__))
#define OLOG_CF(...) OLOG(OutLog::OLoggerLevel::Critical, OFORMAT(__VA_ARGS__))

#ifdef DLL
extern "C" _declspec(dllexport) void InitLogSys(bool useConsole, bool logToFile) { InitLogSystem(useConsole, logToFile); }
extern "C" _declspec(dllexport) void Log(int logLevel, char* message) { OLOG((OutLog::OLoggerLevel) logLevel, message); }
extern "C" _declspec(dllexport) void ClearConsole() { OLOG_CLEAR(); }

// Don't think you can export a generic method like FormatMsg :(
#endif

#ifdef ODIS_V
#define OLOG_V(x)
#define OLOG_VF(...)
#endif

#ifdef ODIS_VLW
#define OLOG_V(x)
#define OLOG_L(x)
#define OLOG_W(x)

#define OLOG_VF(...)
#define OLOG_LF(...)
#define OLOG_WF(...)
#endif

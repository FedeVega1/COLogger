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

			void LogMessage(OLoggerLevel logLevel, std::string message);

			template<typename T, typename ... TArgs>
			std::string FormatMsg(std::string message, T firstArg, TArgs&&... args)
			{
				std::string oldMessage = std::string(message);
				std::string newMessage;

				int currentVarIndex = 0;
				bool onVarFormat = false, appendedArg = false;
				for (int i = 0; i < oldMessage.length(); i++)
				{
					if (!appendedArg && oldMessage[i] == '{')
					{
						onVarFormat = true;
						continue;
					}
					
					if (appendedArg || !onVarFormat)
					{
						newMessage += oldMessage[i];
						continue;
					}

					if (oldMessage[i] == '}')
					{
						appendedArg = true;
						onVarFormat = false;
						continue;
					}

					currentVarIndex = oldMessage[i] - 0x30;
					newMessage.append(CastToType(firstArg));
				}

				return FormatMsg(newMessage, args...);
			}

			static void InitLog(bool useConsole, bool logToFile);
			inline static std::shared_ptr<OutputLogger>& GetLoggerInstance() { return logger; }

		private:
			static std::shared_ptr<OutputLogger> logger;

			bool InitConsoleLog();
			bool InitFileLog();
			std::string ParseLevelToColor(OLoggerLevel level) const;
			std::string ParseLevelToString(OLoggerLevel level) const;

			std::string CastToType(std::any typeToCast) const;

			std::string FormatMsg(std::string message) { return message; }

			bool usingConsole, usingFile;
			void* outConsoleHandle;
			std::fstream* pLogFileStream;
			std::string fileName = "output.log";
	};
}

#define InitLogSystem(...) OutLog::OutputLogger::InitLog(__VA_ARGS__)
#define OLOG(...) OutLog::OutputLogger::GetLoggerInstance()->LogMessage(__VA_ARGS__)

#define OLOG_V(x) OLOG(OutLog::OLoggerLevel::Verbose, x)
#define OLOG_L(x) OLOG(OutLog::OLoggerLevel::Log, x)
#define OLOG_W(x) OLOG(OutLog::OLoggerLevel::Warning, x)
#define OLOG_E(x) OLOG(OutLog::OLoggerLevel::Error, x)
#define OLOG_C(x) OLOG(OutLog::OLoggerLevel::Critical, x)

#define OFORMAT(...) OutLog::OutputLogger::GetLoggerInstance()->FormatMsg(__VA_ARGS__)

#define OLOG_VF(...) OLOG(OutLog::OLoggerLevel::Verbose, OFORMAT(__VA_ARGS__));
#define OLOG_LF(...) OLOG(OutLog::OLoggerLevel::Log, OFORMAT(__VA_ARGS__));
#define OLOG_WF(...) OLOG(OutLog::OLoggerLevel::Warning, OFORMAT(__VA_ARGS__));
#define OLOG_EF(...) OLOG(OutLog::OLoggerLevel::Error, OFORMAT(__VA_ARGS__));
#define OLOG_CF(...) OLOG(OutLog::OLoggerLevel::Critical, OFORMAT(__VA_ARGS__));

#include "pch.h"
#include "OutputLogger.h"

namespace OutLog
{
	std::shared_ptr<OutputLogger> OutputLogger::logger;

	OutputLogger::OutputLogger(bool useConsole, bool logToFile) : pLogFileStream(nullptr), outConsoleHandle(nullptr)
	{ 
		usingConsole = useConsole;
		usingFile = logToFile;
		bool result = true;

		if (useConsole) result = InitConsoleLog();
		if (logToFile) result = InitFileLog();
		if (!result) throw std::runtime_error("FATAL EXCEPTION: Couldn't initialize logger");
	}

	OutputLogger::~OutputLogger() 
	{ 
		if (!pLogFileStream) return;
		pLogFileStream->close();
		delete pLogFileStream;
	}

	void OutputLogger::InitLog(bool useConsole, bool logToFile)
	{
		logger = std::make_shared<OutputLogger>(useConsole, logToFile);
	}

	bool OutputLogger::InitConsoleLog()
	{
		if (!AllocConsole())
		{
			throw std::runtime_error("Failed to alloc console");
			return false;
		}

		FILE* dummyFile;
		freopen_s(&dummyFile, "nul", "w", stdout);

		outConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (outConsoleHandle == INVALID_HANDLE_VALUE) return false;

		int fileDescriptor = _open_osfhandle((intptr_t) outConsoleHandle, _O_TEXT);
		if (fileDescriptor == -1) return false;

		FILE* file = _fdopen(fileDescriptor, "w");
		if (!file) return false;

		int dup2Result = _dup2(_fileno(file), _fileno(stdout));
		if (dup2Result == 0) setvbuf(stdout, NULL, _IONBF, 0);

		DWORD originalOutMode = 0;
		if (!GetConsoleMode(outConsoleHandle, &originalOutMode)) return false;

		DWORD newOutMode = originalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(outConsoleHandle, newOutMode)) return false;

		std::wcout.clear();
		std::cout.clear();

		return true;
	}

	bool OutputLogger::InitFileLog()
	{
		pLogFileStream = new std::fstream();
		pLogFileStream->open(fileName, std::fstream::app);
		*pLogFileStream << "----------------------------------------------------------------------" << std::endl;
		return true;
	}

	void OutputLogger::LogMessage(OLoggerLevel logLevel, std::string message)
	{
		// Why the fuck is this generating a memory leak?
		const auto time = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }; 

		if (usingConsole) std::cout << ParseLevelToColor(logLevel) << "[" << time.get_local_time() << "] " << ParseLevelToString(logLevel) << ": " << message << "\033[0m" << std::endl;
		if (usingFile) *pLogFileStream << "[" << time.get_local_time() << "] " << ParseLevelToString(logLevel) << ": " << message << std::endl;
	}

	std::string OutputLogger::ParseLevelToColor(OLoggerLevel level) const
	{
		switch (level)
		{
			case OLoggerLevel::Verbose: return "\033[37m";
			case OLoggerLevel::Log: return "\033[97m";
			case OLoggerLevel::Warning: return "\033[93m";
			case OLoggerLevel::Error: return "\033[91m";
			default: return "\033[101m\033[97m";
		}
	}

	std::string OutputLogger::ParseLevelToString(OLoggerLevel level) const
	{
		switch (level)
		{
			case OLoggerLevel::Verbose: return "Verbose";
			case OLoggerLevel::Log: return "Log";
			case OLoggerLevel::Warning: return "Warning";
			case OLoggerLevel::Error: return "Error";
			default: return "Critical";
		}
	}

	std::string OutputLogger::CastToType(std::any typeToCast, bool hexPrint) const
	{
		if (typeToCast.type() == typeid(std::string)) 
			return std::any_cast<std::string>(typeToCast);

		if (typeToCast.type() == typeid(char))
		{
			char castedValue = std::any_cast<char>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(char*))
			return std::string(std::any_cast<char*>(typeToCast));

		if (typeToCast.type() == typeid(byte))
		{
			byte castedValue = std::any_cast<byte>(typeToCast);
			if (hexPrint) return  ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(short))
		{
			short castedValue = std::any_cast<short>(typeToCast);
			if (hexPrint) return  ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(int))
		{
			int castedValue = std::any_cast<int>(typeToCast);
			if (hexPrint) return  ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(unsigned int))
		{
			unsigned int castedValue = std::any_cast<unsigned int>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(long))
		{
			long castedValue = std::any_cast<long>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(unsigned long))
		{
			unsigned long castedValue = std::any_cast<unsigned long>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(long long))
		{
			long long castedValue = std::any_cast<long long>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(unsigned long long))
		{
			unsigned long long castedValue = std::any_cast<unsigned long long>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(double))
		{
			double castedValue = std::any_cast<double>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(long double))
		{
			long double castedValue = std::any_cast<long double>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		if (typeToCast.type() == typeid(float))
		{
			float castedValue = std::any_cast<float>(typeToCast);
			if (hexPrint) return ToHex(castedValue);
			return std::to_string(castedValue);
		}

		throw std::runtime_error("Unknown type to cast");
	}

	template<typename T>
	std::string OutputLogger::ToHex(T value) const { return std::vformat("{:#04x}", std::make_format_args(value)); }

	void OutputLogger::ClearOutput()
	{
		std::cout << "\x1B[2J\x1B[H";
	}
}

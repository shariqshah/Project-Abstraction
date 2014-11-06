#include "scriptengine.h"

namespace ScriptEngine
{
	namespace
	{
		static VM sVmInstance;
		static Sqrat::SqratVM *vm;
	}

	void printfunc(HSQUIRRELVM v,const SQChar *s,...)
	{
		va_list vl;
		va_start(vl, s);
		vfprintf(stdout, s, vl);
		va_end(vl);
	}

	void errorfunc(HSQUIRRELVM v,const SQChar *s,...)
	{
		va_list vl;
		va_start(vl, s);
		vfprintf(stderr, s, vl);
		va_end(vl);
	}
	
	void initialize()
	{
		vm = new Sqrat::SqratVM(1024);
		vm->SetPrintFunc(printfunc, errorfunc);
		sVmInstance = vm->GetVM();
	}

	void cleanup()
	{
		//sq_close(sVmInstance);
		delete vm;
	}

	VM getVM()
	{
		return sVmInstance;
	}

	void runScript(const std::string& scriptName)
	{
		try
		{
			Sqrat::Script script(sVmInstance);
			script.CompileFile(scriptName);
			script.Run();
		}
		catch(Sqrat::Error& e)
		{
			Log::error(Log::ErrorLevel::LOW, e.Message(sVmInstance));
		}
	}

	
}

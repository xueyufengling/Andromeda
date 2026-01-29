#include <cxxjvmi/jvm.h>

using namespace cxxjvmi;

jvm::jvm()
{
	CreateJavaVM createJavaVM;
	loadJvmDll(&createJavaVM);
	cout << "Creating JVM Instance" << endl;
	// https://docs.oracle.com/javase/10/docs/specs/jni/invocation.html
	JavaVMInitArgs vm_args;
	auto* options = new JavaVMOption[3];
	options[0].optionString = (char*)"-Djava.class.path=";
	options[1].optionString = (char*)"-Djava.library.path=";
	options[2].optionString = (char*)"-verbose:jni";
	vm_args.version = JNI_VERSION_1_8;
	vm_args.nOptions = 3;
	vm_args.options = options;
	vm_args.ignoreUnrecognized = false;
	cout << "Starting VM..." << endl;
	createJavaVM(&jvm, (void**)&env, &vm_args);
	cout << "VM started" << endl;

	delete[] options;
}

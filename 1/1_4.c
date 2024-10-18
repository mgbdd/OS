#include <dlfcn.h>
#include <stdio.h>

int main()
{
	void* libraryHandle = dlopen("./libdynamic_lib.so", RTLD_LAZY); //загрузка библиотеки
	if(libraryHandle)
	{
		void (*hello)() = dlsym(libraryHandle, "hello_from_dynamic_lib");//получение указателя на функцию
		if(hello)
		{
			hello();
		}
		dlclose(libraryHandle);
	}
	return 0;
}

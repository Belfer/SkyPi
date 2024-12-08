#pragma once

int RuntimeMain(int argc, char** args);

#if defined(WIN32) || defined(__arm__)
int main(int argc, char** args)
{
	return RuntimeMain(argc, args);
}
#endif
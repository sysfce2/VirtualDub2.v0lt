#include <windows.h>

bool ShouldBreak() {
	return !!IsDebuggerPresent();
}

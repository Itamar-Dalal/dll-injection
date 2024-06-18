# dll-injection
Dll-Injection made in c with winAPI.
This program has 2 folders:
<h2>my_dll:</h2> containing the dll file that we want to open when perform the injection (in this case a window pops up with some text).
<h2>dll_injection:</h2> the file that performs the dll-injection. It has 2 varaibles that need to be changed in order for the injection to work: <h4>processName</h4> and <h4>dllPath</h4> <hr/> <b>processName</b> is the name of the target process that the dll will open each time the process is running (for example "WINWORD.EXE").<br/><br/> 
<b>dllPath</b> is the path for the dll file.

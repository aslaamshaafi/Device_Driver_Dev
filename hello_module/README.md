### Hello Module
Steps to run the modules
- run **make** in commandline to generate ko file
- run **insmod first_module.ko mystring="hello module" var_vector=25 var_intArray=1** commandline to insert the module into kernel
- run **dmesg** or **cat /proc/modules** to check if the module is properly insterted
- run **rmmod** to remove the inserted module



##xmlbib

A project to process bibliographical data downloaded from NNDC Nuclear Science Reference database in xml format, and filter selected references based on properties of the isotopes mentioned in the <keyword> node of the data.

The code is based in the pugixml library to use xml tools in c++: https://pugixml.org

filter_bib.cpp: writen to select papers with experimental data of neutron rich isotopes of interest to r-process. Selects papers that mention isotopes with Zmin < Z < 113, and have neutron number a distance 'delta' from a parameterization of the most neutron-rich stable isotope of even Z elements. Default values are Zmin=25 and delta=2.

The function is_interesting(int, int, int, int) has the criteria for including a reference in the filtered database, based on the neutron number of the isotopes identified for that paper. Quite a few things are hard coded, and still to be improved in future versions...

filter2table_bib.cpp: added an additional output in a format that can be easily loaded into a spreadsheet (e.g. Excel) - it looks like excel supports loading xml data, but not in its Mac version... The output in xml2table is a text file that has information of a few of the <tags> in original xml database separated by tabs. Only references that have been selected as interesting for the r-process are included. The output also includes a list of the isotopes that match the criteria for r-process isotopes.

How to compile. Form the folder src:

g++ -o filter2table_bib filter2table_bib.cpp pugixml.cpp 

Same command if you want to compile only filter_bib.cpp
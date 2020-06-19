#xmlbib

A project to process bibliographical data downloaded from NNDC Nuclear Science Reference database in xml format, and filter selected references based on properties of the isotopes mentioned in the <keyword> node of the data.

The code is based in the pugixml library to use xml tools in c++: https://pugixml.org

filter_bib.cpp: writen to select papers with experimental data of neutron rich isotopes of interest to r-process. Selects papers that mention isotopes with Zmin < Z < 113, and have neutron number a distance 'delta' from a parameterization of the most neutron-rich stable isotope of even Z elements. Default values are Zmin=25 and delta=2.


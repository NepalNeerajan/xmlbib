#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <ctype.h>
#include "pugixml.hpp"

int elemZ[113]={0};
std::string elemSymbol[113]={""};

bool load_elements(){
  std::ifstream elem_file("elements.txt");
  std::string str1, str2;
  int x;

  for(int i=0; i<113; i++){
    elem_file >> str1 >> str2 >> x;
    elemSymbol[i]=str2;
    elemZ[i]=x;
    if(elem_file.fail()) return false;
    //std::cout << elemZ[i] <<": "<<elemSymbol[i]<<std::endl;
  }

return true;
}

bool is_digits(const std::string &str){
  return str.find_first_not_of("0123456789") == std::string::npos;
}

bool get_A(const std::string & mass, int & A){
  A=-1;
  std::stringstream stream(mass);
  stream >> A;
  if(!stream) return false;
  else return true;
}

bool get_element(char ch1, char ch2, std::string &element, int &Z){
  element = ""; Z= -1;
  if(isupper(ch1)){
    element.push_back(ch1);
    if(islower(ch2)) element.push_back(ch2);

    for(int i=0; i<113; i++){
      if(element == elemSymbol[i]){
        Z= elemZ[i];
        break;
      }
    }
    return true;
  }
  else return false;
}


//check if this is an interesting isotope; e.g., is it neutron-rich enough?
bool is_interesting(int Z,int A, int Zmin=26, int delta=2){

  double p0 = -4.801;
  double p1 = 1.416;
  double p2 = 2.372e-3;

  double Nstab; 
  Nstab= p0 + p1*Z + p2*Z*Z;

  if( Z>=Zmin && ((A-Z)>(Nstab+delta)) ){
    //hard coded limit for Sn, Te, Xe isotopes that deviate from parameterization
    if(Z==50){ if( A>127 ) return true;}
    else if(Z==52){ if( A>131 ) return true;}
    else if(Z==54){ if( A>137 ) return true;}

    else return true;
  }
  return false;
}


int main()
{
  using namespace pugi;

  // Load XML file from fstream

  //  std::ifstream xml_file("test.xml");
  std::ifstream xml_file("bdn.xml");

  if(!load_elements()){
    std::cerr << "ERROR: can't load file with info on chemical elements" << std::endl;
    return 1;
  }

//return 0;

  if(!xml_file)
    {
      std::cerr << "ERROR: opening XML file: " << std::endl;
      return 1;
    }

  std::ofstream xml_new("rprocess3.xml");
  std::ofstream xml_to_table("xml2table.txt");


  xml_document doc;

  xml_parse_result res = doc.load(xml_file);

  if(!res)
    {
      std::cerr << "ERROR: " << res.description() << std::endl;
      char * buffer = new char[15];
      xml_file.seekg(res.offset);
      xml_file.read(buffer,15);
      //      fragment= xml_file.substr(res.offset, res.offset+10);
      std::cerr << "Error offset: " << res.offset << " (error at [..." << buffer << "]\n\n";
      delete[] buffer;
      return 1;
    }

// to-do:
// use identified isotopes to check if are in correct range of Z, N
//    ++ will require parameterization from excel
//    ++ add Zlim, Nlim as hard coded global variables 
//     - (then make them command line args?)   
// if so, find out how to write entry to new database
// add counter to find out how many papers filtered out
// check that logic is fine with more than one kewrod tag
// 

  int Nnsr=0;
  int Nrproc= 0;
  bool is_rproc;

  for(  xml_node nsr_entry = doc.first_child(); nsr_entry; nsr_entry = nsr_entry.next_sibling()){
    

    Nnsr++; is_rproc= false;

    std::string description;
    std::ostringstream isotopes;

    xml_node keyno = nsr_entry.child("keyno");
    std::cout << "\n keyno= " << keyno.text().get() << std::endl;

    xml_node title = nsr_entry.child("title");
    xml_node doi = nsr_entry.child("doi");
    xml_node refrence = nsr_entry.child("refrence");

    xml_object_range<xml_named_node_iterator> keywords = nsr_entry.children("keywords");
    xml_named_node_iterator kw;

    //std::cout << "\n keywords!:\n";
    for(kw = keywords.begin(); kw != keywords.end(); kw++){
      description = kw->text().get();
      //  std::cout << description << std::endl;

      std::size_t pos, pos2;
      int Nisotopes = 0;
      std::string mass, element;
      int A, Z;

      std::cout << " +++++ isotopes: ";
      pos = description.find("{+");
      while( pos != std::string::npos){

	pos2= description.find('}',pos+2); //find closing bracket '}'
	mass = description.substr(pos+2, pos2-pos-2);

	if(is_digits(mass) && (pos2-pos-2)>0 ){
	  //std::cout << mass << ", "; 

	  if(!get_A(mass, A)){
	    std::cerr << "\nERROR getting isotope mass from " << mass << std::endl;
	    return 1;
	  }
	  else Nisotopes++;
    
	  char a, b;
	  //get the two characters after {+A} to read the element for the isotope
	  //or conclude that the element will appear later on...
	  if(pos2< (description.size() - 1) ){ 
	    a= description[pos2+1];
	    if(pos2 < (description.size() - 2) ){ 
	      b= description[pos2+2];
	    }
	    else b='*';
	  }
	  else {
	    a='*'; b='*';
	  }
	  //convert data into an element name, and obtain Z
	  if(get_element(a,b,element,Z)){
	    if( is_interesting(Z, A)){
	      is_rproc= true;
	      std::cout << "[[ " <<A << element << "  ]], "; 
	      isotopes << A << element << " ";
	    }
	    //else  std::cout << "(" <<A << element << "), ";

	  }
	  
	}
	//else std::cout << " (" << mass << "), ";
	
	pos = description.find("{+",pos2);
      } //loop over description of each keyword
      // if(is_rproc) std::cout << description << std::endl;

      //      std::cout << "\n ---- Found " << Nisotopes << "!"<<std::endl; 
      std::cout << "\n --------------------------------------\n " <<std::endl; 
    } //loop over keywords
    
    if(is_rproc){

      Nrproc++;  //count how many papers have useful information
      std::cout  << "\n" << title.text().get() << std::endl;

      xml_to_table << keyno.text().get() << "\t";
      xml_to_table << refrence.text().get() << "\t";
      xml_to_table << title.text().get() << "\t";
      xml_to_table << isotopes.str() << "\t";
      xml_to_table << "https://doi.org/"<<doi.text().get() << "\n";

      //write NSR entry to new database
      nsr_entry.print(xml_new, "");
      //xml_new << "\n";
    }

  } //loop over nsr-entries


  std::cout << "\n\nNumber of entries in database: " << Nnsr << std::endl;
  std::cout << "Number of r-process papers?: " << Nrproc << std::endl;


}

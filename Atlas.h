#ifndef DEF_Atlas
#define DEF_Atlas


#include <string>

template <typename InputImagePointerType> 
struct Atlas { 

  std::string name;
  InputImagePointerType image;
  InputImagePointerType labels;
  InputImagePointerType weights; 
}; 

#endif

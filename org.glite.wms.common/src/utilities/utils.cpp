/* **************************************************************************
*  filename  : utils.cpp
*  author    : Alessandro Maraschini <alessandro.maraschini@datamat.it>
*  copyright : (C) 2002 by DATAMAT
***************************************************************************/
#include "utils.h"
#include <string>
#include <stdio.h>

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

/******************************************************************
 method :   isInt
 field: integer & string class
 check if str is an integer
*******************************************************************/
int isInt (const string& str)
{
   int len, bCode,eCode;
   char code ;
   const char * strCh ;
   len = str.size() ;
   strCh = str.c_str();
   bCode= '0';
   eCode= '9';
   for (int i=0; i<len; i++){
      code=strCh[i];
      if ( (code < bCode) || (code >eCode) )  //It's not a digit number
         return 1;
   }
   return 0;
}


/******************************************************************
 methods :   toInt , toHex
 field: integer & string class
 Converts a string str to an Integer value sum
 returns 0/1 ---> success/failure
        STRING--->INTEGER
*******************************************************************/
int toHex (const string& str,int &sum)
{
   int len, bCode,eCode,dec=1;
   char code ;
   const char * strCh ;

   len = str.size() ;
   strCh = str.c_str();
   bCode= '0';
   eCode= '9';
   sum=0;
   for (int i=0; i<len; i++)
   {
      dec=1;
      for (int j=0; j<len-i-1 ; j++)
         dec = 0x10 * dec;

      code= strCh[i];
      //cout << code << " checked: \n";
      if ( (code >= bCode) && (code <=eCode) ){     //It's  a digit number
         sum = sum + dec * (code-bCode) ;
         //cout << dec <<" Int added\n" ;
      }
      else if  ( (code >= 'A') && (code <= 'F') ){ //It's  a hex allowed char
         sum = sum + dec * (code- 'A' +10) ;
         //cout << dec << " Hex Added\n" ;
      }
      else {
         return 1;
         //cout << sum << " Azz  Added\n" ;
      }
   }
   return 0;

};

int toInt (const string& str,int &sum)
{
   int len, bCode,eCode,dec=1;
   char code ;
   const char * strCh ;

   len = str.size() ;
   strCh = str.c_str();
   bCode= '0';
   eCode= '9';
   sum=0;
   for (int i=0; i<len; i++)
   {
      dec=1;
      for (int j=0; j<len-i-1 ; j++)
         dec=10*dec;
      code= strCh[i];
      if ( (code < bCode) || (code >eCode) )  //It's not a digit number
         return 1;
      else
         sum = sum + dec * (code-bCode) ;
   }
   return 0;
}
/******************************************************************
 method :   inTo
 field: integer & string class
 Converts an integer to a string
 INTEGER --->STRING
*******************************************************************/
string inTo(const int number){
   char buffer [CHAR_BUFFER_SIZE] ;
   sprintf (buffer, "%i" , number) ;
   return std::string(buffer);
};

/******************************************************************
 method :   count
 field: string class
 Counts number of sub-string occurrences  in str
*******************************************************************/
int count(const string& strMain,const string& sep){
   int n=0,found ;
   int l;
   //string str=strMain ;
   l = sep.length();
   found = strMain.find(sep) ;
   while (found!= -1){
         found = strMain.find(sep , found + l) ;
         ++n ;
      }
   return n ;
};

/******************************************************************
 method :   split
 field: string class
 Splits a string into a vector of substring
 maxLenght: max dimension of vector (the last item of the vector will be the lasting part)
 trough: if trough == 1 the separator will be copied at the end of the left-substring
*******************************************************************/
vector<string> split(const string& str, const string& sep, int maxLength, int trough)
{
   vector<string> strList ;
   int n,l,end, begin;
   string tmpStr=str ;
   l= sep.length();
   // Optional trough variable indicates that the separator
   // string remain in the array string
   if (trough) trough = l ;
   n = count(tmpStr,sep);
   if (n>maxLength)  n = maxLength ;
   //n (or maxLength) is the number of separator to be found
   begin = 0;
   //cout << "->"<< str << " <--------> " << sep << ":\n";
   end = tmpStr.find(sep);
   for (int i=0; i<n; i++)
   {
      //cout << end << "<---END\n";
      strList.push_back(  tmpStr.substr(begin,end +trough - begin)  ) ;
      //cout << "##"<< tmpStr.substr(begin,end + trough - begin)<<"##\n" ;
      begin = end + l ;
      end= tmpStr.find(sep, begin) ;
      //tmpStr=tmpStr.substr(end+l, str.size()   );
   }
   //cout << end << ">beg>end>" << tmpStr.length() ;
   strList.push_back(tmpStr.substr(begin, tmpStr.length() - begin ) ) ;
   //cout << "||"<< tmpStr.substr( begin,tmpStr.length() - begin  )<<"||\n" ;
   return strList ;

};

/******************************************************************
 method :   sp
 field: string class
 used to create a checkString
*******************************************************************/
string sp(const string& separator)
  {
   const string sep = "_SEP_"  ;
   return sep + separator + sep ;
  }

/******************************************************************
 method :   checkFormat
 field: string class
 Chek the format of a string
 The format should be given as:
       TYPE+sep+SEPARATOR+sep+TYPE+sep+SEPARATOR+sep+ TYPE +...
 Returns: 0 No Error
          1 Error Found
*******************************************************************/
int checkFormat (const string& format, const string& str)
{
   string  type , separator, tmpStr=str ;
   vector<string> form ;
   int len, pos, i;
   const string sep = "_SEP_"  ;
   const string sepStr =  "$STR$"  ;
   const string sepInt =  "$INT$"  ;
   form =split (format,sep);
   len=form.size();
   for (i =1;i<len;i=i+2)  {
      separator = form[i];
      pos = tmpStr.find(separator);
      if (pos ==-1)
         //No required separator found
         return 1  ;
      else
         //Required Separator successfully found
         type   =  tmpStr.substr (0, pos);
         tmpStr =  tmpStr.substr( pos+separator.size() , tmpStr.size()  );
         if (form[i-1] == sepInt)
            {
             pos = isInt(type);
             if (pos)
                return 1;
            }
      }
    type   =  tmpStr ;
    if (form[i-1] == sepInt)
       {
        pos = isInt(type);
        if (pos)
          return 1;
       }
   return 0 ;
}

/******************************************************************
 *  method :   replace
 *  field: string class
 *  Replace "what" value with "with" value
 *******************************************************************/
void
replace(string& where, const string& what, const string& with)
{ 
  size_t fpos = where.find(what);
  
  while (fpos  != string::npos) {
    where.replace(fpos, what.length(), with);
    fpos = where.find(what, fpos + what.length() + 1);
  }
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

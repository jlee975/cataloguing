#ifndef SUBFIELD_CODE_H
#define SUBFIELD_CODE_H

#include <string_view>

namespace marc
{
enum class subfield_code : unsigned char { invalid, exclamation, quote, pound, dollar, percent, ampersand, single_quote, left_paren, right_paren, asterisk,
                plus, comma, minus, period, foreslash, digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8,
                digit9, colon, semicolon, leftangle, equals, rightangle, question_mark, A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
                leftbracket,backslash,rightbracket,caret,underscore,backtick,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,leftbrace,
                rightbrace,tilde, num_subfield_codes};
std::string_view to_string(subfield_code value);
subfield_code ascii_to_subfield_code(unsigned char value_);
}

#endif // SUBFIELD_CODE_H

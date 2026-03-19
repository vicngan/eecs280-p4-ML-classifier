#include<iostream>
#include <set>
#include "csvstream.hpp"

class Classifier {
public: 
    //public function 
private:
    //parameters for the classifier

    //denominator of the Log-Prior P(C)
    int total_posts;
    std::set<std::string> vocabulary;

    //hashmap (key:value)
    std::map<std::string, int> labeled_posts;
    std::map<std::string, int> word_posts;

    //nested map (key:label, value:map<word, count>)
    std::map<std::string, std::map<std::string , int>> label_word_counts; 
}
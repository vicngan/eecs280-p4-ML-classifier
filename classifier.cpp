#include<iostream>
#include <set>
#include <vector>
#include "csvstream.hpp"

class Classifier {
public: 
    //initialize total to 0 
    Classifier() : total_posts(0) {}

private:
    //parameters for the classifier

    //denominator of the Log-Prior P(C)
    int total_posts;
    std::set<std::string> vocabulary;

    //hashmap (key:value)
    std::map<std::string, int> labeled_posts; //use as numerator of P(C) and denominator of P(w|C)
    std::map<std::string, int> word_posts; //Log-Likelihood P(w|C) numerator

    //nested map (key:label, value:map<word, count>)
    std::map<std::string, std::map<std::string , int>> label_word_counts; 
};

//helper function to split strings 
std::set<std::string> unique_words(const std::string &str) {
    std::istringstream source(str); 
    std::set<std::string> words; 
    std::string word; 
    while (source >> word) { 
        words.insert(word); 
    } 
    return words;
}

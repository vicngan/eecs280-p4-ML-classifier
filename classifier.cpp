#include<iostream>
#include <set>
#include <vector>
#include "csvstream.hpp"

class Classifier {
public: 
    //initialize total to 0 
    Classifier() : total_posts(0) {}

    void train(csvstream &is){
        std::map<std::string, std::string> row;

        //iterate through each post 
        while (is >>row){
            std::string label = row["label"];
            std::string text = row["content"];

            //increment total post count 
            total_posts++;

            //increment count for label 
            labeled_posts[label]++;

            //get unique words 
            std::set<std::string> words = unique_words(text);

            //update word counts 
            for (const std::string &word : words){
                vocabulary.insert(word); //add to global vocab list 
                word_posts[word]++; //increment global word count 
                label_word_counts[label][word]++; //increment word count for specific label 

            }

        }
    }

private:
    //parameters for the classifier

    //denominator of the Log-Prior P(C)
    int total_posts;
    std::set<std::string> vocabulary;

    //hashmap (sorted key:value)
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

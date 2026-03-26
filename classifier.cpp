}
#include<iostream>
#include <sstream> 
#include <map>  
#include <set>
#include <vector>
#include <cmath>
#include <limits>
#include <string>
#include "csvstream.hpp"

std::set<std::string> unique_words(const std::string &str);

class Classifier {
public: 
    //getter function 
    int get_total() const{
        return total_posts;
    }
    int get_vocab_size() const{
        return vocabulary.size();
    }

    //initialize total to 0 
    Classifier() : total_posts(0) {}

    void train(csvstream &is, bool debug = false){
        if (debug) std::cout << "training data: \n";
        std::map<std::string, std::string> row;

        //iterate through each post 
        while (is >>row){
            std::string label = row["tag"];
            std::string text = row["content"];

            //print if debug is true
            if (debug){
                std::cout << "  label = " << label << ", content = " << text << "\n";
            }

            //increment total post count 
            total_posts++;

            //increment count for label 
            labeled_posts[label]++;

            //get unique words
            //split a post's content into unique tokens using whitespace and return a set 
            std::set<std::string> words = unique_words(text);

            //update word counts 
            for (const std::string &word : words){
                vocabulary.insert(word); //add to global vocab list 
                word_posts[word]++; //increment global word count 
                label_word_counts[label][word]++; //increment word count for specific label 

            }

        }
    }
    void print_debug() const {
    std::cout << "classes:\n";
    for (const auto &entry : labeled_posts) { //loop var is a read-only reference to each item, not a copy 
        const std::string &label = entry.first; //key
        int count = entry.second; //value 
        std::cout << "  " << label << ", " << count << " examples, log-prior = "
                  << log_prior(label) << "\n";
    }

    std::cout << "classifier parameters:\n";
    for (const auto &label_entry : label_word_counts) {
        const std::string &label = label_entry.first;
        const std::map<std::string, int> &words = label_entry.second;
        for (const auto &word_entry : words) {
            const std::string &word = word_entry.first;
            int count = word_entry.second;
            std::cout << "  " << label << ":" << word << ", count = " << count
                      << ", log-likelihood = " << log_likelihood(label, word) << "\n";
        }
    }
    std::cout << "\n";
}

    //main prediction; return a pair: {winning label, score}
    std::pair<std::string, double> predict(const std::set<std::string> &test)const {
        std::string best_label; 
        double max_score = -std::numeric_limits<double>::infinity(); //start with the smallest value (neg infinity)

        //loop through every learned label 
        for (const auto &entry : labeled_posts) {
            const std::string &label = entry.first;
            double current_score = calculate_score(label, test);
            if (current_score > max_score){
                max_score = current_score;
                best_label = label;
            }
        }
        return {best_label, max_score};
    }

private:
    //calculate total score for specific label 
    double log_prior(const std::string &label) const {
        return std::log((double)labeled_posts.at(label)/total_posts);
    }
    double log_likelihood(const std::string &label, const std::string &word) const {
    auto label_it = label_word_counts.find(label);
    if (label_it != label_word_counts.end()) {
        auto word_it = label_it->second.find(word);
        if (word_it != label_it->second.end()) {
            return std::log((double)word_it->second / labeled_posts.at(label));
        }
    }
    auto gw = word_posts.find(word);
    if (gw != word_posts.end()) {
        return std::log((double)gw->second / total_posts);
    }
    return std::log(1.0 / total_posts);
}
    double calculate_score(const std::string &label, const std::set<std::string> &words) const {
        double score = log_prior(label);
        for (const std::string &word : words){
            score += log_likelihood(label, word);
        }
        return score;
    }

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
//main function implementation
int main(int argc, char *argv[]) {
    std::cout.precision(3);
    bool debug = false;
    

    if (argc != 2 && argc != 3){
        std::cout << "Usage: main.exe TRAIN_FILE.csv TEST_FILE.csv [--debug]\n"; 
        return -1;
    }

    //uses the current index as test_file and increment arg_idx; text_file checks whether another argument is available, 
    //if not, test_file becomes and empty string 
    std::string train_file = argv[1];
    std::string test_file = argv[2];

    if (argc == 4 && std::string(argv[3]) == "--debug") {
        debug = true;
    }

    try{
        //open train file and train 
        csvstream train_stream(train_file);
        Classifier classifier;

        classifier.train(train_stream, debug);
        std::cout << "trained on " << classifier.get_total() << " examples\n";
        std::cout << "vocabulary size = " << classifier.get_vocab_size() << "\n";


        if (debug) {
            std::cout << "\n";
            classifier.print_debug();
        }
           std::cout << "\n";

            //open testing file 
            csvstream test_stream(test_file);

            int correct_predict = 0;
            int total_test_posts = 0;
            std::map<std::string, std::string>row;

            std::cout << "test data:\n"; 

            //loop through each row in the test file 
            while(test_stream >> row){
                std::string label = row["tag"];
                std::string text = row["content"];

                //extract unique words 
                std::set<std::string> test_words = unique_words(text);
                
                //prediction 
                std::pair<std::string, double> prediction = classifier.predict(test_words);

                if(prediction.first == label){
                    correct_predict++;
                }
                total_test_posts++;

                //print result per post 
                std::cout << "  correct = " << label << ", predicted = " << prediction.first << ", log-probability score = " << prediction.second << "\n";
                std::cout << "  content = " << text << "\n\n";
            }
            //print performance summary 
    std::cout << "performance: " << correct_predict << " / " << total_test_posts << " posts predicted correctly\n";
    }
    catch (const csvstream_exception &e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

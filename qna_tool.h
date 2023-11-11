#pragma once
#include <iostream>
#include <fstream>
#include "Node.h"
#include "dict.h"
#include "search.h"

using namespace std;

class QNA_tool {

private:

    // You are free to change the implementation of this function
    void query_llm(string filename, Node* root, int k, string API_KEY, string question);
    // filename is the python file which will call ChatGPT API
    // root is the head of the linked list of paragraphs
    // k is the number of paragraphs (or the number of nodes in linked list)
    // API_KEY is the API key for ChatGPT
    // question is the question asked by the user

    // You can add attributes/helper functions here

public:
    /* Please do not touch the attributes and
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    
    QNA_tool(); // Constructor
    ~QNA_tool(); // Destructor

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);
    // This function is similar to the functions in dict and search 
    // The corpus will be provided to you via this function
    // It will be called for each sentence in the corpus

    Node* get_top_k_para(string question, int k);
    // This function takes in a question, preprocess it
    // And returns a list of paragraphs which contain the question
    // In each Node, you must set: book_code, page, paragraph (other parameters won't be checked)

    std::string get_paragraph(int book_code, int page, int paragraph);
    // Given the book_code, page number, and the paragraph number, returns the string paragraph.
    // Searches through the corpus.

    void query(string question, string filename);
    // This function takes in a question and a filename.
    // It should write the final answer to the specified filename.

    /* -----------------------------------------*/
    /* Please do not touch the code above this line */

    class my_ds1
    {
    private:
        /* data */
    public:
        Dict dictionary;
        int score = 0;
        int b_no;
        int p_no;
        int para_no;
        my_ds1();
        my_ds1(int b, int p, int para);
        ~my_ds1();
    };

    class my_ds2
    {
    private:
        /* data */
    public:

        int size, capacity;
        vector<my_ds1>* books;
        my_ds2(/* args */);
        ~my_ds2();
    };
    
    vector<my_ds2> library;
    
    vector<pair<string, double>> score_words(string question);

    struct new_user_defined_enhanced_struct {
        int book_no;
        int page_no;
        int para_no;

        double score;
    };
    
    vector<new_user_defined_enhanced_struct> built_over_other_build_struct(vector<pair<string, double>> );

};
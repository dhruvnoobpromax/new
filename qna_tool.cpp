#include <assert.h>
#include <sstream>
#include "qna_tool.h"

using namespace std;

QNA_tool::my_ds1::my_ds1()
{
    b_no = -1;
    p_no = -1;
    para_no = -1;
}

QNA_tool::my_ds1::my_ds1(int b, int p, int para)
{
    b_no = b;
    p_no = p;
    para_no = para;
}

QNA_tool::my_ds1::~my_ds1()
{
}

QNA_tool::my_ds2::my_ds2()
{
    book = new vector<my_ds1>[1024];
}

QNA_tool::my_ds2::~my_ds2()
{
    delete[] book;
}

QNA_tool::QNA_tool()
{
    library = vector<my_ds2>();
    library.reserve(128);
}

QNA_tool::~QNA_tool()
{
    // Implement your function here
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence)
{

    if (library.size() < book_code)
    {
        library.resize(2 * book_code);
    }

    if (library[book_code].book->size() <= page)
    {
        int k = library[book_code].book->size();

        vector<my_ds1> *new_pages = new vector<my_ds1>[2 * page];

        for (int i = 0; i < k; i++)
        {
            new_pages[i] = library[book_code].book[i];
        }

        delete[] library[book_code].book;
        library[book_code].book = new_pages;
    }

    if (library[book_code].book[page].size() < paragraph)
    {
        library[book_code].book[page].resize(2 * paragraph);
    }

    library[book_code].book[page][paragraph].dictionary.insert_sentence(book_code, page, paragraph, sentence_no, sentence);

    return;
}

vector<pair<string, int>> QNA_tool::score_words(string question) {
    
    vector<pair<string, int>> result;
    
    string word = "";

    for (int i = 0; i < question.size(); i++) {
        
        char s = question[i];

        if( s == '!' || s == '\"' || s == '[' || s == '.' || s == ',' || s == '?' ||
            s == ']' || s == '(' || s == ')' || s == '@' || s == ';' || s == ':' || 
            s == '\'' || s == '-' || s == ' ')
            {
                if(word.size() != 0)
                {
                    result.push_back(make_pair(word, 0));
                    word = "";
                }

            }
        
        else {
            
            if(s >= 'A' and s <= 'Z') {
                word += (char)(s + 32);
            }

            else {
                word += s;
            }
        
        }
    
    }

    if(word.size() != 0) {
        result.push_back(make_pair(word, 0));
        word = "";
    }
    

    for (int i = 0; i<library.size(); i++) {
        for (int j = 0; j<library[i].book->size(); j++) {
            for (int k = 0; k<library[i].book[j].size(); k++) {
                
                Dict dictionary = library[i].book[j][k].dictionary;
                
                for (int l = 0; l<result.size(); l++) {
                    result[l].second += dictionary.get_word_count(result[l].first);
                }
                
            }
        }
    }

    int find_count = 0;
    ifstream frequency_file("unigram.csv");
    while (find_count < result.size()) {
        
    }

    return result;
}

Node *QNA_tool::get_top_k_para(string question, int k)
{

    return nullptr;
}

void QNA_tool::query(string question, string filename)
{
    // Implement your function here
    std::cout << "Q: " << question << std::endl;
    std::cout << "A: "
              << "Studying COL106 :)" << std::endl;
    return;
}

std::string QNA_tool::get_paragraph(int book_code, int page, int paragraph)
{

    cout << "Book_code: " << book_code << " Page: " << page << " Paragraph: " << paragraph << endl;

    std::string filename = "mahatma-gandhi-collected-works-volume-";
    filename += to_string(book_code);
    filename += ".txt";

    std::ifstream inputFile(filename);

    std::string tuple;
    std::string sentence;

    if (!inputFile.is_open())
    {
        std::cerr << "Error: Unable to open the input file " << filename << "." << std::endl;
        exit(1);
    }

    std::string res = "";

    while (std::getline(inputFile, tuple, ')') && std::getline(inputFile, sentence))
    {
        // Get a line in the sentence
        tuple += ')';

        int metadata[5];
        std::istringstream iss(tuple);

        // Temporary variables for parsing
        std::string token;

        // Ignore the first character (the opening parenthesis)
        iss.ignore(1);

        // Parse and convert the elements to integers
        int idx = 0;
        while (std::getline(iss, token, ','))
        {
            // Trim leading and trailing white spaces
            size_t start = token.find_first_not_of(" ");
            size_t end = token.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos)
            {
                token = token.substr(start, end - start + 1);
            }

            // Check if the element is a number or a string
            if (token[0] == '\'')
            {
                // Remove the single quotes and convert to integer
                int num = std::stoi(token.substr(1, token.length() - 2));
                metadata[idx] = num;
            }
            else
            {
                // Convert the element to integer
                int num = std::stoi(token);
                metadata[idx] = num;
            }
            idx++;
        }

        if (
            (metadata[0] == book_code) &&
            (metadata[1] == page) &&
            (metadata[2] == paragraph))
        {
            res += sentence;
        }
    }

    inputFile.close();
    return res;
}

void QNA_tool::query_llm(string filename, Node *root, int k, string API_KEY, string question)
{

    // first write the k paragraphs into different files

    Node *traverse = root;
    int num_paragraph = 0;

    while (num_paragraph < k)
    {
        assert(traverse != nullptr);
        string p_file = "paragraph_";
        p_file += to_string(num_paragraph);
        p_file += ".txt";
        // delete the file if it exists
        remove(p_file.c_str());
        ofstream outfile(p_file);
        string paragraph = get_paragraph(traverse->book_code, traverse->page, traverse->paragraph);
        assert(paragraph != "$I$N$V$A$L$I$D$");
        outfile << paragraph;
        outfile.close();
        traverse = traverse->right;
        num_paragraph++;
    }

    // write the query to query.txt
    ofstream outfile("query.txt");
    outfile << "These are the excerpts from Mahatma Gandhi's books.\nOn the basis of this, ";
    outfile << question;
    // You can add anything here - show all your creativity and skills of using ChatGPT
    outfile.close();

    // you do not need to necessarily provide k paragraphs - can configure yourself

    // python3 <filename> API_KEY num_paragraphs query.txt
    string command = "python3 ";
    command += filename;
    command += " ";
    command += API_KEY;
    command += " ";
    command += to_string(k);
    command += " ";
    command += "query.txt";

    system(command.c_str());
    return;
}

int main()
{
    QNA_tool a;
    a.insert_sentence(1, 1, 1, 1, "Ji, ");
    a.insert_sentence(1, 1, 1, 2, "Hello, ");
    a.insert_sentence(1, 1, 1, 3, "World!");
    cout << "done" << endl;
}
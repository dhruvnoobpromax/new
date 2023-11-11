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
    books = new vector<my_ds1>[1024];
    capacity = 1024;
    size = 1;
}

QNA_tool::my_ds2::~my_ds2()
{
    delete[] books;
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

void QNA_tool::insert_sentence(int book_code, int page_no, int paragraph, int sentence_no, string sentence)
{

    if (library.size() <= book_code)
    {
        library.resize(2 * book_code);
    }

    if (library[book_code].capacity <= page_no)
    {
        int k = library[book_code].capacity;

        vector<my_ds1> *new_pages = new vector<my_ds1>[2 * page_no];

        for (int i = 0; i < k; i++)
        {
            new_pages[i] = library[book_code].books[i];
        }

        delete[] library[book_code].books;
        library[book_code].books = new_pages;
        library[book_code].capacity = 2 * page_no;
    }

    library[book_code].size = max(library[book_code].size, page_no);

    if (library[book_code].books[page_no].size() <= paragraph)
    {
        library[book_code].books[page_no].resize(2 * paragraph);
    }

    library[book_code].books[page_no][paragraph].dictionary.insert_sentence(book_code, page_no, paragraph, sentence_no, sentence);

    return;
}

vector<pair<string, double>> QNA_tool::score_words(string question) {
    
    vector<pair<string, double>> result;
    
    // strip punctuations
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
    

    for (int i = 1; i<library.size(); i++) {
        for (int j = 1; j<=library[i].size; j++) {
            for (int k = 1; k<library[i].books[j].size(); k++) {
                
                Dict dictionary = library[i].books[j][k].dictionary;
                
                for (int l = 0; l<result.size(); l++) {
                    result[l].second += dictionary.get_word_count(result[l].first);
                }
                
            }
        }
    }

    int find_count = 0;
    
    for (int i = 0; i < result.size(); i++)
    {
        ifstream frequency_file("unigram_freq.csv");
        
        std::string word1;
        std::string countofword;

        getline(frequency_file,word1);

        while (getline(frequency_file,word1,','))
        {
            getline(frequency_file,countofword);

            if (word1 == result[i].first)
            {   
                result[i].second = (result[i].second)/(double)(stod(countofword));                ;
                break;
            }
        }

        frequency_file.close();
    }

    return result;
}

vector<QNA_tool::paragraph_details> QNA_tool::score_paragraphs(vector<pair<string, double>> query_words) {
        
        vector<paragraph_details> result;

        for (int i = 1; i<library.size(); i++) {
            for (int j = 1; j<=library[i].size; j++) {
                for (int k = 1; k<library[i].books[j].size(); k++) {
                    
                    Dict dictionary = library[i].books[j][k].dictionary;

                    paragraph_details para;
                    para.book_no = i;
                    para.page_no = j;
                    para.para_no = k;

                    for (int l = 0; l<query_words.size(); l++) {
                        
                        int count = dictionary.get_word_count(query_words[l].first);

                        para.score += count * query_words[l].second;

                    }

                    result.push_back(para);
                    
                }
            }
        }

        return result;

}

void QNA_tool::heapify(vector<QNA_tool::paragraph_details>& arr, int n, int i) {
    
    int largest = i; // Initialize the root as the largest
    int left = 2 * i + 1; // Index of the left child
    int right = 2 * i + 2; // Index of the right child

    // If the left child is smaller than the root
    if (left < n && arr[left].score > arr[largest].score)
        largest = left;

    // If the right child is smaller than the current largest
    if (right < n && arr[right].score > arr[largest].score)
        largest = right;

    // If the largest is not the root, swap and recursively heapify the affected sub-tree
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void QNA_tool::heapSort(vector<QNA_tool::paragraph_details>& arr, int k) {
    
    int n = arr.size();

    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements from the heap one by one
    for (int i = n - 1; i >= n-k-1; i--) {
        // Swap the root (maximum element) with the last element
        std::swap(arr[0], arr[i]);

        // Call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

Node *QNA_tool::get_top_k_para(string question, int k)
{

    vector<paragraph_details> scored_paragraphs = score_paragraphs(score_words(question));

    heapSort(scored_paragraphs, k);

    Node *root = new Node;
    root->right = nullptr;
    root->left = nullptr;

    Node* temp = root;

    while(temp->right != nullptr) {
        temp = temp->right;
    }

    int n = scored_paragraphs.size();
    for (int i = n-k; i < n; i++) {

        Node* newnode = new Node;
        newnode->left = temp;
        newnode->right = nullptr;
        newnode->book_code = scored_paragraphs[i].book_no;
        newnode->page = scored_paragraphs[i].page_no;
        newnode->paragraph = scored_paragraphs[i].para_no;
        newnode->sentence_no = -1;
        temp->right = newnode;
        temp = temp->right;

    }

    Node* temp1 = root->right;
    root->right = nullptr;
    delete root;
    
    return temp1;
}

void QNA_tool::query(string question, string filename)
{
    // Implement your function here
    std::cout << "Q: " << question << std::endl;
    std::cout << "A: "
              << "Studying COL106 :)" << std::endl;
    return;
}

std::string QNA_tool::get_paragraph(int book_code, int books, int paragraph)
{

    cout << "Book_code: " << book_code << " Page: " << books << " Paragraph: " << paragraph << endl;

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
            (metadata[1] == books) &&
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
/*
int main()
{
    QNA_tool a;
    a.insert_sentence(1, 1, 1, 1, "the ");
    a.insert_sentence(1, 1, 1, 2, "the?");
    a.insert_sentence(1, 1, 1, 3, "the,");
    a.insert_sentence(1, 1, 1, 4, "the.");
    a.insert_sentence(1, 1, 1, 5, "the!");
    a.insert_sentence(1, 1, 2, 1, "the-");
    a.insert_sentence(1, 1, 2, 2, "the:");
    a.insert_sentence(1, 1, 2, 3, "the;");
    a.insert_sentence(1, 1, 2, 4, "the[");
    a.insert_sentence(1, 1, 2, 5, "the]");
    a.insert_sentence(1, 1, 3, 1, "fuck(");
    a.insert_sentence(1, 1, 3, 2, "fuck)");
    a.insert_sentence(1, 1, 3, 3, "fuck");
    a.insert_sentence(1, 1, 3, 4, "fuck");
    a.insert_sentence(1, 1, 3, 5, "fuck@");
    a.insert_sentence(1, 1, 4, 6, "is");
    a.insert_sentence(1, 1, 4, 7, "is");
    a.insert_sentence(1, 1, 4, 8, "is");
    a.insert_sentence(1, 1, 4, 9, "is");
    a.insert_sentence(1, 1, 4, 0, "is");
    a.insert_sentence(1, 1, 5, 1, "this");

    string q = "the fuck is your name?";

    // auto ww = a.score_words(q);
    // for (int i = 0; i < ww.size(); i++)
    // {
    //     cout << ww[i].first << " " << ww[i].second << endl;
    // }

    // auto pp = a.score_paragraphs(ww);
    // for (auto p : pp)
    // {
    //     cout << p.book_no << " " << p.page_no << " " << p.para_no << " " << p.score << endl;
    // }

    Node* list = a.get_top_k_para("the fuck", 4);

    Node* temp = list;

    while (temp != nullptr) {
        cout << temp->book_code << " " << temp->page << " " << temp->paragraph << endl;
        temp = temp->right;
    }

    cout << "done" << endl;

    // QNA_tool wasted;
    // vector<QNA_tool::paragraph_details> ass;
    // for (int i = 0; i < 11; i++)
    // {
    //     QNA_tool::paragraph_details tits;
    //     tits.book_no = 1;
    //     tits.page_no = 1;
    //     tits.para_no = i+1;
    //     tits.score = i+1;

    //     ass.push_back(tits);
    // }
    
    // wasted.heapSort(ass,10);
    // cout << "done";

}*/
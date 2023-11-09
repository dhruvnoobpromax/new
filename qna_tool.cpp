#include <assert.h>
#include <sstream>
#include "qna_tool.h"

using namespace std;

QNA_tool::line::line(int bcode,int pg,int para, int sentno, string s)
{
    book_code = bcode;
    page = pg;
    paragraph = para;
    sentence_no = sentno;
    sentence = s; 
}

QNA_tool::QNA_tool()
{
    a = vector<vector<vector<vector<line>>>>();
    // Implement your function here  
}

QNA_tool::~QNA_tool(){
    // Implement your function here  
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    // Implement your function here 
    if (book_code < a.size() -1)
    {
        a.resize(2*book_code);
    }

    if (page < a[book_code].size() - 1)
    {
        a[book_code].resize(2*page);
    }

    if (paragraph < a[book_code][page].size()-1)
    {
        a[book_code][page].resize(2*paragraph);
    }



    line x(book_code,page,paragraph,sentence_no,sentence);

    a[book_code][page][paragraph].push_back(x);
    
    return;
}


vector<int> makelps(std::string pat) {

    vector<int> lps(pat.length());
    lps[0] = 0;
    int len = 0;
    int i =1;

    while (true) {

        if(i == pat.length()) {break;}

        if(pat[len] == pat[i]) {

            len++;
            lps[i] = len;
            i++;
        }

        else if (pat[len] != pat[i]) {

            if(len != 0)
            {
                len = lps[len-1];
            }

            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

vector<int> kmp (string txt, string pat) {

    int i = 0;
    int j = 0;
    int M = pat.length();
    vector<int> offs;
    vector<int> lps = makelps(pat);

    while (true) { if(i > txt.length()) {break;}
    
        if(j == M)
        {
            offs.push_back(i - M);
            j = lps[j-1];
        }
        if(txt[i] == pat[j])
        {  
            i++;
            j++;
        }

        else
        {
            if(j>0)
            {
                j = lps[j-1];
            }

            else
            {
                i++;
            }
        }
    }

    return offs;
    
}

class linef
{
    public:
    int bookno;
    int pageno;
    int parano;
    double score;

    linef(int b,int p, int pa,double s)
    {
        bookno = b;
        pageno = p;
        parano = pa;
        score = s;

    }
};


std::pair<vector<QNA_tool::line>,int> idk_wtf_isthis(string q,vector<vector<vector<vector<QNA_tool::line>>>> a)
{
    vector<QNA_tool::line> paracount;
    int total = 0;
    for (int i = 1; i < a.size(); i++)
    {
        for (int j = 1; j < a[i].size(); j++)
        {
            for (int k = 1; k < a[i][j].size(); k++)
        {
            int paramatches = 0;
            for (int l = 0; l < a[i][j][k].size(); i++)
            {
                vector<int> m = kmp(a[i][j][k][l].sentence,q);
                paramatches += m.size();
                total += m.size();
            }
            QNA_tool::line qq(i,j,k,paramatches,"");
            paracount.push_back(qq);
        }
        }
        
    }

    std::pair aq = make_pair(paracount,total);
    return aq;
}

vector<linef> kk (string q,vector<vector<vector<vector<QNA_tool::line>>>> a,int scoreinwholecorpus)
{
    pair kwk = idk_wtf_isthis(q,a);
    int scoreincorpus = kwk.second;

    double score = (double(scoreincorpus)+1)/(double(scoreinwholecorpus) +1);

    vector<linef> as;
    for (int i = 0; i < kwk.first.size(); i++)
    {
        double qqq = double(kwk.first[i].sentence_no)*score;
        linef aqq(kwk.first[i].book_code,kwk.first[i].page,kwk.first[i].paragraph,qqq);
        as.push_back(aqq);
    }
    
    return as;
}


vector<string> ppr(std::string pat) {

    vector<string> ret;
    string word = "";
    for (int i = 0; i < pat.size(); i++) {
        char s = pat[i];

        if( s == '!' || s == '\"' || s == '[' || s == '.' || s == ',' || s == '?' ||
            s == ']' || s == '(' || s == ')' || s == '@' || s == ';' || s == ':' || 
            s == '\'' || s == '-' || s == ' ')
            {
                if(word.size() !=0)
                {
                    ret.push_back(word);
                    word = "";

                }

            }
        
        else {
            if(s - 'A' >=0 and s - 'Z' <= 0) {
                word += (char)(s + 32);

            }

            else{word += s;}
        }
    }

    if(word.size() !=0) {
        ret.push_back(word);
        word = "";
    }

    return ret;
    
}


// vector<linef> add(vector<linef> a, vector<linef> b)
// {
//     vector<linef> q;
//     for (int i = 0; i < a.size(); i++)
//     {
//         linef qq(a[i].bookno,a[i].pageno,a[i].parano,a[i].score+b[i].score);
//         q.push_back(qq);
//     }

//     return q;
// }

Node* QNA_tool::get_top_k_para(string question, int k) {
    // Implement your function here
    //pair b = idk_wtf_isthis(question,a);
    vector<string> w = ppr(question);
    vector<vector<linef>> e;
    for (int i = 0; i <w.size(); i++)
    {
        int scoreinwholecorpus;
        e.push_back(kk(w[i],a,scoreinwholecorpus));
    }

    for (int i = 0; i < e.size(); i++)
    {
        double fsc = 0;
        for (int j = 0; j < e.size(); j++)
        {
            //
        }
        
    }
    
    

   
    
    
    return nullptr;
}

void QNA_tool::query(string question, string filename){
    // Implement your function here  
    std::cout << "Q: " << question << std::endl;
    std::cout << "A: " << "Studying COL106 :)" << std::endl;
    return;
}

std::string QNA_tool::get_paragraph(int book_code, int page, int paragraph){

    cout << "Book_code: " << book_code << " Page: " << page << " Paragraph: " << paragraph << endl;
    
    std::string filename = "mahatma-gandhi-collected-works-volume-";
    filename += to_string(book_code);
    filename += ".txt";

    std::ifstream inputFile(filename);

    std::string tuple;
    std::string sentence;

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the input file " << filename << "." << std::endl;
        exit(1);
    }

    std::string res = "";

    while (std::getline(inputFile, tuple, ')') && std::getline(inputFile, sentence)) {
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
        while (std::getline(iss, token, ',')) {
            // Trim leading and trailing white spaces
            size_t start = token.find_first_not_of(" ");
            size_t end = token.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) {
                token = token.substr(start, end - start + 1);
            }
            
            // Check if the element is a number or a string
            if (token[0] == '\'') {
                // Remove the single quotes and convert to integer
                int num = std::stoi(token.substr(1, token.length() - 2));
                metadata[idx] = num;
            } else {
                // Convert the element to integer
                int num = std::stoi(token);
                metadata[idx] = num;
            }
            idx++;
        }

        if(
            (metadata[0] == book_code) &&
            (metadata[1] == page) &&
            (metadata[2] == paragraph)
        ){
            res += sentence;
        }
    }

    inputFile.close();
    return res;
}

void QNA_tool::query_llm(string filename, Node* root, int k, string API_KEY, string question){

    // first write the k paragraphs into different files

    Node* traverse = root;
    int num_paragraph = 0;

    while(num_paragraph < k){
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

// int main()
// {
//     QNA_tool a;
//     a.insert_sentence(1,1,1,1,"hello");
//     cout << "gg";
// }
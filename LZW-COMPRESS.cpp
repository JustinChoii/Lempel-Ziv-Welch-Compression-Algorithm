#include <iostream>
#include <limits>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

using namespace std;


constexpr std::size_t BITS_PER_BYTE = std::numeric_limits<unsigned char>::digits;

using bits_in_byte = std::bitset<BITS_PER_BYTE>;
using byte = unsigned char;



// Tokens keep track of doubles and triples. Doubles = len, offset. Triples = string reference.
class Token {
    public:
        int len,offset,code,strlen;
        string chars;
        bool isTriple;
        
        // double token
        Token(int l, int o) {
            len = l;
            offset = o;
            code = 0;
            strlen = 0;
            isTriple = false;
       }

       // triple token 
       Token(int c, int l, string characters) {
           len = 0;
           offset = 0;
           code = c;
           strlen = l;
           chars = characters;
           isTriple = true;
       }
};

bool parser(int argc, char** argv, int *N, int *L, int *S, string *filename, bool *parameters_given, bool *filename_given) {
    bool N_exist = false;
    bool L_exist = false;
    bool S_exist = false;
    bool error = false;
    if (argv[argc-1][0] == '-' && argv[argc-1][2] == '=') {
        *filename_given = false;
    }
    for (int i = 1; i < argc; i++) { // parse through all the arguments after the LZ funtion call
        string temp = argv[i];
        if (argv[i][0] == '-' && argv[i][2] == '=') {
            if (argv[i][1] == 'N') { 
                N_exist = true;
                int parameter = atoi(temp.substr(temp.find("=") + 1).c_str());
                // std::cout << "N = " << parameter << std::endl;
                if (parameter < 9 || parameter > 14) {
                    std::cout << "Error: Parameter N is not in the range 9-14." << std::endl;
                    error = true;
                    *N = 11;
                }
                else {
                    *N = parameter;
                }

            }
            else if (argv[i][1] == 'L') {
                L_exist = true;
                int parameter = atoi(temp.substr(temp.find("=") + 1).c_str());
                // std::cout << "L = " << parameter << std::endl;
                if (parameter < 3 || parameter > 4) {
                    std::cout << "Error: Parameter L is not in the range 3-4." << std::endl;
                    error = true;
                    *L = 3;
                }
                else {
                    *L = parameter;
                }
            }
            else if (argv[i][1] == 'S') {
                S_exist = true;
                int parameter = atoi(temp.substr(temp.find("=") + 1).c_str());
                // std::cout << "S = " << parameter << std::endl;
                if (parameter < 1 || parameter  > 5) {
                    std::cout << "Error: Parameter S is not in the range 1-5." << std::endl;
                    error = true;
                    *S = 3;
                }
                else {
                    *S = parameter;
                }
            }
            *parameters_given = true;
        }
        else {
            *filename = temp;
            std::cout << "Filename is: " << *filename << std::endl;
            *filename_given = true;
        }
    }
    if (N_exist == false) {
        *N = 11;
    }
    if (L_exist == false) {
        *L = 4;
    }
    if (S_exist == false) {
        *S = 3;
    }
    if (error == false) {
    	std::cout << "Failed to Parse" << std::endl;
        return false;
    }
}


// Receive a file, and open it in binary.
std::string read_bits_from_file(std::string path_to_file) {
    std::ifstream file;
    std::string string_of_bits = "";
    file.open(path_to_file, std::ios::binary);
    char c;
    while (file.get(c)) {
        string_of_bits += bits_in_byte(byte(c)).to_string();
    }
    return string_of_bits;
}



// Receive a string of bits (this will be the file converted into binary).
// Encode the bits using Lempel-zev compression readable by the decoder/expand.
void encode(std::string string_of_bits, int N, int L, int S) {
    int window_size = ((int)pow(2,N))*BITS_PER_BYTE;
    int look_ahead_size = ((int)pow(2,L))*BITS_PER_BYTE;
    int search_size = ((int)pow(2,S-1))*BITS_PER_BYTE;

    std::vector<Token> token_vector;
    int window_position = window_size;
    int current_position = 0;
    bool currently_matching;for(;;){}

    // while we haven't reached the end of our string of bits
    while (window_position < string_of_bits.length()) {
        // search buffer 
        std::string search_buffer = string_of_bits.substr(current_position, search_size);
        // look ahead buffer
        std::string look_ahead_buffer = string_of_bits.substr((current_position+search_size),window_size-look_ahead_size);
        // current position in string of bits
        current_position += BITS_PER_BYTE;
        // current position of window
        window_position += current_position;
        int buffer = 0;

        int search_buffer_index = search_size-1; 
        int lookahead_buffer_index = 0;
        int current_match_len;
        while (search_buffer_index > 0) {
            if (search_buffer.at(search_buffer_index) == look_ahead_buffer.at(lookahead_buffer_index)) {
                current_match_len += 1;
                search_buffer_index -= 1;
                lookahead_buffer_index += 1;
            } else {
                if (current_match_len > 1) {
                    Token(current_match_len,search_size-search_buffer_index);
                } else {
                    Token(look_ahead_buffer.at(lookahead_buffer_index+1), current_match_len, look_ahead_buffer);
                }
                current_match_len = 0;
                search_buffer_index -= 1;
                lookahead_buffer_index += 1;
            }
            
        }

        cout << "current search buffer: " << search_buffer << " current look_ahead_buffer " << look_ahead_buffer << endl;
        
    }

}





// N - number of bits to encode window offset and also determines window size W
// L - number of bits to encode match length and also determines the lookahead buffer size F
// S - number of bits to encode length of literal string
// number of bits in char is 8, numbers ranging from 0-255


// to do stdin do read_bits_from_file("")
// to stop reading from input press control + D
int main(int argc, char** argv) {
	clock_t timer;
	timer = clock();

    string filename;
    int N;
    int L;
    int S;
    int bitCount = 24;
    bool parameters_given = false;
    bool filename_given = false;
    std::string stringbits;
    string input;
    bool parsed = true;

    // if parsed fails, then exit the program.
    // parse all the arguments and return them based off user input.
    parsed = parser(argc,argv, &N, &L, &S, &filename, &parameters_given, &filename_given);
    if (parsed == false){
    	std::exit;
    }
    std::cout << "N = " << N << "; L = " << L << "; S = " << S << std::endl;
    
    if (!parameters_given) {
        std::cout << "No parameters were given." << std::endl;
        N = 11;
        L = 3;
        S = 4;
    }
    if (!filename_given) {
        
        std::cout << "Filename is not given. Please input a message to be encoded/decoded." << std::endl;
        getline(std::cin, input);
        std::cout << input << std::endl;
         
    } 
    //encoding
    cout << endl;
    cout << "Beginning encoding: " << endl;
    encode(stringbits,N,L,S);

    return 0;
}

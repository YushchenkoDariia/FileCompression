#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

// Structure for a Huffman Tree node
struct Node {
    char ch;    
    int freq;   
    Node* left;
    Node* right;

    // Constructor for the node
    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
};


struct Compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;  
    }
};

// Function to build the Huffman Tree
Node* buildHuffmanTree(map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* node = new Node('\0', left->freq + right->freq);  
        node->left = left;
        node->right = right;
        pq.push(node);
    }

    return pq.top();  
}

// Function to generate Huffman codes from the tree
void generateHuffmanCodes(Node* root, string str, map<char, string>& huffmanCode) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    generateHuffmanCodes(root->left, str + "0", huffmanCode);
    generateHuffmanCodes(root->right, str + "1", huffmanCode);
}

// Function to compress a file using Huffman coding
void compressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!inFile.is_open()) {
        cerr << "Cannot open input file.\n";
        return;
    }

    // Step 1: Count the frequency of each character in the input file
    map<char, int> freqMap;
    char ch;
    while (inFile.get(ch)) {
        freqMap[ch]++;
    }
    inFile.clear();  
    inFile.seekg(0); 

    // Step 2: Build Huffman Tree from the frequency map
    Node* root = buildHuffmanTree(freqMap);

    // Step 3: Generate Huffman codes for each character
    map<char, string> huffmanCode;
    generateHuffmanCodes(root, "", huffmanCode);

    // Step 4: Write the frequency table to the output file (for decompression)
    for (auto pair : freqMap) {
        outFile << pair.first << pair.second << ' ';
    }
    outFile << '\n';

    // Step 5: Compress the input file using Huffman codes
    string encodedStr = "";
    while (inFile.get(ch)) {
        encodedStr += huffmanCode[ch];
    }

    
    bitset<8> bits;
    for (size_t i = 0; i < encodedStr.size(); i += 8) {
        bits = bitset<8>(encodedStr.substr(i, 8));
        outFile.put(static_cast<unsigned char>(bits.to_ulong()));
    }

    inFile.close();
    outFile.close();
}

// Function to decompress a file using Huffman coding
void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!inFile.is_open()) {
        cerr << "Cannot open input file.\n";
        return;
    }

    // Step 1: Read the frequency table from the compressed file
    map<char, int> freqMap;
    char ch;
    int freq;
    while (inFile >> ch >> freq) {
        freqMap[ch] = freq;
    }
    inFile.clear();
    inFile.seekg(1, ios::cur); 

    // Step 2: Rebuild the Huffman Tree from the frequency table
    Node* root = buildHuffmanTree(freqMap);

    // Step 3: Decompress the file using the Huffman tree
    Node* currentNode = root;
    char byte;
    while (inFile.get(byte)) {
        bitset<8> bits(static_cast<unsigned char>(byte)); 

        for (int i = 7; i >= 0; --i) {
            if (bits[i] == 0) currentNode = currentNode->left;
            else currentNode = currentNode->right;

       
            if (!currentNode->left && !currentNode->right) {
                outFile.put(currentNode->ch);
                currentNode = root;
            }
        }
    }

    inFile.close();
    outFile.close();
}


int main() {
    int choice;
    string inputFile, outputFile;

   
    cout << "1. Compress file\n2. Decompress file\nEnter your choice: ";
    cin >> choice;

    if (choice == 1) {
       
        cout << "Enter input file path: ";
        cin >> inputFile;
        cout << "Enter output file path: ";
        cin >> outputFile;
        compressFile(inputFile, outputFile);
    }
    else if (choice == 2) {
        
        cout << "Enter compressed file path: ";
        cin >> inputFile;
        cout << "Enter output file path: ";
        cin >> outputFile;
        decompressFile(inputFile, outputFile); 
    }
    else {
        cout << "Invalid choice!";  
    }

    return 0;
}


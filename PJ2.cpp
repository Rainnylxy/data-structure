#include<iostream>
#include<time.h>
#include<fstream>
#include<map>
#include<vector>
#include<algorithm>
#include<assert.h>
#include<io.h>
#include<dirent.h>
#include<direct.h>
#include<string>
using namespace std;
class HuffmanNode {
public:
	unsigned char ch;
	int freq;
	HuffmanNode* lft;
	HuffmanNode* rig;
public:
	unsigned char get_ch() { return ch; }
	int get_freq() { return freq; }
	HuffmanNode(HuffmanNode* test) {
		this->ch = test->ch;
		this->freq = test->freq;
		this->lft = test->lft;
		this->rig = test->rig;
	}
	HuffmanNode(pair<unsigned char, int>test) {
		this->ch = test.first;
		this->freq = test.second;
		this->lft = NULL;
		this->rig = NULL;
	}
	HuffmanNode() {
		lft = NULL;
		rig = NULL;
		freq = 0;
		ch = 'h';
	}
	bool is_leaf() {
		return (lft == NULL) && (rig == NULL);
	}
};

bool comp(HuffmanNode* a, HuffmanNode* b) {
	return a->freq < b->freq;
}
class HuffmanTree {
public:
	HuffmanNode* root;
	string huffcode[256];
	int* huffcodeBits[256];
	int num;
	HuffmanTree(int* file);
	void get_huff_code(HuffmanNode*& rot, string huff);
	void build_from_code(const unsigned char& ch, const string& code);
	void destroy(HuffmanNode* root);
	HuffmanTree() { root = NULL; }
	~HuffmanTree() { destroy(root); }
};
void HuffmanTree::destroy(HuffmanNode* root) {
	if (root != NULL) {
		destroy(root->lft);
		destroy(root->rig);
		delete root;
		root = NULL;
	}
}
HuffmanTree::HuffmanTree(int* file) {	//create huffmantree
	vector<HuffmanNode*> vec;
	for (int i = 0; i < 256; i++) {
		if (file[i] != 0) {
			pair<unsigned char, int>temp((unsigned char)i, file[i]);
			HuffmanNode* t = new HuffmanNode(temp);
			vec.push_back(t);
		}
		this->huffcodeBits[i] = NULL;
	}
	if (vec.size() == 1) {
		HuffmanNode* last = new HuffmanNode;
		last->lft = new HuffmanNode(vec[0]);
	}
	while (vec.size() != 1) {
		sort(vec.begin(), vec.end(), comp);
		HuffmanNode* last = new HuffmanNode;
		last->lft = new HuffmanNode(vec[0]);
		last->rig = new HuffmanNode(vec[1]);
		last->freq = last->lft->freq + last->rig->freq;
		vec.erase(vec.begin(), vec.begin() + 2);
		vec.push_back(last);
	}
	root = vec[0];
	vec.erase(vec.begin(), vec.begin() + 1);
}

void HuffmanTree::get_huff_code(HuffmanNode*& rot, string huff) {
	if (rot->is_leaf()) {
		huffcode[rot->ch] = huff;
		huffcodeBits[rot->ch] = new int[huff.length()];
		for (int i = 0; i < huff.length(); i++) {
			huffcodeBits[rot->ch][i] = huff[i] - '0';
		}
		num++;
		return;
	}
	else {
		if (rot->lft) {
			get_huff_code(rot->lft, huff + "0");
		}
		if (rot->rig) {
			get_huff_code(rot->rig, huff + "1");
		}
	}
}
void HuffmanTree::build_from_code(const unsigned char& ch, const string& code) {
	if (this->root == NULL) {
		root = new HuffmanNode;
	}
	HuffmanNode* curr = root;
	for (int i = 0; i < code.length(); i++) {
		if (code[i] == '0') {
			if (curr->lft == NULL) {
				curr->lft = new HuffmanNode;
			}
			curr = curr->lft;
		}
		else {
			if (curr->rig == NULL) {
				curr->rig = new HuffmanNode;
			}
			curr = curr->rig;
		}
	}
	curr->ch = ch;
}

class Compress {
public:
	string filename;//Actually, it is path
	int filedt[256] = { 0 };
	Compress(string filename) { this->filename = filename; }
	void compress();
	void files_compress(string name, FILE*& fw);
	void file_compress(FILE*& fw, string filename, FILE* fp);
	void read_file(FILE* fp, long int& length, unsigned char*& data);
	void get_head_data(HuffmanTree* huffmanTree, unsigned char*& dt, long int& index, int size);
	void get_file_data(HuffmanTree* huffmanTree, unsigned char*& dt, long int& index, long int length, unsigned char* data);
};
void Compress::compress() {
	struct stat filedata;
	FILE* fw;
	stat(filename.c_str(), &filedata);
	if (S_ISDIR(filedata.st_mode)) {
		string name = filename + ".lxy";
		fopen_s(&fw, name.c_str(), "wb");
		files_compress(filename, fw);
		fclose(fw);
	}
	else if (S_ISREG(filedata.st_mode)) {
		string afilename = filename.substr(0, filename.rfind("."));
		string cfile = afilename + ".lxy";
		fopen_s(&fw, cfile.c_str(), "wb");
		FILE* fp;
		fopen_s(&fp, filename.c_str(), "rb");
		file_compress(fw, filename, fp);
		fclose(fw);
	}
}
void Compress::get_head_data(HuffmanTree* huffmanTree, unsigned char*& dt, long int& index, int size) {
	unsigned char a = (unsigned char)(size - 1);
	dt[++index] = a;
	int b;
	string* huffcode = huffmanTree->huffcode;
	string temp = "";								
	for (int i = 0; i < 256; i++) {
		int val = 0;
		int len = 0;
		if (huffcode[i][0] != '\0') {
			temp = huffcode[i];
			dt[++index] = (unsigned char)i;
			b = temp.length() / 8;
			if (temp.length() % 8 != 0) {
				b = b + 1;
			}
			dt[++index] = b;
			dt[++index] = 0;
			for (int idx = 0; idx < temp.length(); idx++) {
				val = val << 1;
				if (temp[idx] == '1') {
					val = val | 1;
				}
				len++;
				if (len >= 8) {
					dt[++index] = (unsigned char)val;
					val = 0;
					len = 0;
				}
			}
			if (len > 0) {
				val = val << (8 - len);
				dt[++index] = (unsigned char)val;
				len = 8 - len;
			}
			dt[index - b] = (unsigned char)len;
		}
	}
}
void Compress::read_file(FILE* fp, long int& length, unsigned char*& data) {
	data = new unsigned char[length];
	rewind(fp);
	fread(data, 1, length, fp);
	fclose(fp);
	for (long int i = 0; i < length; i++) {
		filedt[data[i]]++;
	}
}
void Compress::get_file_data(HuffmanTree* huffmanTree, unsigned char*& dt, long int& index, long int length, unsigned char* data) {
	int len = 0;
	int val = 0;
	int** huffcodeBits = huffmanTree->huffcodeBits;
	for (long int i = 0; i < length; i++) {
		int* temp = huffcodeBits[data[i]];
		for (int idx = 0; idx < huffmanTree->huffcode[data[i]].length(); idx++) {
			val = (val << 1) + temp[idx];
			len++;
			if (len >= 8) {
				dt[++index] = (unsigned char)val;
				val = 0;
				len = 0;
			}
		}
	}
	if (len > 0) {
		val = val << (8 - len);
		dt[++index] = (unsigned char)val;
		len = 8 - len;
	}
	dt[++index] = len;
}
void Compress::file_compress(FILE*& fw, string filename, FILE* fp) {
	unsigned char* data;
	long int length;
	assert(fp != NULL);
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	if (length == 0) {
		fclose(fp);
		unsigned char a = (unsigned char)filename.size();
		data = new unsigned char[100];
		length = -1;
		data[++length] = a;
		for (int i = 0; i < filename.size(); i++) {
			data[++length] = filename[i];
		}
		data[++length] = (unsigned char)0;
		fwrite(data, 1, ++length, fw);
		int v = 0;
		fwrite(&v, 8, 1, fw);
		fclose(fw);
		delete[]data;
	}
	else {
		read_file(fp, length, data);
		HuffmanTree tree(filedt);
		string huff = "";
		tree.get_huff_code(tree.root, huff);
		unsigned char* dt = new unsigned char[2000 + 5 * length];
		long int index = -1;
		get_head_data(&tree, dt, index, tree.num);
		get_file_data(&tree, dt, index, length, data);
		unsigned char a = (unsigned char)filename.size();
		fwrite(&a, 1, 1, fw);
		unsigned char* t = new unsigned char[filename.size() + 1];
		for (int i = 0; i < filename.size(); i++) {
			t[i] = filename[i];
		}
		t[filename.size()] = (unsigned char)0;
		fwrite(t, 1, filename.size() + 1, fw);
		fwrite(&(++index), 8, 1, fw);
		fwrite(dt, 1, index, fw);
		delete[]data;
		delete[]dt;
	}
}
void Compress::files_compress(string name, FILE*& fw) {
	int flag = 0;
	struct stat filedata;
	stat(name.c_str(), &filedata);
	if (S_ISDIR(filedata.st_mode)) {
		cout << name << "  files write success!!" << endl;
		struct dirent* file;
		DIR* dp = opendir(name.c_str());
		file = readdir(dp);
		while (file) {
			char file_path[200] = {};
			strcat_s(file_path, name.c_str());
			strcat_s(file_path, "/");
			strcat_s(file_path, file->d_name);
			if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
				stat(file_path, &filedata);
				if (S_ISDIR(filedata.st_mode)) {
					files_compress(file_path, fw);
				}
				if (S_ISREG(filedata.st_mode)) {
					//cout << file_path << " write success!!"<< endl;
					FILE* fp;
					fopen_s(&fp, file_path, "rb");
					file_compress(fw, file_path, fp);
					for (int i = 0; i < 256; i++) {
						filedt[i] = 0;
					}
				}
			}
			if (strcmp(file->d_name, "..") == 0) {
				file = readdir(dp);
				if (file == NULL) {
					cout << " empty files " << endl;
					unsigned char a = (unsigned char)name.size();
					fwrite(&a, 1, 1, fw);
					unsigned char* t = new unsigned char[name.size() + 1];
					for (int i = 0; i < name.size(); i++) {
						t[i] = name[i];
					}
					t[name.size()] = '/';
					fwrite(t, 1, name.size() + 1, fw);
					int v = 0;
					fwrite(&v, 8, 1, fw);
				}
			}
			else {
				file = readdir(dp);
			}
		}
	}
}
class Decompress {
public:
	string filename;
	HuffmanTree* huffmanTree = NULL;
	Decompress(string filename) { this->filename = filename; }
	string to_string(int a);
	void decompress();
	int file_decompress(FILE* fp, int idx);
	void decode_huffman_tree(unsigned char* data, long int& index);
	void write_new_file(string lastname, unsigned char*& data, long int& index, long int length);
};
string Decompress::to_string(int a) {
	string res = "00000000";
	int i = 7;
	while (a > 0) {
		res[i] = a % 2 + '0';
		i--;
		a = a / 2;
	}
	return res;
}

void Decompress::decode_huffman_tree(unsigned char* data, long int& index) {
	int ch = (int)data[++index];
	ch++;
	for (int i = 0; i < ch; i++) {
		unsigned char temp = data[++index];
		unsigned char a = data[++index];
		unsigned char b = data[++index];
		string huff = "";
		for (int j = 0; j < a; j++) {
			huff += to_string(data[++index]);
		}
		huff = huff.substr(0, huff.size() - b);
		huffmanTree->build_from_code(temp, huff);
	}
}

void Decompress::write_new_file(string lastname, unsigned char*& data, long int& index, long int length) {
	FILE* fw;
	fopen_s(&fw, lastname.c_str(), "wb");
	unsigned char* dt = new unsigned char[10 * length];
	long int index1 = 0;
	int comp = data[length - 1];
	HuffmanNode* curr = this->huffmanTree->root;
	index++;
	while (index < length - 2 ) {
		int code = data[index++];
		int mask = 1 << 7;
		for (int i = 0; i < 8; i++) {
			if ((mask & code) == 0) {
				curr = curr->lft;
			}
			else {
				curr = curr->rig;
			}
			if (curr->is_leaf()) {
				dt[index1++] = curr->ch;
				curr = this->huffmanTree->root;
			}
			mask = mask >> 1;
		}
	}
	int code = data[length - 2];
	int mask = 1 << 7;
	for (int i = 0; i < 8 - comp; i++) {
		if ((mask & code) == 0) {
			curr = curr->lft;
		}
		else {
			curr = curr->rig;
		}
		if (curr->is_leaf()) {
			dt[index1++] = curr->ch;
			curr = this->huffmanTree->root;
		}
		mask = mask >> 1;
	}
	fwrite(dt, 1, index1, fw);
	fclose(fw);
	delete[] dt;
}
void Decompress::decompress() {
	int a = filename.rfind(".");
	string pat = filename.substr(a + 1, filename.size() - a);
	if (strcmp(pat.c_str(), "lxy")) {
		cout << "error! the file form is not right!" << endl;
	}
	FILE* fp;
	fopen_s(&fp, filename.c_str(), "rb");
	assert(fp != NULL);
	long int idx = 0;
	long int Length;
	fseek(fp, 0, SEEK_END);
	Length = ftell(fp);
	cout << Length << endl;
	rewind(fp);
	while (idx < Length) {
		fseek(fp, idx, SEEK_SET);
		idx = idx + file_decompress(fp, idx) + 8;
	}
	fclose(fp);
}
int Decompress::file_decompress(FILE* fp, int idx) {
	string lastname;
	long int length;
	int sz;
	unsigned char t;
	fread(&t, 1, 1, fp);
	sz = (int)t;
	sz++;
	unsigned char* data;
	data = new unsigned char[sz];
	fread(data, 1, sz, fp);
	long int index = -1;
	for (int i = 0; i < sz; i++) {
		lastname.push_back(data[++index]);
	}
	delete[]data;
	int a[10];
	int j = 1;
	a[0] = -1;
	for (int i = 0; i < lastname.size(); i++) {
		if (lastname[i] == '/') {
			a[j] = i;
			j++;
		}
	}
	for (int i = 1; i < j; i++) {
		int b = a[i];
		string temp = lastname.substr(0, b + 1);
		DIR* mydir = opendir(temp.c_str());
		if (mydir == NULL) {
			_mkdir(temp.c_str());
		}
	}
	if (lastname[lastname.size() - 1] == '/') {
		return sz + 1;
	}
	lastname[sz - 1] = '\0';
	fseek(fp, idx + sz + 1, SEEK_SET);
	fread(&length, 8, 1, fp);
	if (length == 0) {
		FILE* fw;
		fopen_s(&fw, lastname.c_str(), "wb");
		fclose(fw);
		return sz + 1;
	}
	data = new unsigned char[length];
	fread(data, 1, length, fp);
	index = -1;
	huffmanTree = new HuffmanTree;
	decode_huffman_tree(data, index);
	write_new_file(lastname, data, index, length);
	//cout << lastname << " is created!!" << endl;
	delete[]data;
	return sz + length + 1;
}
void _compress() {
	clock_t start, finish;
	double time;
	string filename;
	cout << "please input file's name to compress" << endl;
	getline(cin, filename);
	start = clock();
	Compress tet(filename);
	tet.compress();
	finish = clock();
	time = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "compress time:  " << time << endl;
}
void _decompress() {
	clock_t start, finish;
	double time;
	string test;
	cout << "please input file's name to decompress" << endl;
	getline(cin,test);
	start = clock();
	Decompress tet(test);
	tet.decompress();
	finish = clock();
	time = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "decompress time:  " << time << endl;
}
int main() {
	_compress();
	_decompress();
}

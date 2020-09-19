#ifndef VARINT_H
#define VARINT_H 

#include <iostream>
#include <vector>

#include <iomanip>

template <class T>
class Varint {

private:
	T inputBytes;
	std::vector<size_t> startIndexes;
	std::vector<unsigned char> result;
	void setStartIndexes();
	void processBytes();
	void base128To256(const T& b128, T& b256);

public:
	Varint(T _inputCollection);
	void outputResult();
	ssize_t decode(size_t start, std::vector<unsigned char>& result);
	void shiftAllBytesRight(size_t shift);
	void static shiftAllBytesRight(T& bytes, size_t shift);
};

#endif /* VARINT_H */

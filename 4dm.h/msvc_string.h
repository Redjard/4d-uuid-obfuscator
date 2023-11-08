
class msvc_string {
// public:
	char _content[0x10]; // this could contain a pointer
	uint64_t _length;  // 0x10
	uint64_t _maxlength;  // 0x18
	
	bool isSSO(){
		return this->_maxlength < 0x10;
	}
	char* read() {
		if ( this->isSSO() )
			return this->_content;
		return *reinterpret_cast<char**>(this->_content);
	}
	msvc_string write( const char* str, uint64_t length ) {
		
		uint64_t actualLength = length + 1;  // including NULL terminator
		
		if ( length > this->_maxlength ) {
			if (!this->isSSO())
				delete[] this->data();
			
			uint64_t newAlloc = ( (actualLength-1)/0x10 + 1 )*0x10;  // allocate up to next 0x10
			
			this->_maxlength = newAlloc - 1;
			*reinterpret_cast<char**>(this->_content) = new char[newAlloc];
		}
		
		this->_length = length;
		memcpy(this->data(),str,length);
		this->data()[actualLength] = NULL;
		
		return *this;
	}
	
public:
	uint64_t length() {
		return this->_length;
	}
	char* data() {
		return this->read();
	}
	const char* c_str() {
		return this->data();
	}
	char& operator[](uint64_t index) {
		return this->data()[index];
	}
	
	operator std::string() {
		return std::string(this->data());
	}
	
	msvc_string operator=(msvc_string str) {
		return this->write(str.data(),str.length());
	}
	msvc_string operator=(const char* str) {
		return this->write(str,strlen(str));
	}
	msvc_string operator=(std::string str) {
		return this->write(str.c_str(),str.length());
	}
	msvc_string* operator=(auto arg) {
		static_assert(false,"assignment with this type of argument not yet implemented");
	}
	
	bool operator==(std::string str) {
		return str == std::string(*this);
	}
};

std::ostream& operator<<(std::ostream& os, msvc_string str) {
	os << str.data();
	return os;
}

/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <sstream>
#include <SFML/System/InputStream.hpp>
#include <boost/filesystem/path.hpp>

class cScenario;
class cUniverse;

namespace fs = boost::filesystem; // TODO: Centralize this alias!

bool load_scenario(fs::path file_to_load, cScenario& scenario);

bool load_party(fs::path file_to_load, cUniverse& univ);
bool save_party(fs::path dest_file, const cUniverse& univ);

void init_directories(const char* exec_path);
void check_for_intel();

std::string read_maybe_quoted_string(std::istream& from);
std::string maybe_quote_string(std::string which);

template<typename T, int D>
void writeArray(std::ostream& to, const T(* array)[D], int width, int height) {
	using int_type = decltype(T() + 1);
	for(int y = 0; y < height; y++) {
		to << array[0][y];
		for(int x = 1; x < width; x++)
			to << '\t' << int_type(array[x][y]);
		to << '\n';
	}
	to << '\f';
}

template<typename T, int D>
void readArray(std::istream& from, T(* array)[D], int width, int height) {
	using int_type = decltype(T() + 1);
	from >> std::ws;
	std::string arrayContents;
	getline(from, arrayContents, '\f');
	std::istringstream arrayIn(arrayContents);
	for(int y = 0; y < height; y++) {
		std::string line;
		getline(arrayIn, line);
		std::istringstream lineIn(line);
		lineIn.flags(from.flags());
		int_type temp;
		for(int x = 0; x < width; x++)
			lineIn >> temp, array[x][y] = temp;
		if(!arrayIn) break;
	}
}

// SFML doesn't support standard C++ streams, so I need to do this in order to load images from a stream.
class StdInputStream : public sf::InputStream {
	std::istream& stream;
public:
	StdInputStream(std::istream& stream) : stream(stream) {}
	virtual ~StdInputStream() {}
	// TODO: All four of these functions should return -1 on failure, but I'm not quite sure which conditions should be counted as failure
	virtual sf::Int64 read(void *data, sf::Int64 size) {
		stream.read((char*)data, size);
		return stream.gcount();
	}
	virtual sf::Int64 seek(sf::Int64 position) {
		stream.seekg(position);
		return tell();
	}
	virtual sf::Int64 tell() {
		return stream.tellg();
	}
	virtual sf::Int64 getSize() {
		auto was_at = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto pos = stream.tellg();
		stream.seekg(was_at);
		return pos;
	}
};